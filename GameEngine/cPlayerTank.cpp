#include "cPlayerTank.h"

bool IsGridEmpty(int playerX, int playerY);     // Forward Declaration

void cPlayerTank::StartCharging()
{
    if (health <= 0) return;

    isCharging = true;
    currentChargeTime = 0.0f;
}

bool cPlayerTank::IsFullyCharged() const
{
    if (health <= 0) return false;

    return currentChargeTime >= chargeTime;
}

void cPlayerTank::UpdateCharge(float deltaTime)
{
    if (health <= 0) return;

    if (isCharging) {
        currentChargeTime += deltaTime;
        if (currentChargeTime >= chargeTime) {
            isCharging = false; // Ready to fire
        }
    }
}

cPlayerTank::cPlayerTank(cLoadModels* model) : playerModel(*model), playerPosition(glm::vec3(model->pMeshTransform.x, model->pMeshTransform.y, model->pMeshTransform.z))
{
    this->health = 100;
    vecProjectiles.clear();
    vecProjectileDirections.clear();
    vecLaserSpheres.clear();
}

cPlayerTank::~cPlayerTank()
{
}

cLoadModels* cPlayerTank::GetModel()
{
    return &playerModel;
}

void cPlayerTank::SetModel(cLoadModels& model)
{
    this->playerModel = model;
}

void cPlayerTank::MoveLeft(float deltaTime) {
    if (health <= 0) return;

    //std::cout << "Player Pos: (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;
    if (CanMove(deltaTime) && !isCharging && health != 0) {
        if (IsGridEmpty(playerPosition.x + 1, playerPosition.y)) {
            forwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
            playerModel.pMeshTransform.zRotation = 90.0f;
            //playerPosition += forwardVector * speed;
            playerPosition.x = (int)playerPosition.x + 1;
            playerPosition.y = (int)playerPosition.y;
            playerModel.pMeshTransform.x = playerPosition.x;
            playerModel.pMeshTransform.y = playerPosition.y;
            playerModel.pMeshTransform.z = playerPosition.z;
        }
    }
}

void cPlayerTank::MoveRight(float deltaTime) {
    if (health <= 0) return;

    //std::cout << "Player Pos: (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;
    if (CanMove(deltaTime) && !isCharging && health != 0) {
        if (IsGridEmpty(playerPosition.x - 1, playerPosition.y)) {
            forwardVector = glm::vec3(-1.0f, 0.0f, 0.0f);
            playerModel.pMeshTransform.zRotation = -90.0f;
            //playerPosition += forwardVector * speed * deltaTime;
            playerPosition.x = (int)playerPosition.x - 1;
            playerPosition.y = (int)playerPosition.y;
            playerModel.pMeshTransform.x = playerPosition.x;
            playerModel.pMeshTransform.y = playerPosition.y;
            playerModel.pMeshTransform.z = playerPosition.z;
        }
    }
}

void cPlayerTank::MoveForward(float deltaTime) { // Move Up
    if (health <= 0) return;

    //std::cout << "Player Pos: (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;
    if (CanMove(deltaTime) && !isCharging && health != 0) {
        if (IsGridEmpty(playerPosition.x, playerPosition.y + 1)) {
            forwardVector = glm::vec3(0.0f, 1.0f, 0.0f);
            playerModel.pMeshTransform.zRotation = 180.0f;
            //playerPosition += forwardVector * speed;
            playerPosition.x = (int)playerPosition.x;
            playerPosition.y = (int)playerPosition.y + 1;
            playerModel.pMeshTransform.x = playerPosition.x;
            playerModel.pMeshTransform.y = playerPosition.y;
            playerModel.pMeshTransform.z = playerPosition.z;
        }
    }
}

void cPlayerTank::MoveBackward(float deltaTime) { // Move Down
    if (health <= 0) return;

    //std::cout << "Player Pos: (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;
    if (CanMove(deltaTime) && !isCharging && health != 0) {
        if (IsGridEmpty(playerPosition.x, playerPosition.y - 1)) {
            forwardVector = glm::vec3(0.0f, -1.0f, 0.0f);
            playerModel.pMeshTransform.zRotation = 0.0f;
            //playerPosition += forwardVector * speed;
            playerPosition.x = (int)playerPosition.x;
            playerPosition.y = (int)playerPosition.y - 1;
            playerModel.pMeshTransform.x = playerPosition.x;
            playerModel.pMeshTransform.y = playerPosition.y;
            playerModel.pMeshTransform.z = playerPosition.z;
        }
    }
}

