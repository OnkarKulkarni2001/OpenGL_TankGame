#pragma once
#include "iTank.h"
#include <vector>

enum class TankState {
    Waiting,
    Moving,
    Blocked
};

class cEnemyTank : public iTank{
public:
    int health = 100;
    glm::vec3 forwardVector;
    bool isCharging = false;
    float chargeTime = 1.0f; // Laser charging time in seconds
    float currentChargeTime = 0.0f;
    float laserVisibleTime = 1.0f; // How long the laser remains visible
    float laserTimer = 0.0f;
    void StartCharging();
    bool IsFullyCharged() const;
    void UpdateCharge(float deltaTime);
    int killCount = 0;

    cEnemyTank(cLoadModels* model);
    ~cEnemyTank();
    glm::vec3 laserFront;
    std::vector<cLoadModels*> vecProjectiles;
    std::vector<glm::vec3> vecProjectileDirections;
    std::vector<cLoadModels*> vecLaserSpheres;
    void FireLaser(cLoadModels* laserSphere, float laserLength, float interval);
    cLoadModels* GetModel() override;
    void SetModel(cLoadModels& model) override;
    void MoveLeft(float deltaTime) override;
    void MoveRight(float deltaTime) override;
    void MoveForward(float deltaTime) override;
    void MoveBackward(float deltaTime) override;
    void Shoot(float deltaTime, cLoadModels* projectile) override;
    void EnemyShoot(float deltaTime, cLoadModels* projectile, glm::vec3& currentForwardVector);
    void TakeHit(float deltaTime) override;
    glm::vec3 GetPosition() override;
    void SetPosition(glm::vec3 position) override;
    float GetSpeed() override;
    void SetSpeed(float speed) override;
    void PickRandomDirection();
    void Update(float deltaTime);
    void UpdateProjectiles(float deltaTime);
    bool CanMove(float deltaTime);
    void UpdateLaser(float deltaTime);
private:
    int nextX, nextY;
    float speed = 1.4f;
    bool bIsMoving = false;
    glm::vec3 nextGridLocation;
    cLoadModels enemyModel;
    glm::vec3 enemyPosition;
    TankState currentState;
    float waitTime;
    float timeBlocked;
};