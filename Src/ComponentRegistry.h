#pragma once
#include <string>
#include <functional>
#include <vector>

class GameObject;

// 1. 함수 포인터나 std::function을 사용하여 생성자 보관
using ComponentFactoryFunc = std::function<void(GameObject*)>;

struct ComponentMeta
{
	std::string name;
	std::string category;

	ComponentFactoryFunc createFunc;
};

class ComponentRegistry
{
public:
	static void Register(const std::string& name, const std::string& category, ComponentFactoryFunc func);

	static std::vector<ComponentMeta>& GetList();

};

#define REGISTER_COMPONENT(Type, Category) \
struct Type##Register { \
    Type##Register() { \
        ComponentRegistry::Register(#Type, Category, [](GameObject* go) { \
            if (!go->GetComponent<Type>()) go->AddComponent<Type>(); \
        }); \
    } \
}; \
static Type##Register global_##Type##Register;