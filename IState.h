#pragma once
#include <string>
#include "IUpdate.h"
#include "IOnEnter.h"
#include "IOnExit.h"


class IState : public IUpdate, public IOnEnter, public IOnExit
{
public:
	virtual ~IState() {}
	virtual std::string GetStateID() const = 0;
};