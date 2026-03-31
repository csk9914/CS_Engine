#pragma once
#include "IUpdate.h"
#include "IRender.h"
#include "IStart.h"
#include "IEnd.h"
#include "AppConfig.h"

class IGame : public IUpdate, public IStart, public IEnd 
{
public:
	virtual ~IGame() = default;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual AppConfig GetAppConfig() const = 0;
};