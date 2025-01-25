#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "cLoadModels.h"
#include "FlyCam.h"
#include "cShaderCompiler.h"
#include "cVAOManager.h"
#include "cScene.h"
#include "IPlayer.h"
#include "cPlayer.h"
#include "cAiEnemy.h"
#include "cLightManager.h"
#include "cLightMover.h"
#include "cPhysicsUpdated.h"
#include "cRenderModel.h"
#include "cLua.h"
#include "iTank.h"
#include "cPlayerTank.h"
#include "cEnemyTank.h"
#include "cSuperTank.h"

bool LoadMap(const std::string& filename); // Forward declaration
bool IsGridEmpty(int playerX, int playerY); // Forward declaration

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version (3.3 core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Refer cShaderCompiler class for more info.
    cShaderCompiler shader;
    GLuint shaderProgram = shader.CompileShader();

    // Import Scene
    cScene scene;
    scene.CreateScene("../sceneFileWithNormals.txt");
    //scene.ExportMaterial(shaderProgram, scene.numberOfMeshesToLoad);            // Considering number of materials = number of meshes to load

    cVAOManager VAOManager;
    for (int i = 0; i != scene.pModels.size(); i++) {
        VAOManager.GettingModelReadyToRender(scene.pModels[i]);         // This thing is new just because I created whole new VAO thing which creates several different VAOs and now I can render a single model multiple times
    }

    cRenderModel renderer;

    cLightManager lightManager;
    lightManager.LoadLights("../lightsFile.txt");
    // Camera Initialization
    FlyCam flyCam(800, 600, glm::vec3(-51.65f, 31.0f, -75.64f), 90.0f);                   //    FlyCam flyCam(800, 600, glm::vec3(28.0f, 45.0f, -136.0f), 90.0f);

    flyCam.camSpeed = 1.0f;

    cLightMover lightMover(lightManager, flyCam, 1);


    float deltaTime = 0;
    float startTime, endTime;

    for (int modelIndex = 0; modelIndex != scene.numberOfMeshesToLoad; modelIndex++) {
        scene.pModels[modelIndex].pTransformedVertices = new cLoadModels::sTransformedVertex[scene.pModels[modelIndex].numberOfVertices];
    
        glm::mat4 model = scene.pModels[modelIndex].CreateModelMatrix(shaderProgram, scene.pModels[modelIndex]);      // Creation of model matrix with arguements passed in sceneFile.txt
        scene.pModels[modelIndex].GenerateTransformedVertices(model);
    }   // Used for initializing the pTransformedVertices, Nothing new xD

    // Starting physics
    cPhysicsUpdated physicsEngine(scene);

    // Loading Maze
    LoadMap("../Question1/Maze_1286580.txt");


    startTime = glfwGetTime();
    


    cPlayerTank player(&scene.pModels[3]);
    cEnemyTank enemy1(&scene.pModels[1]);
    cEnemyTank enemy2(&scene.pModels[2]);
    cEnemyTank enemy3(&scene.pModels[4]);
    cEnemyTank enemy4(&scene.pModels[5]);
    cEnemyTank enemy5(&scene.pModels[6]);
    cSuperTank superTank(&scene.pModels[6]);

    std::vector<cEnemyTank> vecTanks;

    //vecTanks.push_back(player);
    vecTanks.push_back(enemy1);
    vecTanks.push_back(enemy2);
    vecTanks.push_back(enemy3);
    vecTanks.push_back(enemy4);
    vecTanks.push_back(enemy5);

    // -----------------------------------------------LUA----------------------------------------------
    
    cLua lua;
    lua_State* state = luaL_newstate();
    lua.InitLua(state);
    //lua.ExecuteLuaScript(state, "game_logic.lua");      // Need to implement this script game_logic.lua

    // -----------------------------------------------LUA----------------------------------------------
    bool bIsFKeyPressed = false; 
    bool bIsRKeyPressed = false;
    bool bDidFire = false;
    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // Input handling
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        glfwGetWindowSize(window, &flyCam.camWidth, &flyCam.camHeight);

        std::string cameraPositionInTitle = "Camera Location: (" + to_string(flyCam.camLocation.x) + ", " + to_string(flyCam.camLocation.y) + ", " + to_string(flyCam.camLocation.z) + ") Camera Speed: " + to_string(flyCam.camSpeed) + " Camera Roll: " + to_string(flyCam.camRoll) + " Cam yaw: " + to_string(flyCam.camYaw) + " Cam pitch: " + to_string(flyCam.camPitch);
        glfwSetWindowTitle(window, cameraPositionInTitle.c_str());

        endTime = glfwGetTime();
        deltaTime = endTime - startTime;
        startTime = endTime;
        //std::cout << deltaTime << " Time passed" << std::endl;
        
        flyCam.cameraMatrix(45.0f, 0.1f, 10000.0f, shaderProgram, "camMatrix", window);

        // Rendering commands here
        glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);   // Enable depth testing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader program and draw the triangle
        glUseProgram(shaderProgram);

        // ------------------------------------------------------------------------------------------------------------------------------
        // You can call movement functions from light mover class for lights here now and then call turn on lights function of light manager
        lightManager.TurnOnLights(shaderProgram, 1);
        // ------------------------------------------------------------------------------------------------------------------------------
        // You can create player objects here and make them move from here
        // ------------------------------------------------------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            player.MoveLeft(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            player.MoveRight(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            player.MoveForward(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            player.MoveBackward(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            if (!bIsFKeyPressed) {
                player.Shoot(deltaTime, &scene.pModels[7]);
                bIsFKeyPressed = true;
            }
            //renderer.DrawDebugSphere(&scene.pModels[7], glm::vec3(scene.pModels[7].pMeshTransform.x, scene.pModels[7].pMeshTransform.y, scene.pModels[7].pMeshTransform.z), glm::vec4(1, 0, 0, 1), 1.0f, shaderProgram);
        }
        else {
            bIsFKeyPressed = false;
        }
        if(player.vecProjectiles.size() == 1)
            player.Update(deltaTime);
        for (int i = 0; i != player.vecProjectiles.size(); i++) {
            renderer.DrawDebugSphere(player.vecProjectiles[i], glm::vec3(player.vecProjectiles[i]->pMeshTransform.x, player.vecProjectiles[i]->pMeshTransform.y, player.vecProjectiles[i]->pMeshTransform.z), glm::vec4(1, 0, 0, 1), 1.0f, shaderProgram);
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !player.isCharging) {
            player.StartCharging();

        }
        player.UpdateCharge(deltaTime);

        if (player.IsFullyCharged()) {
            player.FireLaser(&scene.pModels[7], 10.0f, 0.1f); // Laser length and sphere interval
        }

        player.UpdateLaser(deltaTime);

        // Render Laser
        if (player.vecLaserSpheres.size() > 0) {
            for (int i = 0; i != player.vecLaserSpheres.size(); i++) {
                renderer.DrawDebugSphere(player.vecLaserSpheres[i], glm::vec3(player.vecLaserSpheres[i]->pMeshTransform.x, player.vecLaserSpheres[i]->pMeshTransform.y, player.vecLaserSpheres[i]->pMeshTransform.z), glm::vec4(1, 0, 0, 1), 1.0f, shaderProgram);
            }
        }


        vecTanks[0].forwardVector = enemy1.forwardVector;
        vecTanks[1].forwardVector = enemy2.forwardVector;
        vecTanks[2].forwardVector = enemy3.forwardVector;
        vecTanks[3].forwardVector = enemy4.forwardVector;
        vecTanks[4].forwardVector = enemy5.forwardVector;

        vecTanks[0].SetPosition(enemy1.GetPosition());
        vecTanks[1].SetPosition(enemy2.GetPosition());
        vecTanks[2].SetPosition(enemy3.GetPosition());
        vecTanks[3].SetPosition(enemy4.GetPosition());
        vecTanks[4].SetPosition(enemy5.GetPosition());

        for (int i = 0; i != vecTanks.size(); i++) {
            bool projectileFired = false;
            for (int j = 0; j != vecTanks.size(); j++) {
                if (i != j && vecTanks[i].GetPosition().x == vecTanks[j].GetPosition().x ||
                    vecTanks[i].GetPosition().y == vecTanks[j].GetPosition().y &&
                    vecTanks[i].GetPosition().x == player.GetPosition().x ||
                    vecTanks[i].GetPosition().y == player.GetPosition().y) {

                    // ---------------------------EnemyLaser---------------------------------
                    vecTanks[i].StartCharging();
                    // ---------------------------EnemyLaser---------------------------------

                    glm::vec3 directionToTank = vecTanks[j].GetPosition() - vecTanks[i].GetPosition();
                    float dotProduct = glm::dot(vecTanks[j].forwardVector, directionToTank);


                    if (dotProduct > 0.9f) {
                        if (!projectileFired && vecTanks[i].vecProjectiles.size() == 0) {
                            // Validate forward vector
                            if (glm::length(vecTanks[i].forwardVector) == 0) {
                                std::cout << "Warning: Tank's forward vector is zero!" << std::endl;
                                continue; // Skip this tank
                            }

                            if (vecTanks[i].health > 0) {
                                vecTanks[i].Shoot(deltaTime, &scene.pModels[7]);
                            }
                            projectileFired = true;
                            for (int k = 0; k != vecTanks[i].vecProjectiles.size(); k++) {
                                if (glm::distance(vecTanks[j].GetPosition(), glm::vec3(vecTanks[i].vecProjectiles[k]->pMeshTransform.x, vecTanks[i].vecProjectiles[k]->pMeshTransform.y, vecTanks[i].vecProjectiles[k]->pMeshTransform.z)) <= 0.1f) {
                                    vecTanks[j].TakeHit(deltaTime);
                                    vecTanks[i].vecProjectiles.erase(vecTanks[i].vecProjectiles.begin() + k);
                                    k--;
                                    break;
                                }
                            }
                            if (vecTanks[j].health <= 0 && vecTanks[i].killCount == 1) {
                                cSuperTank* superTank = new cSuperTank(vecTanks[i].GetModel()); // Copy existing tank attributes
                                vecTanks[i] = *dynamic_cast<cEnemyTank*>(superTank);
                                superTank->UpdateMissiles(deltaTime);
                                for (const std::vector<cSuperTank::sTrackingMissile>::iterator::value_type& missile : superTank->vecTrackingMissiles) {
                                    renderer.DrawDebugSphere(&scene.pModels[7], missile.position, glm::vec4(1, 0, 0, 1), 1.0f, shaderProgram);
                                }
                            }
                        }
                    }
                }
                for (int i = 0; i != player.vecProjectiles.size(); i++) {
                    if (player.vecProjectiles.size() > 0) {
                        if (glm::distance(enemy1.GetPosition(), glm::vec3(player.vecProjectiles[i]->pMeshTransform.x, player.vecProjectiles[i]->pMeshTransform.y, player.vecProjectiles[i]->pMeshTransform.z)) <= 0.1f) {
                            enemy1.TakeHit(deltaTime);
                            player.vecProjectiles.erase(player.vecProjectiles.begin() + i);
                            i--;
                            break;
                        }
                        if (glm::distance(enemy2.GetPosition(), glm::vec3(player.vecProjectiles[i]->pMeshTransform.x, player.vecProjectiles[i]->pMeshTransform.y, player.vecProjectiles[i]->pMeshTransform.z)) <= 0.1f) {
                            enemy2.TakeHit(deltaTime);
                            player.vecProjectiles.erase(player.vecProjectiles.begin() + i);
                            i--;
                            break;
                        }
                        if (glm::distance(enemy3.GetPosition(), glm::vec3(player.vecProjectiles[i]->pMeshTransform.x, player.vecProjectiles[i]->pMeshTransform.y, player.vecProjectiles[i]->pMeshTransform.z)) <= 0.1f) {
                            enemy3.TakeHit(deltaTime);
                            player.vecProjectiles.erase(player.vecProjectiles.begin() + i);
                            i--;
                            break;
                        }
                        if (glm::distance(enemy4.GetPosition(), glm::vec3(player.vecProjectiles[i]->pMeshTransform.x, player.vecProjectiles[i]->pMeshTransform.y, player.vecProjectiles[i]->pMeshTransform.z)) <= 0.1f) {
                            enemy4.TakeHit(deltaTime);
                            player.vecProjectiles.erase(player.vecProjectiles.begin() + i);
                            i--;
                            break;
                        }
                        if (glm::distance(enemy5.GetPosition(), glm::vec3(player.vecProjectiles[i]->pMeshTransform.x, player.vecProjectiles[i]->pMeshTransform.y, player.vecProjectiles[i]->pMeshTransform.z)) <= 0.1f) {
                            enemy5.TakeHit(deltaTime);
                            player.vecProjectiles.erase(player.vecProjectiles.begin() + i);
                            i--;
                            break;
                        }
                    }
                }
            }
            vecTanks[i].UpdateCharge(deltaTime);
            if (vecTanks[i].IsFullyCharged()) {
                //std::cout << "Tank " << i << " fully charged, firing laser!" << std::endl;
                vecTanks[i].FireLaser(&scene.pModels[7], 10.0f, 0.1f); // Laser length and sphere interval
            }
            vecTanks[i].UpdateLaser(deltaTime);
        }

        enemy1.UpdateProjectiles(deltaTime);
        enemy2.UpdateProjectiles(deltaTime);
        enemy3.UpdateProjectiles(deltaTime);
        enemy4.UpdateProjectiles(deltaTime);
        enemy5.UpdateProjectiles(deltaTime);
        for (int j = 0; j != vecTanks.size(); j++) {

            // Render EnemyLaser
            if (vecTanks[j].vecLaserSpheres.size() > 0) {
                for(int i = 0; i != vecTanks[j].vecLaserSpheres.size(); i++) {

                    renderer.DrawDebugSphere(vecTanks[j].vecLaserSpheres[i], glm::vec3(vecTanks[j].vecLaserSpheres[i]->pMeshTransform.x, vecTanks[j].vecLaserSpheres[i]->pMeshTransform.y, vecTanks[j].vecLaserSpheres[i]->pMeshTransform.z), glm::vec4(1, 0, 0, 1), 1.0f, shaderProgram);
                }
            }
            if (!vecTanks[j].vecProjectiles.empty()) {
                // Update the projectile's position
                vecTanks[j].vecProjectiles[0]->pMeshTransform.x += vecTanks[j].vecProjectileDirections[0].x * deltaTime * 12 * vecTanks[j].GetSpeed();
                vecTanks[j].vecProjectiles[0]->pMeshTransform.y += vecTanks[j].vecProjectileDirections[0].y * deltaTime * 12 * vecTanks[j].GetSpeed();

                //-------------------------------------PlayerDamage-------------------------------------------------------------------------
                for (int k = 0; k != vecTanks[j].vecProjectiles.size(); k++) {
                    if (glm::distance(player.GetPosition(), glm::vec3(vecTanks[j].vecProjectiles[k]->pMeshTransform.x, vecTanks[j].vecProjectiles[k]->pMeshTransform.y, vecTanks[j].vecProjectiles[k]->pMeshTransform.z)) <= 0.1f) {
                        player.TakeHit(deltaTime);
                        vecTanks[j].vecProjectiles.erase(vecTanks[j].vecProjectiles.begin() + k);
                        k--;
                        break;
                    }
                }
                //-------------------------------------PlayerDamage-------------------------------------------------------------------------

                if (IsGridEmpty(vecTanks[j].vecProjectiles[0]->pMeshTransform.x, vecTanks[j].vecProjectiles[0]->pMeshTransform.y) && vecTanks[j].vecProjectiles.size() != 0) {

                    // Render the projectile
                    renderer.DrawDebugSphere(
                        vecTanks[j].vecProjectiles[0],
                        glm::vec3(
                            vecTanks[j].vecProjectiles[0]->pMeshTransform.x,
                            vecTanks[j].vecProjectiles[0]->pMeshTransform.y,
                            vecTanks[j].vecProjectiles[0]->pMeshTransform.z
                        ),
                        glm::vec4(1, 0, 0, 1), // Red color
                        1.0f,
                        shaderProgram
                    );
                }
                else if (!IsGridEmpty(vecTanks[j].vecProjectiles[0]->pMeshTransform.x, vecTanks[j].vecProjectiles[0]->pMeshTransform.y) && vecTanks[j].vecProjectiles.size() != 0) {
                    delete vecTanks[j].vecProjectiles[0];
                    vecTanks[j].vecProjectiles.erase(vecTanks[j].vecProjectiles.begin() + 0);
                    vecTanks[j].vecProjectileDirections.erase(vecTanks[j].vecProjectileDirections.begin() + 0);
                }
            }
        }


        enemy1.Update(deltaTime);
        enemy2.Update(deltaTime);
        enemy3.Update(deltaTime);
        enemy4.Update(deltaTime);
        enemy5.Update(deltaTime);

        if (player.health > 0) {
            if (player.health <= 100 && player.health > 70) {
                renderer.DrawDebugSphere(&scene.pModels[8], glm::vec3(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (player.health <= 70 && player.health > 50) {
                renderer.DrawDebugSphere(&scene.pModels[9], glm::vec3(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (player.health <= 50 && player.health > 30) {
                renderer.DrawDebugSphere(&scene.pModels[10], glm::vec3(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (player.health <= 30 && player.health > 0) {
                renderer.DrawDebugSphere(&scene.pModels[11], glm::vec3(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (player.health == 0) {
                renderer.DrawDebugSphere(&scene.pModels[12], glm::vec3(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            renderer.Render(shaderProgram, player.GetModel());
        }
        if (enemy1.health > 0) {
            if (enemy1.health <= 100 && enemy1.health > 70) {
                renderer.DrawDebugSphere(&scene.pModels[8], glm::vec3(enemy1.GetPosition().x, enemy1.GetPosition().y, enemy1.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy1.health <= 70 && enemy1.health > 50) {
                renderer.DrawDebugSphere(&scene.pModels[9], glm::vec3(enemy1.GetPosition().x, enemy1.GetPosition().y, enemy1.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy1.health <= 50 && enemy1.health > 30) {
                renderer.DrawDebugSphere(&scene.pModels[10], glm::vec3(enemy1.GetPosition().x, enemy1.GetPosition().y, enemy1.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy1.health <= 30 && enemy1.health > 0) {
                renderer.DrawDebugSphere(&scene.pModels[11], glm::vec3(enemy1.GetPosition().x, enemy1.GetPosition().y, enemy1.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy1.health == 0) {
                renderer.DrawDebugSphere(&scene.pModels[12], glm::vec3(enemy1.GetPosition().x, enemy1.GetPosition().y, enemy1.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            renderer.Render(shaderProgram, enemy1.GetModel());
        }
        if (enemy2.health > 0) {
            if (enemy2.health <= 100 && enemy2.health > 70) {
                renderer.DrawDebugSphere(&scene.pModels[8], glm::vec3(enemy2.GetPosition().x, enemy2.GetPosition().y, enemy2.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy2.health <= 70 && enemy2.health > 50) {
                renderer.DrawDebugSphere(&scene.pModels[9], glm::vec3(enemy2.GetPosition().x, enemy2.GetPosition().y, enemy2.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy2.health <= 50 && enemy2.health > 30) {
                renderer.DrawDebugSphere(&scene.pModels[10], glm::vec3(enemy2.GetPosition().x, enemy2.GetPosition().y, enemy2.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy2.health <= 30 && enemy2.health > 0) {
                renderer.DrawDebugSphere(&scene.pModels[11], glm::vec3(enemy2.GetPosition().x, enemy2.GetPosition().y, enemy2.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy2.health == 0) {
                renderer.DrawDebugSphere(&scene.pModels[12], glm::vec3(enemy2.GetPosition().x, enemy2.GetPosition().y, enemy2.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            renderer.Render(shaderProgram, enemy2.GetModel());
        }
        if (enemy3.health > 0) {
            if (enemy3.health <= 100 && enemy3.health > 70) {
                renderer.DrawDebugSphere(&scene.pModels[8], glm::vec3(enemy3.GetPosition().x, enemy3.GetPosition().y, enemy3.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy3.health <= 70 && enemy3.health > 50) {
                renderer.DrawDebugSphere(&scene.pModels[9], glm::vec3(enemy3.GetPosition().x, enemy3.GetPosition().y, enemy3.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy3.health <= 50 && enemy3.health > 30) {
                renderer.DrawDebugSphere(&scene.pModels[10], glm::vec3(enemy3.GetPosition().x, enemy3.GetPosition().y, enemy3.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy3.health <= 30 && enemy3.health > 0) {
                renderer.DrawDebugSphere(&scene.pModels[11], glm::vec3(enemy3.GetPosition().x, enemy3.GetPosition().y, enemy3.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy3.health == 0) {
                renderer.DrawDebugSphere(&scene.pModels[12], glm::vec3(enemy3.GetPosition().x, enemy3.GetPosition().y, enemy3.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            renderer.Render(shaderProgram, enemy3.GetModel());
        }
        if (enemy4.health > 0) {
            if (enemy4.health <= 100 && enemy4.health > 70) {
                renderer.DrawDebugSphere(&scene.pModels[8], glm::vec3(enemy4.GetPosition().x, enemy4.GetPosition().y, enemy4.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy4.health <= 70 && enemy4.health > 50) {
                renderer.DrawDebugSphere(&scene.pModels[9], glm::vec3(enemy4.GetPosition().x, enemy4.GetPosition().y, enemy4.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy4.health <= 50 && enemy4.health > 30) {
                renderer.DrawDebugSphere(&scene.pModels[10], glm::vec3(enemy4.GetPosition().x, enemy4.GetPosition().y, enemy4.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy4.health <= 30 && enemy4.health > 0) {
                renderer.DrawDebugSphere(&scene.pModels[11], glm::vec3(enemy4.GetPosition().x, enemy4.GetPosition().y, enemy4.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy4.health == 0) {
                renderer.DrawDebugSphere(&scene.pModels[12], glm::vec3(enemy4.GetPosition().x, enemy4.GetPosition().y, enemy4.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            renderer.Render(shaderProgram, enemy4.GetModel());
        }
        if (enemy5.health > 0) {
            if (enemy5.health <= 100 && enemy5.health > 70) {
                renderer.DrawDebugSphere(&scene.pModels[8], glm::vec3(enemy5.GetPosition().x, enemy5.GetPosition().y, enemy5.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy5.health <= 70 && enemy5.health > 50) {
                renderer.DrawDebugSphere(&scene.pModels[9], glm::vec3(enemy5.GetPosition().x, enemy5.GetPosition().y, enemy5.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy5.health <= 50 && enemy5.health > 30) {
                renderer.DrawDebugSphere(&scene.pModels[10], glm::vec3(enemy5.GetPosition().x, enemy5.GetPosition().y, enemy5.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy5.health <= 30 && enemy5.health > 0) {
                renderer.DrawDebugSphere(&scene.pModels[11], glm::vec3(enemy5.GetPosition().x, enemy5.GetPosition().y, enemy5.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            if (enemy5.health == 0) {
                renderer.DrawDebugSphere(&scene.pModels[12], glm::vec3(enemy5.GetPosition().x, enemy5.GetPosition().y, enemy5.GetPosition().z - 0.8), glm::vec4(1, 0, 0, 1), 0.01f, shaderProgram);
            }
            renderer.Render(shaderProgram, enemy5.GetModel());
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        for (int i = 0; i != 1; i++) {
            renderer.Render(shaderProgram, &scene.pModels[i]);
        }
        // Swap buffers and poll IO events (keys pressed/released, mouse moved, etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // -----------------------------------------------LUA----------------------------------------------
    
    lua_close(state);   // Lua cleanup

    // -----------------------------------------------LUA----------------------------------------------

    // Cleanup
    VAOManager.VAOVBOCleanup();
    
    // Terminate GLFW
    glfwTerminate();
    return 0;
}