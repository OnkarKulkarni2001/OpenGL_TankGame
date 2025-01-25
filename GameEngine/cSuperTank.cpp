#include "cSuperTank.h"
bool IsGridEmpty(int playerX, int playerY);     // Forward Declaration
bool IsDeadEnd(int x, int y, const glm::vec3& direction);

cSuperTank::cSuperTank(cLoadModels* model) : superTankModel(*model)
{
    health = 150;
    turnsRemainingForMissile = 2;
}

cSuperTank::~cSuperTank()
{
}

void cSuperTank::Shoot(float deltaTime, cLoadModels* projectileModel)
{
    FireTrackingMissile(deltaTime, projectileModel);
}

void cSuperTank::TakeHit(float deltaTime)
{
}

glm::vec3 cSuperTank::GetPosition()
{
    return glm::vec3();
}

void cSuperTank::SetPosition(glm::vec3 position)
{
}

float cSuperTank::GetSpeed()
{
    return 0.0f;
}

void cSuperTank::SetSpeed(float speed)
{
}

void cSuperTank::FireTrackingMissile(float deltaTime, cLoadModels* projectileModel)
{
    sTrackingMissile missile;
    missile.position = GetPosition();
    missile.direction = forwardVector;
    missile.turnsRemaining = 2; // Can turn twice
    vecTrackingMissiles.push_back(missile);
}

void cSuperTank::UpdateMissiles(float deltaTime)
{
    for (int i = 0; i < vecTrackingMissiles.size(); i++) {
        vecTrackingMissiles[i].Move(deltaTime);
        if (vecTrackingMissiles[i].hasExploded) {
            vecTrackingMissiles.erase(vecTrackingMissiles.begin() + i);
            i--;
        }
    }
}

cLoadModels* cSuperTank::GetModel()
{
    return nullptr;
}

void cSuperTank::SetModel(cLoadModels& model)
{
}

void cSuperTank::MoveLeft(float deltaTime)
{
}

void cSuperTank::MoveRight(float deltaTime)
{
}

void cSuperTank::MoveForward(float deltaTime)
{
}

void cSuperTank::MoveBackward(float deltaTime)
{
}

void cSuperTank::sTrackingMissile::Move(float deltaTime)
{
    if (turnsRemaining <= 0) {
        Explode();
        return;
    }

    // Wall collision check
    int nextX = (int)position.x + direction.x;
    int nextY = (int)position.y + direction.y;
    if (!IsGridEmpty(nextX, nextY)) {
        direction = ChooseNewDirection();
        if (direction == glm::vec3(0)) {
            Explode();
            return;
        }
        turnsRemaining--;
    }

    position += direction * deltaTime * speed;
}

void cSuperTank::sTrackingMissile::Explode()
{
    hasExploded = true;
    std::cout << "Tracking missile exploded!" << std::endl;
}

glm::vec3 cSuperTank::sTrackingMissile::ChooseNewDirection()
{
    int newDirection = rand() % 4;
    switch (newDirection) {
    case 0: direction = glm::vec3(1, 0, 0); break;  // Move right
    case 1: direction = glm::vec3(-1, 0, 0); break; // Move left
    case 2: direction = glm::vec3(0, 1, 0); break;  // Move up
    case 3: direction = glm::vec3(0, -1, 0); break; // Move down
    }
    return direction;
}