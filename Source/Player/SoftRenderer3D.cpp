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

float rad = 0.f;
float t = 0.f;
float t2 = 1.f;
bool straight = true;
// 게임 로직
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	InputManager input = _GameEngine.GetInputManager();

	Transform& player = _GameEngine.FindGameObjectWithName("Player")->GetTransform();
	//player.AddWorldRotation(Rotator(90.f * InDeltaSeconds, 0.f, 0.f));

	Transform& cam = _GameEngine.GetCamera()->GetTransform();
	if (input.GetXAxis())
		cam.AddWorldPosition(cam.GetForward() * 300.f * InDeltaSeconds);
	if (input.GetYAxis())
		cam.AddWorldPosition(-cam.GetForward() * 300.f * InDeltaSeconds);

	if (input.SpacePressed())
		_ClipFrustumCull = false;
	else
		_ClipFrustumCull = true;

	rad += Math::Deg2Rad(30.f * InDeltaSeconds);

	Rotator ri = Rotator(30, 20, 90.f);
	Quaternion q(ri);
	Rotator r = q.ToRotator();

	Quaternion q1(Rotator(0, 0, -45));
	Quaternion q2(Rotator(0, 0, 45));
	Quaternion q3(Rotator(-5, 0, -5));
	Quaternion q4(Rotator(5, 0, 5));
	if (straight)
	{
		t += InDeltaSeconds ;
		t2 -= InDeltaSeconds ;
		if (t >= 1.f)
			straight = false;
	}
	else
	{
		t -= InDeltaSeconds ;
		t2 += InDeltaSeconds ;
		if (t <= 0.f)
			straight = true;
	}

	Transform& la = _GameEngine.FindGameObjectWithName("LArm")->GetTransform();
	Transform& ra = _GameEngine.FindGameObjectWithName("RArm")->GetTransform();
	Transform& ll = _GameEngine.FindGameObjectWithName("LLeg")->GetTransform();
	Transform& rl = _GameEngine.FindGameObjectWithName("RLeg")->GetTransform();
	Transform& h = _GameEngine.FindGameObjectWithName("Head")->GetTransform();
	Transform& b = _GameEngine.FindGameObjectWithName("Body")->GetTransform();
	la.SetWorldRotation(Quaternion::Slerp(q1, q2, t));
	ra.SetWorldRotation(Quaternion::Slerp(q1, q2, t2));
	ll.SetWorldRotation(Quaternion::Slerp(q1, q2, t2));
	rl.SetWorldRotation(Quaternion::Slerp(q1, q2, t));
	h.SetWorldRotation(Quaternion::Slerp(q3, q4, t));
	b.SetWorldRotation(Quaternion::Slerp(q3, q4, t2));
}

