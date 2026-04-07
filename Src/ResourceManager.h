#pragma once
#include <map>
#include <string>
#include <memory>
#include "Mesh.h"
#include "GeometryFactory.h"

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

    // 메쉬 추가 ( 미리 등록해서 사용)
    void AddMesh(const std::string& name, Mesh* mesh) {
        if (m_meshDict.find(name) == m_meshDict.end()) {
            m_meshDict[name] = std::unique_ptr<Mesh>(mesh);
        }
    }

    // 메쉬 가져오기 (공유 자원)
    Mesh* GetMesh(const std::string& name) {
        if (m_meshDict.find(name) != m_meshDict.end())
            return m_meshDict[name].get();
        return nullptr;
    }

    // [중요] 기본 도형들을 미리 구워두는 함수
    void InitDefaultResources() {
        // 큐브 구워서 "DefaultCube"라는 이름으로 창고에 넣기
        MeshData cubeData = GeometryFactory::CreateCube(1.0f);
        Mesh* cubeMesh = new Mesh();
        cubeMesh->Create(cubeData);
        AddMesh("CSCube", cubeMesh);

        // 평면
        MeshData planeData = GeometryFactory::CreatePlane(10.0f);
        Mesh* planeMesh = new Mesh();
        planeMesh->Create(planeData);
        AddMesh("CSPlane", planeMesh);

        
        // 나중에 평면, 구 등도 여기서 미리 구워둠


    }

private:
    std::map<std::string, std::shared_ptr<Mesh>> m_meshDict;
};