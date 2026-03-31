#include "Component.h"
#include "GameObject.h"

inline void Component::SetEnabled(bool enabled)
{
	if (m_enabled == enabled) return;

	m_enabled = enabled;

	if (m_owner && m_owner->GetActive())
	{
		if (m_enabled)
		{
			m_active = true;
			OnEnable();
		}
		else
		{
			m_active = false;
			OnDisable();
		}
	}
}
