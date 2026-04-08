#pragma once
#include"IUpdate.h"
#include <memory>
#include "Scene.h"

template<typename T>
class StateMachine : public IUpdate
{
public:
	StateMachine() : m_curState(nullptr) {}
	virtual ~StateMachine() {}

	virtual void ChangeState(std::unique_ptr<T> nextState)
	{
		// 기존 상태 종료
		if (m_curState)
		{
			m_curState->OnExit();
		}

		// 상태 교체 (unique_ptr의 특성상 여기서 이전 객체는 delete됨)
		m_curState = std::move(nextState);

		// 새 상태 진입
		if (m_curState)
		{
			m_curState->OnEnter();
		}
	}

	virtual void Update(float deltaTime) override
	{
		if (m_curState)
			m_curState->Update(deltaTime);
	}

public:
	 T* GetCurState() const { return m_curState.get(); }

private:
	std::unique_ptr<T> m_curState;
};

