#include "Gizmo.h"
#include "GameEngine.h"
#include "RenderManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "InputHandler.h"

using namespace DirectX;

Gizmo::Gizmo() {}

Gizmo::~Gizmo() {}

bool Gizmo::Init()
{
	// ImGuizmo는 별도의 DX 리소스 초기화가 필요 없지만, 
	// 엔진 구조상 초기화 여부를 체크하기 위해 남겨둡니다.
	m_initialized = true;
	return true;
}

void Gizmo::BeginFrame()
{
	// 프레임 선언
	ImGuizmo::BeginFrame();
}

void Gizmo::Update()
{
	if (!m_target) return;

	// 단축키로 기즈모 모든 변경 (W: 이동, E 회전, R: 크기)
	InputHandler* input = InputHandler::Instance();
	if (input->IsKeyDown('W'))
		m_currentOp = ImGuizmo::TRANSLATE;

	if (input->IsKeyDown('E'))
		m_currentOp = ImGuizmo::ROTATE;

	if (input->IsKeyDown('R'))
		m_currentOp = ImGuizmo::SCALE;

	// 로컬 / 월드 전환 (L 키)
	if (input->IsKeyOneDown('L'))
	{
		if (m_currentMode == ImGuizmo::WORLD)
			m_currentMode = ImGuizmo::LOCAL;
		else
			m_currentMode = ImGuizmo::WORLD;
	}
}

void Gizmo::Render()
{
	if (!m_target || !m_initialized) return;


	// 카메라 데이터 가져오기
	Camera* cam = GameEngine::Instance()->GetEditorCamera();
	if (!cam) return;

	XMMATRIX view = cam->GetViewMatrix();
	XMMATRIX proj = cam->GetProjectionMatrix();

	// 타겟의 월드 좌표 가져오기
	XMMATRIX world = m_target->GetTransform()->GetWorldMatrix();

	 //기즈모 조작 및 렌더링
	ImGuizmo::Manipulate(
		(float*)&view,
		(float*)&proj,
		m_currentOp,
		m_currentMode,
		(float*)&world
	);

	// 6. 조작 결과가 있다면 다시 Transform에 반영
	if (ImGuizmo::IsUsing())
	{
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		// ImGuizmo 내부 함수를 사용하여 행렬 분해 (오일러 각도로 바로 추출)
		ImGuizmo::DecomposeMatrixToComponents((float*)&world, matrixTranslation, matrixRotation, matrixScale);

		// [Position] 반영
		m_target->GetTransform()->SetPosition(Vector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));

		// [Scale] 반영
		m_target->GetTransform()->SetScale(Vector3(matrixScale[0], matrixScale[1], matrixScale[2]));

		// [Rotation] 반영
		// ImGuizmo는 기본적으로 Degree 값을 반환하므로 별도의 변환 없이 바로 넣습니다.
		m_target->GetTransform()->SetRotation(Vector3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));
	}



}
