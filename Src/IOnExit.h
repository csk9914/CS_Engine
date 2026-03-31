#pragma once

class IOnExit
{
public:
	virtual ~IOnExit() {}
	virtual void OnExit() = 0;
};