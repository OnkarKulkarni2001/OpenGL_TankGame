#pragma once
#include "iTank.h"
#include <vector>

class cPlayerTank : public iTank {
public:
    int health = 100;
    bool isCharging = false;
    float chargeTime = 1.0f; // Laser charging time in seconds
    float currentChargeTime = 0.0f;
    float laserVisibleTime = 1.0f; // How long the laser remains visible
    float laserTimer = 0.0f;
    void StartCharging();
    bool IsFullyCharged() const;
    void UpdateCharge(float deltaTime);

    glm::vec3 laserFront;
    std::vector<cLoadModels*> vecProjectiles;
    std::vector<glm::vec3> vecProjectileDirections;
    std::vector<cLoadModels*> vecLaserSpheres;
    cPlayerTank(cLoadModels* model);
    ~cPlayerTank();
    cLoadModels* GetModel() override;
    void SetModel(cLoadModels& model) override;
    void MoveLeft(float deltaTime) override;
    void MoveRight(float deltaTime) override;
    void MoveForward(float deltaTime) override;
    void MoveBackward(float deltaTime) override;
    void Shoot(float deltaTime, cLoadModels* projectile) override;
    void TakeHit(float deltaTime) override;
    glm::vec3 GetPosition() override;
    void SetPosition(glm::vec3 position) override;
    float GetSpeed() override;
    void SetSpeed(float speed) override;
    bool CanMove(float deltaTime);
    void Update(float deltaTime);
    void FireLaser(cLoadModels* laserSphere, float laserLength, float interval);
    void UpdateLaser(float deltaTime);
private:
    float speed = 1.2f;
    glm::vec3 forwardVector = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 nextGridLocation;
    cLoadModels playerModel;
    glm::vec3 playerPosition;
};