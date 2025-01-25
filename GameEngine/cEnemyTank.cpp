#include "cEnemyTank.h"

bool IsGridEmpty(int playerX, int playerY);     // Forward Declaration

void cEnemyTank::StartCharging()
{
    if (health <= 0) return;

    isCharging = true;
    currentChargeTime = 0.0f;
}

bool cEnemyTank::IsFullyCharged() const
{
    if (health <= 0) return false;

    return currentChargeTime >= chargeTime;
}

void cEnemyTank::UpdateCharge(float deltaTime)
{
    if (health <= 0) return;

    if (isCharging) {
        currentChargeTime += deltaTime;
        if (currentChargeTime >= chargeTime) {
            isCharging = false; // Ready to fire
        }
    }
}

cEnemyTank::cEnemyTank(cLoadModels* model) : enemyModel(*model)
{
    enemyPosition.x = enemyModel.pMeshTransform.x;
    enemyPosition.y = enemyModel.pMeshTransform.y;
    enemyPosition.z = enemyModel.pMeshTransform.z;
    currentState = TankState::Blocked;
    waitTime = 3.0f;
    timeBlocked = 0.0f;
    this->health = 100;

    vecProjectiles.clear();
    vecProjectileDirections.clear();
    vecLaserSpheres.clear();
}

cEnemyTank::~cEnemyTank()
{
}

cLoadModels* cEnemyTank::GetModel()
{
    return &enemyModel;
}

void cEnemyTank::SetModel(cLoadModels& model)
{
    this->enemyModel = model;
}

void cEnemyTank::MoveLeft(float deltaTime)
{

}

void cEnemyTank::MoveRight(float deltaTime)
{
}

void cEnemyTank::MoveForward(float deltaTime)
{
}

void cEnemyTank::MoveBackward(float deltaTime)
{
}

void cEnemyTank::Shoot(float deltaTime, cLoadModels* projectile)
{
    if (health <= 0) return;

    if (vecProjectiles.size() >= 1) {
        vecProjectiles.resize(1);
        return;
    }
    glm::vec3 bulletFront = forwardVector;
    cLoadModels* newProjectile = new cLoadModels(*projectile);
    newProjectile->pMeshTransform.x = enemyPosition.x;
    newProjectile->pMeshTransform.y = enemyPosition.y;
    newProjectile->pMeshTransform.z = enemyPosition.z;
    vecProjectileDirections.push_back(bulletFront);
    vecProjectiles.push_back(newProjectile);
}

void cEnemyTank::EnemyShoot(float deltaTime, cLoadModels* projectile, glm::vec3& currentForwardVector)
{
    //glm::vec3 bulletFront = currentForwardVector;
    //cLoadModels* newProjectile = new cLoadModels(*projectile);
    //newProjectile->pMeshTransform.x = enemyPosition.x;
    //newProjectile->pMeshTransform.y = enemyPosition.y;
    //newProjectile->pMeshTransform.z = enemyPosition.z;
    //vecProjectileDirections.push_back(bulletFront);
    //vecProjectiles.push_back(newProjectile);
}

void cEnemyTank::TakeHit(float deltaTime)
{
    if (health <= 0) return;

    if (health > 0) {
        health -= 10;
        std::cout << "Enemy Current health: " << health << std::endl;
    }
    if (health == 0) {
        std::cout << "Enemy Tank is dead" << std::endl;
    }
}

glm::vec3 cEnemyTank::GetPosition()
{
    return enemyPosition;
}

void cEnemyTank::SetPosition(glm::vec3 position)
{
    this->enemyPosition = position;
}

float cEnemyTank::GetSpeed()
{
    return speed;
}

void cEnemyTank::SetSpeed(float speed)
{
    this->speed = speed;
}

void cEnemyTank::PickRandomDirection()
{
    if (health <= 0) return;

    int direction = rand() % 4;
    switch (direction) {
    case 0: forwardVector = glm::vec3(1.0f, 0.0f, 0.0f); break;  // Move right
    case 1: forwardVector = glm::vec3(-1.0f, 0.0f, 0.0f); break; // Move left
    case 2: forwardVector = glm::vec3(0.0f, 1.0f, 0.0f); break;  // Move up
    case 3: forwardVector = glm::vec3(0.0f, -1.0f, 0.0f); break; // Move down
    }
}

