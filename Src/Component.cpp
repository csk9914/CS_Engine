#include "Component.h"
#include "GameObject.h"

inline void Component::SetEnabled(bool enabled)
{
	if (m_enabled == enabled) return;

	m_enabled = enabled;

	if (m_gameObject && m_gameObject->GetActive())
	{
		if (m_enabled)
		{
			SetActive(true);
			OnEnable();
		}
		else
		{
			SetActive(false);
			OnDisable();
		}
	}
}
