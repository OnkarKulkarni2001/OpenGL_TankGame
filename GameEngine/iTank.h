#pragma once
#include "cLoadModels.h"

class iTank {
public:
    virtual cLoadModels* GetModel() = 0;
    virtual void SetModel(cLoadModels& model) = 0;
    virtual void MoveLeft(float deltaTime) = 0;
    virtual void MoveRight(float deltaTime) = 0;
    virtual void MoveForward(float deltaTime) = 0;
    virtual void MoveBackward(float deltaTime) = 0;
    virtual void Shoot(float deltaTime, cLoadModels* projectile) = 0;
    virtual void TakeHit(float deltaTime) = 0;
    virtual glm::vec3 GetPosition() = 0;
    virtual void SetPosition(glm::vec3 position) = 0;
    virtual float GetSpeed() = 0;
    virtual void SetSpeed(float speed) = 0;
};