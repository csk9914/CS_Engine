#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Mesh.h"
#include "GeometryGenerator.h"
#include "PrimitiveType.h"


class ResourceManager
{
private:
    ResourceManager() = default;

public:
    static ResourceManager* Instance()
    {
        static ResourceManager instance;
        return &instance;
    }

    void Init();

private:
    // 기본 도형들을 미리 구워두는 함수
    void InitDefaultResources();




public:
    // 메쉬 추가 ( 미리 등록해서 사용)
    void AddMesh(const std::string& name, Mesh* mesh)
    {
        if (m_meshDict.find(name) == m_meshDict.end())
        {
            m_meshDict[name] = std::unique_ptr<Mesh>(mesh);
        }
        else
            delete mesh;
    }

    // 메쉬 가져오기 (공유 자원)
    Mesh* GetMesh(const std::string& name) {
        if (m_meshDict.find(name) != m_meshDict.end())
            return m_meshDict[name].get();
        return nullptr;
    }

    Mesh* GetMesh(PrimitiveType type)
    {
        return GetMesh(m_primitiveNames[type]);
    }

    std::string GetPrimitiveNames(PrimitiveType key)
    {
        return m_primitiveNames[key];
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_meshDict;
    std::unordered_map<PrimitiveType, std::string> m_primitiveNames;
};