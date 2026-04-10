#pragma once
#include "Component.h"
#include "Transform.h"

#include <vector>
#include <memory>
#include <string>

#include "Object.h"
#include "PrimitiveType.h"

class Scene;

class GameObject : public Object
{
public:
    GameObject(Scene* scene, const std::string& name = "GameObject");
    virtual ~GameObject() {}

    virtual void OnDestroy();

    void Destroy() { m_isDestroyed = true; }
    bool IsDestroy() const { return m_isDestroyed; }

public:
    void Awake();
    void Start();
    void Update(float deltaTime);

    template<typename T>
    T* AddComponent()
    {
        auto component = std::make_unique<T>();
        T* ptr = component.get();
        ptr->m_gameObject = this;

        // 기존: m_isAwake && IsPlaying() 둘 다 필요
        // 수정: m_isAwake 만 체크 — 에디터 모드에서 추가해도 OnEnable 호출됨
        // (OnEnable에서 RenderManager/Scene 등록이 일어나므로 반드시 필요)
        if (m_isAwake)
        {
            ptr->Awake();

            if (this->GetActive() && ptr->m_enabled)
            {
                ptr->SetActive(true);
                ptr->OnEnable();
                ptr->Start();
            }
        }
 

        m_components.push_back(std::move(component));
        return ptr;
    }

    template<typename T>
    T* GetComponent()
    {
        for (auto& comp : m_components)
        {
            if (T* t = dynamic_cast<T*>(comp.get()))
                return t;
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Component>>& GetComponents() const { return m_components; }
    std::vector<std::unique_ptr<Component>>& GetComponents() { return m_components; }

    template<typename T>
    std::vector<T*> GetComponents()
    {
        std::vector<T*> results;
        for (auto& comp : m_components)
            if (T* t = dynamic_cast<T*>(comp.get()))
                results.push_back(t);
        return results;
    }

    void RemoveComponent(Component* component);

    template<typename T>
    void RemoveComponent()
    {
        for (auto it = m_components.begin(); it != m_components.end(); ++it)
        {
            if (dynamic_cast<T*>(it->get()))
            {
                (*it)->OnDestroy();
                m_components.erase(it);
                return;
            }
        }
    }

    static GameObject* CreateGameObject(const std::string& name = "GameObject");
    static GameObject* CreatePrimitive(PrimitiveType type);

public:
    virtual void SetActive(bool active) override;

    Transform* GetTransform() const { return m_transform; }
    Transform* GetTransform() { return m_transform; }
    Scene* GetScene()     const { return m_scene; }

    bool IsPlaying() const;

private:
    Scene* m_scene = nullptr;
    Transform* m_transform = nullptr;
    std::vector<std::unique_ptr<Component>> m_components;

    bool m_isDestroyed = false;
    bool m_isAwake = false;
};

inline GameObject* CreateGameObject(const std::string& name = "GameObject")
{
    return GameObject::CreateGameObject(name);
}
inline GameObject* CreatePrimitive(PrimitiveType type)
{
    return GameObject::CreatePrimitive(type);
}