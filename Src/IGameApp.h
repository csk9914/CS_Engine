#pragma once
#include "IUpdate.h"
#include "IRender.h"
#include "IStart.h"
#include "IEnd.h"
#include "AppConfig.h"

class Scene;

class IGameApp : public IUpdate, public IStart, public IEnd 
{
public:
	virtual ~IGameApp() = default;
	virtual void Awake() = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual const AppConfig& GetAppConfig() const = 0;

	// EditorUI가 씬 오브젝트 목록에 접근하기 위해 필요
	virtual Scene* GetCurrentScene() = 0;
};