#include "Precompiled.h"
#include "SoftRenderer.h"
#include <chrono>

void SoftRenderer::DrawGizmo3D()
{
	Matrix4x4 viewMatrix = _GameEngine.GetCamera()->GetViewMatrix();
	Matrix4x4 perspectiveMatrix = _GameEngine.GetCamera()->GetPerspectiveMatrix(_ScreenSize.X, _ScreenSize.Y);

	const static int gizmosVertexCount = 6;
	Vector4 gizmoPoints[gizmosVertexCount] = {
		Vector4(Vector3::Zero),
		Vector4(Vector3::UnitX * 50.f),
		Vector4(Vector3::Zero),
		Vector4(Vector3::UnitY * 50.f),
		Vector4(Vector3::Zero),
		Vector4(Vector3::UnitZ * 50.f),
	};

	Matrix4x4 finalMatrix = perspectiveMatrix * viewMatrix;
	for (int pi = 0; pi < gizmosVertexCount; ++pi)
	{
		gizmoPoints[pi] = finalMatrix * gizmoPoints[pi];
		float invW = 1.f / gizmoPoints[pi].W;
		gizmoPoints[pi].X *= invW;
		gizmoPoints[pi].Y *= invW;
		gizmoPoints[pi].Z *= invW;

		// 화면 크기로 늘리기
		gizmoPoints[pi].X *= (_ScreenSize.X * 0.5f);
		gizmoPoints[pi].Y *= (_ScreenSize.Y * 0.5f);
	}

	_RSI->DrawLine(gizmoPoints[0].ToVector2(), gizmoPoints[1].ToVector2(), LinearColor::Red);
	_RSI->DrawLine(gizmoPoints[2].ToVector2(), gizmoPoints[3].ToVector2(), LinearColor::Green);
	_RSI->DrawLine(gizmoPoints[4].ToVector2(), gizmoPoints[5].ToVector2(), LinearColor::Blue);
}

// 게임 로직
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	InputManager input = _GameEngine.GetInputManager();

	Transform& player = _GameEngine.FindGameObjectWithName("Player")->GetTransform();
	player.AddWorldRotation(Rotator(0.f, 90.f * InDeltaSeconds, 0.f));
}

void SoftRenderer::Render3D()
{
	// 격자 그리기
	DrawGizmo3D();

	Camera* camera = _GameEngine.GetCamera();

	Matrix4x4 viewMat = _GameEngine.GetCamera()->GetViewMatrix();
	
	for (auto& go : _GameEngine.GetGameObjects())
	{
		const Mesh* mesh = go->GetMesh();
		if (!mesh) continue;

		size_t vertexCount = mesh->_Vertices.size();
		size_t indexCount = mesh->_Indices.size();
		size_t triangleCount = indexCount / 3;

		// 정점 배열과 인덱스 배열 생성
		Vector3* vertices = new Vector3[vertexCount];
		memcpy(vertices, &mesh->_Vertices[0], sizeof(Vector3) * vertexCount);
		int* indices = new int[indexCount];
		memcpy(indices, &mesh->_Indices[0], sizeof(int) * indexCount);

		//_RSI->PushStatisticText(playerTransform.GetPosition().ToString());

		// 변환 행렬의 설계
		Matrix4x4 finalMat = viewMat * go->GetTransform().GetWorldModelingMatrix();

		// 정점에 행렬을 적용
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = finalMat * vertices[vi];
		}

		// 변환된 정점을 잇는 선 그리기
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi = ti * 3;
			_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 1]], _CurrentColor);
			_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 2]], _CurrentColor);
			_RSI->DrawLine(vertices[indices[bi + 1]], vertices[indices[bi + 2]], _CurrentColor);
		}
		delete[] vertices;
		delete[] indices;
	}
}