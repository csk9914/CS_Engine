#pragma once
#include "IUpdate.h"
#include "IRender.h"
#include "IStart.h"
#include "IEnd.h"
#include "AppConfig.h"

class GameScene;

class IGame : public IUpdate, public IStart, public IEnd 
{
public:
	virtual ~IGame() = default;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual const AppConfig& GetAppConfig() const = 0;

	// EditorUI가 씬 오브젝트 목록에 접근하기 위해 필요
	virtual GameScene* GetCurrentScene() = 0;
};