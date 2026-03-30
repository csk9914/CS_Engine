#pragma once


class IUpdate
{
public:
	virtual ~IUpdate() {}
	virtual void Update(float deltaTime) = 0;
};