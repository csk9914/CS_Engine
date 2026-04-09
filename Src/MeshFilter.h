#pragma once
#include "Mesh.h"
#include "Component.h"

class MeshFilter : public Component
{
public:
    MeshFilter(const std::string name = "MeshFilter") : Component(name) {}

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh() { return m_mesh; }

    virtual void OnEditorGUI();
private:
    Mesh* m_mesh = nullptr; // "큐브 메쉬"의 주소값을 저장
};