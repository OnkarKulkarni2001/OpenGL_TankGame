#pragma once
#include "cEnemyTank.h"

class cSuperTank : public iTank{
public:
	struct sTrackingMissile {
		glm::vec3 position;
		glm::vec3 direction;
		float speed;
		int turnsRemaining;
		bool hasExploded = false;
		void Move(float deltaTime);
		void Explode();
		glm::vec3 ChooseNewDirection();
	};
	std::vector<sTrackingMissile> vecTrackingMissiles;

	int health = 150;
	int turnsRemainingForMissile = 2;
	cSuperTank(cLoadModels* model);
	~cSuperTank();
	void Shoot(float deltaTime, cLoadModels* projectileModel) override;
	void FireTrackingMissile(float deltaTime, cLoadModels* projectileModel);
	void UpdateMissiles(float deltaTime);
	cLoadModels* GetModel() override;
	void SetModel(cLoadModels& model) override;
	void MoveLeft(float deltaTime) override;
	void MoveRight(float deltaTime) override;
	void MoveForward(float deltaTime) override;
	void MoveBackward(float deltaTime) override;
	void TakeHit(float deltaTime) override;
	glm::vec3 GetPosition() override;
	void SetPosition(glm::vec3 position) override;
	float GetSpeed() override;
	void SetSpeed(float speed) override;
private:
	glm::vec3 superTankPosition;
	glm::vec3 forwardVector;
	cLoadModels superTankModel;
	
};