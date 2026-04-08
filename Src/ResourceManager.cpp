#include "ResourceManager.h"

void ResourceManager::InitDefaultResources()
{

#define DEFINE_TYPE(id, str) m_primitiveNames[PrimitiveType::id] = str;
    PRIMITIVE_LIST
#undef DEFINE_TYPE

    // 평면
    MeshData planeData = GeometryGenerator::CreatePlaneData(10.0f);
    AddMesh(m_primitiveNames[PrimitiveType::Plane], new Mesh(planeData));

    // 큐브
    MeshData cubeData = GeometryGenerator::CreateCubeData(1.0f);
    AddMesh(m_primitiveNames[PrimitiveType::Cube], new Mesh(cubeData));

    // 구
    MeshData sphereData = GeometryGenerator::CreateSphereData(0.5f, 32, 32);
    AddMesh(m_primitiveNames[PrimitiveType::Sphere], new Mesh(sphereData));

    // 캡슐
    MeshData capsuleData = GeometryGenerator::CreateCapsuleData(0.5f, 1.0f, 20, 20);
    AddMesh(m_primitiveNames[PrimitiveType::Capsule], new Mesh(capsuleData));

}