void cEnemyTank::Update(float deltaTime)
{
    if (health <= 0) return;

    switch (currentState) {
    case TankState::Waiting:
        timeBlocked += deltaTime * speed;
        if (timeBlocked >= waitTime) {
            PickRandomDirection();
            if (forwardVector == glm::vec3(1.0f, 0.0f, 0.0f)) {
                enemyModel.pMeshTransform.zRotation = 90.0f;
            }
            if (forwardVector == glm::vec3(-1.0f, 0.0f, 0.0f)) {
                enemyModel.pMeshTransform.zRotation = -90.0f;
            }
            if (forwardVector == glm::vec3(0.0f, 1.0f, 0.0f)) {
                enemyModel.pMeshTransform.zRotation = 0.0f;
            }
            if (forwardVector == glm::vec3(0.0f, -1.0f, 0.0f)) {
                enemyModel.pMeshTransform.zRotation = 180.0f;
            }
            currentState = TankState::Moving;
            timeBlocked = 0.0f;
        }
        break;

    case TankState::Moving:
        nextX = (int)enemyPosition.x + forwardVector.x;
        nextY = (int)enemyPosition.y + forwardVector.y;
        if (CanMove(deltaTime) && !isCharging && health!= 0) {
            if (IsGridEmpty(nextX, nextY)) {
                enemyPosition.x = nextX;
                enemyPosition.y = nextY;
                //enemyModel.pMeshTransform.x = enemyPosition.x;
                //enemyModel.pMeshTransform.y = enemyPosition.y;
                enemyModel.pMeshTransform.x = nextX;
                enemyModel.pMeshTransform.y = nextY;
            }
            else {
                currentState = TankState::Blocked;
            }
        }
        break;

    case TankState::Blocked:
        PickRandomDirection();
        if (forwardVector == glm::vec3(1.0f, 0.0f, 0.0f)) {
            enemyModel.pMeshTransform.zRotation = 90.0f;
        }
        if (forwardVector == glm::vec3(-1.0f, 0.0f, 0.0f)) {
            enemyModel.pMeshTransform.zRotation = -90.0f;
        }
        if (forwardVector == glm::vec3(0.0f, 1.0f, 0.0f)) {
            enemyModel.pMeshTransform.zRotation = 180.0f;
        }
        if (forwardVector == glm::vec3(0.0f, -1.0f, 0.0f)) {
            enemyModel.pMeshTransform.zRotation = 0.0f;
        }
        if (IsGridEmpty((int)enemyPosition.x + forwardVector.x, (int)enemyPosition.y + forwardVector.y)) {
            currentState = TankState::Moving;
        }
        else {
            currentState = TankState::Waiting;
        }
        break;
    }
}

void cEnemyTank::UpdateProjectiles(float deltaTime) {
    if (health <= 0) return;

    for (int i = 0; i != vecProjectiles.size(); i++) {
        // Debug: print the position of the projectile
        //std::cout << "Checking projectile position: "
            //<< vecProjectiles[i]->pMeshTransform.x << ", "
            //<< vecProjectiles[i]->pMeshTransform.y << std::endl;

        if (!IsGridEmpty(vecProjectiles[i]->pMeshTransform.x, vecProjectiles[i]->pMeshTransform.y)) {
            // Print if projectile is going to be deleted
            //std::cout << "Deleting projectile at: "
                //<< vecProjectiles[i]->pMeshTransform.x << ", "
                //<< vecProjectiles[i]->pMeshTransform.y << std::endl;

            // Delete the projectile
            delete vecProjectiles[i];
            vecProjectiles.erase(vecProjectiles.begin() + i);
            vecProjectileDirections.erase(vecProjectileDirections.begin() + i);
        }
    }
}
bool cEnemyTank::CanMove(float deltaTime)
{
    float movementAccumulator = 0.0f;
    movementAccumulator += deltaTime * speed;
    return (movementAccumulator >= 0.013f);
}

void cEnemyTank::FireLaser(cLoadModels* laserSphere, float laserLength, float interval)
{
    if (health <= 0) return;

    //std::cout << "Firing enemy laser" << std::endl;
    if (!IsFullyCharged()) return;

    glm::vec3 laserFront = forwardVector; // Direction of the laser
    glm::vec3 startPosition = enemyPosition;

    vecLaserSpheres.clear(); // Clear any previous laser data

    for (float distance = 0.0f; distance <= laserLength; distance += interval) {
        cLoadModels* newLaserSphere = new cLoadModels(*laserSphere);

        glm::vec3 spherePosition = startPosition + laserFront * distance;

        newLaserSphere->pMeshTransform.x = spherePosition.x;
        newLaserSphere->pMeshTransform.y = spherePosition.y;
        newLaserSphere->pMeshTransform.z = spherePosition.z;

        vecLaserSpheres.push_back(newLaserSphere);
    }
    //std::cout << "Laser spheres populated: " << vecLaserSpheres.size() << std::endl;
    isCharging = false; // Reset charging state
}

void cEnemyTank::UpdateLaser(float deltaTime)
{
    if (health <= 0) return;

    if (vecLaserSpheres.empty()) return;

    laserTimer += deltaTime;
    if (laserTimer >= laserVisibleTime) {
        for (int i = 0; i != vecLaserSpheres.size(); i++) {
            delete vecLaserSpheres[i];
            vecLaserSpheres.erase(vecLaserSpheres.begin() + i);
        }
        vecLaserSpheres.clear();
        laserTimer = 0.0f; // Reset timer
        currentChargeTime = 0.0f;
    }
}