void cPlayerTank::Shoot(float deltaTime, cLoadModels* projectile)
{
    if (health <= 0) return;

    if (vecProjectiles.size() == 1) {
        return;
    }
    glm::vec3 bulletFront = glm::vec3(forwardVector);
    cLoadModels* newProjectile = new cLoadModels(*projectile);
    newProjectile->pMeshTransform.x = playerPosition.x;
    newProjectile->pMeshTransform.y = playerPosition.y;
    newProjectile->pMeshTransform.z = playerPosition.z;
    vecProjectileDirections.push_back(bulletFront);
    vecProjectiles.push_back(newProjectile);
}

void cPlayerTank::TakeHit(float deltaTime)
{
    if (health <= 0) return;

    if (health > 0) {
        health -= 10;
        std::cout << "Current health: " << health << std::endl;
    }
    if (health == 0) {
        std::cout << "Player is dead" << std::endl;
    }
}

glm::vec3 cPlayerTank::GetPosition()
{
    return playerPosition;
}

void cPlayerTank::SetPosition(glm::vec3 position)
{
    this->playerPosition = position;
}

float cPlayerTank::GetSpeed()
{
    return speed;
}

void cPlayerTank::SetSpeed(float speed)
{
    this->speed = speed;
}

bool cPlayerTank::CanMove(float deltaTime)
{
    float movementAccumulator = 0.0f;
    movementAccumulator += deltaTime * speed;
    return (movementAccumulator >= 0.01f);
}

void cPlayerTank::Update(float deltaTime)
{
    if (health <= 0) return;

    for (int i = 0; i != vecProjectiles.size(); i++) {
        if (vecProjectiles.size() > 0) {
            if (!IsGridEmpty(vecProjectiles[i]->pMeshTransform.x, vecProjectiles[i]->pMeshTransform.y)) {
                delete vecProjectiles[i];
                vecProjectiles.erase(vecProjectiles.begin() + i);
                vecProjectileDirections.erase(vecProjectileDirections.begin() + i);
            }
            else {
                vecProjectiles[i]->pMeshTransform.x += vecProjectileDirections[i].x * deltaTime * 12 * speed;
                vecProjectiles[i]->pMeshTransform.y += vecProjectileDirections[i].y * deltaTime * 12 * speed;
            }
        }
        else {
            return;
        }
    }
}

void cPlayerTank::FireLaser(cLoadModels* laserSphere, float laserLength, float interval)
{
    if (health <= 0) return;

    if (!IsFullyCharged()) return;

    glm::vec3 laserFront = forwardVector; // Direction of the laser
    glm::vec3 startPosition = playerPosition;

    vecLaserSpheres.clear(); // Clear any previous laser data

    for (float distance = 0.0f; distance <= laserLength; distance += interval) {
        cLoadModels* newLaserSphere = new cLoadModels(*laserSphere);

        glm::vec3 spherePosition = startPosition + laserFront * distance;

        newLaserSphere->pMeshTransform.x = spherePosition.x;
        newLaserSphere->pMeshTransform.y = spherePosition.y;
        newLaserSphere->pMeshTransform.z = spherePosition.z;

        vecLaserSpheres.push_back(newLaserSphere);
    }

    isCharging = false; // Reset charging state
}

void cPlayerTank::UpdateLaser(float deltaTime)
{
    if (health <= 0) return;

    if (vecLaserSpheres.empty()) return;

    laserTimer += deltaTime;
    if (laserTimer >= laserVisibleTime) {
        for (std::vector<cLoadModels*>::iterator::value_type sphere : vecLaserSpheres) {
            delete sphere;
        }
        vecLaserSpheres.clear();
        laserTimer = 0.0f; // Reset timer
        currentChargeTime = 0.0f;
    }
}
