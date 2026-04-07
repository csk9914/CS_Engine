#pragma once
#include <map>
#include <string>
#include <memory>

// 자원들의 부모 클래스 (나중에 Mesh, Texture가 이를 상속받음)
class Resource
{
public:
    virtual ~Resource() {}
    std::string name;
};

class ResourceManager
{
public:
    static ResourceManager* Instance()
    {
        static ResourceManager instance;
        return &instance;
    }

    // 자원 가져오기 (없으면 로드하거나 nullptr 반환)
    template <typename T>
    std::shared_ptr<T> Get(const std::string& name) 
    {
        if (m_resources.find(name) != m_resources.end())
            return std::static_pointer_cast<T>(m_resources[name]);
        return nullptr;
    }

    // 자원 수동 등록 (직접 생성한 자원 등)
    //void Add(const std::string& name, std::shared_ptr<Resource> res);

private:
    ResourceManager() {}
    // 모든 자원을 통합 관리하는 저장소
    std::map<std::string, std::shared_ptr<Resource>> m_resources;
};