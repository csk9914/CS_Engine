#include "ResourceManager.h"

void ResourceManager::InitDefaultResources()
{

#define DEFINE_TYPE(id, str) m_primitiveNames[PrimitiveType::id] = str;
    PRIMITIVE_LIST
#undef DEFINE_TYPE

    // 평면
    MeshData planeData = GeometryGenerator::CreatePlaneData(10.0f);
    Mesh* planeMesh = new Mesh(planeData);
    AddMesh(m_primitiveNames[PrimitiveType::Plane], planeMesh);

    // 큐브
    MeshData cubeData = GeometryGenerator::CreateCubeData(1.0f);
    Mesh* cubeMesh = new Mesh(cubeData);
    AddMesh(m_primitiveNames[PrimitiveType::Cube], cubeMesh);

    // 구
    MeshData sphereData = GeometryGenerator::CreateSphereData(0.5f, 32, 32);
    Mesh* sphereMesh = new Mesh(sphereData);
    AddMesh(m_primitiveNames[PrimitiveType::Sphere], sphereMesh);



}