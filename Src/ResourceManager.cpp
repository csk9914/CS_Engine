#include "ResourceManager.h"

void ResourceManager::Init()
{
    InitDefaultResources();
}

void ResourceManager::InitDefaultResources()
{

#define DEFINE_TYPE(id, str) m_primitiveNames[PrimitiveType::id] = str;
    PRIMITIVE_LIST
#undef DEFINE_TYPE

        // 평면
    std::string planeName = m_primitiveNames[PrimitiveType::Plane];
    MeshData planeData = GeometryGenerator::CreatePlaneData(10.0f);
    AddMesh(planeName, new Mesh(planeData, planeName));

    // 큐브
    std::string cubeName = m_primitiveNames[PrimitiveType::Cube];
    MeshData cubeData = GeometryGenerator::CreateCubeData(1.0f);
    AddMesh(cubeName, new Mesh(cubeData, cubeName));

    // 구
    std::string sphereName = m_primitiveNames[PrimitiveType::Sphere];
    MeshData sphereData = GeometryGenerator::CreateSphereData(0.5f, 32, 32);
    AddMesh(sphereName, new Mesh(sphereData, sphereName));

    // 캡슐
    std::string capsuleName = m_primitiveNames[PrimitiveType::Capsule];
    MeshData capsuleData = GeometryGenerator::CreateCapsuleData(0.5f, 1.0f, 20, 20);
    AddMesh(capsuleName, new Mesh(capsuleData, capsuleName));

}