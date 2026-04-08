#pragma once
#include "Mesh.h"
#include "Component.h"

class MeshFilter : public Component
{
public:
    void SetMesh(Mesh* mesh) { m_mesh = mesh; }
    Mesh* GetMesh() { return m_mesh; }

private:
    Mesh* m_mesh = nullptr; // "큐브 메쉬"의 주소값을 저장
};