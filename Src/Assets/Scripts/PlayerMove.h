#pragma once
#include "MonoBehaviour.h"

class PlayerMove : public MonoBehaviour
{
public:
	PlayerMove(std::string name = "PlayerMove") : MonoBehaviour(name) {}
	virtual void Start()              override;
	virtual void Update(float dt)     override;

private:
	Transform* m_transform;
	Rigidbody* m_rigid;

	float m_speed;
	float m_rotSpeed;
};