void SoftRenderer::Render3D()
{
	memset(depthBuffer, 0.f, (_ScreenSize.X + 1) * (_ScreenSize.Y + 1));

	// 격자 그리기
	DrawGizmo3D();

	Camera* camera = _GameEngine.GetCamera();

	Matrix4x4 viewMat = _GameEngine.GetCamera()->GetViewMatrix();
	Matrix4x4 projMat = camera->GetPerspectiveMatrix(_ScreenSize.X, _ScreenSize.Y);
	Matrix4x4 viewProjMat = projMat * viewMat;

	unsigned int viewFrustumCulledCount = 0;
	unsigned int viewFrustumDrawedCount = 0;
	unsigned int clipFrustumCulledCount = 0;
	unsigned int clipFrustumDrawedCount = 0;
	for (auto& go : _GameEngine.GetGameObjects())
	{
		const Mesh* mesh = go->GetMesh();
		if (!mesh) continue;

#pragma region View space frustum culling
		Matrix4x4 mat = viewMat * go->GetTransform().GetWorldModelingMatrix();
		Sphere goSphere = mesh->GetSphereBound();
		goSphere.Center = mat * goSphere.Center;
		goSphere.Radius = goSphere.Radius * go->GetTransform().GetWorldScale().Max();
		if (!_ClipFrustumCull)
		{
			if (!camera->ViewSpaceFrustumCulling(goSphere))
			{
				viewFrustumCulledCount++;
				continue;
			}
			viewFrustumDrawedCount++;
		}
#pragma endregion
#pragma region Clip space frustum culling
		else
		{
			if (!camera->ClipSpaceFrustumCulling(Vector2(_ScreenSize.X, _ScreenSize.Y), goSphere))
			{
				clipFrustumCulledCount++;
				continue;
			}
			clipFrustumDrawedCount++;
		}
#pragma endregion

		size_t vertexCount = mesh->_Vertices.size();
		size_t indexCount = mesh->_Indices.size();
		size_t triangleCount = indexCount / 3;

		// 정점 배열과 인덱스 배열 생성
		Vector4* vertices = new Vector4[vertexCount];
		for (int i = 0; i < vertexCount; ++i)
		{
			vertices[i] = Vector4(mesh->_Vertices[i]);
		}
		//memcpy(vertices, &mesh->_Vertices[0], sizeof(Vector4) * vertexCount);
		int* indices = new int[indexCount];
		memcpy(indices, &mesh->_Indices[0], sizeof(int) * indexCount);

		//_RSI->PushStatisticText(playerTransform.GetPosition().ToString());

		// 변환 행렬의 설계
		Matrix4x4 finalMat = viewProjMat * go->GetTransform().GetWorldModelingMatrix();

#pragma region Rodrigues rotation
		//Vector4 n(1.f, 3.f, 1.f, 0.f);
		//n = n.Normalize();
		//float cs = cosf(rad);
		//for (int i = 0; i < vertexCount; ++i)
		//{
		//	vertices[i] = vertices[i] * cs + n * (1 - cs) * vertices[i].Dot(n) + Vector4(Vector3(n).Cross(vertices[i]) * sinf(rad), 0);
		//	vertices[i].W = 1.f;
		//}
#pragma endregion

		// 정점에 행렬을 적용
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = finalMat * vertices[vi];
			float invW = 1.f / vertices[vi].W;
			vertices[vi].X *= invW;
			vertices[vi].Y *= invW;

			float depth = 1.f / (1000.f - 0.003f);
			vertices[vi].Z *= invW;//depth;
		}

		// 변환된 정점을 잇는 선 그리기
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi = ti * 3;

#pragma region Backface Culling
			Vector4 edge1 = vertices[indices[bi + 1]] - vertices[indices[bi]];
			Vector4 edge2 = vertices[indices[bi + 2]] - vertices[indices[bi]];
			Vector3 cross = edge1.ToVector3().Cross(edge2.ToVector3()).Normalize();
			Vector4 camForward = Vector4(viewProjMat * camera->GetTransform().GetForward());
			camForward *= 1.f / camForward.W;
			if (cross.Dot(camForward) < 0.f)
			{
				continue;
			}
#pragma endregion

			Vector4 tp[3] = { vertices[indices[bi]], vertices[indices[bi + 1]], vertices[indices[bi + 2]]};
			for (int i = 0; i < 3; ++i)
			{
				tp[i].X *= (_ScreenSize.X * 0.5f);
				tp[i].Y *= (_ScreenSize.Y * 0.5f);
			}
			_RSI->DrawLine(tp[0], tp[1], _CurrentColor);
			_RSI->DrawLine(tp[0], tp[2], _CurrentColor);
			_RSI->DrawLine(tp[1], tp[2], _CurrentColor);
			//_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 1]], _CurrentColor);
			//_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 2]], _CurrentColor);
			//_RSI->DrawLine(vertices[indices[bi + 1]], vertices[indices[bi + 2]], _CurrentColor);
		}
		delete[] vertices;
		delete[] indices;
	}

	Transform& player = _GameEngine.FindGameObjectWithName("Player")->GetTransform();
	_RSI->PushStatisticText("Distance from O: " + std::to_string((player.GetWorldPosition() - camera->GetTransform().GetWorldPosition()).Size()));
	_RSI->PushStatisticText("Sphere Radius: " + std::to_string(_GameEngine.FindGameObjectWithName("Player")->GetMesh()->GetSphereBound().Radius * player.GetWorldScale().Max()));
	if (!_ClipFrustumCull)
	{
		_RSI->PushStatisticText("Total: " + std::to_string(viewFrustumCulledCount + viewFrustumDrawedCount));
		_RSI->PushStatisticText("ViewSpaceFrustumCulledCount: " + std::to_string(viewFrustumCulledCount));
		_RSI->PushStatisticText("ViewSpaceFrustumDrawedCount: " + std::to_string(viewFrustumDrawedCount));
	}
	else
	{
		_RSI->PushStatisticText("Total: " + std::to_string(clipFrustumCulledCount + clipFrustumDrawedCount));
		_RSI->PushStatisticText("ClipSpaceFrustumCulledCount: " + std::to_string(clipFrustumCulledCount));
		_RSI->PushStatisticText("ClipSpaceFrustumDrawedCount: " + std::to_string(clipFrustumDrawedCount));
	}
}