#include "ComponentRegistry.h"
#include "GameObject.h"

void ComponentRegistry::Register(const std::string& name, const std::string& category, ComponentFactoryFunc func)
{
	GetList().push_back({ name, category, func });
}

std::vector<ComponentMeta>& ComponentRegistry::GetList()
{
	static std::vector<ComponentMeta> s_list;
	return s_list;
}
