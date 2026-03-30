#pragma once
#include "IUpdate.h"
#include "IRender.h"
#include "IStart.h"
#include "IEnd.h"
#include"LoaderParams.h"

class GameObject : public IUpdate, public IRender, public IStart, public IEnd
{
public:
	virtual ~GameObject() {}

	// 객체가 파괴될 때 자원 정리
	virtual void OnDestroy() = 0;

	// 객체를 제거 목록에 추가
	void Destroy() { m_isDestroyed = true; }
	bool IsDestroy() const { return m_isDestroyed; }

	void SetActive(bool active) { m_active = active; }
	bool GetActive() const { return m_active; }


protected:
	GameObject(LoaderParams* params) : m_active(true), m_isDestroyed(false)
	{
		m_x = params->GetX();
		m_y = params->GetY();
		m_width = params->GetWidth();
		m_height = params->GetHeight();
	}
	
protected:
	float m_x, m_y;
	float m_width, m_height;

private:
	bool m_active;

	// Destory 체크
	bool m_isDestroyed;

};