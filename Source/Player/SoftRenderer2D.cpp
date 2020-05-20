
#include "Precompiled.h"
#include "SoftRenderer.h"

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = _GameEngine.FindGameObjectWithName("Camera")->GetTransform2D().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		_RSI->DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		_RSI->DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	// 월드의 원점
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	_RSI->DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}


// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	InputManager input = _GameEngine.GetInputManager();

	Transform2D& player =_GameEngine.FindGameObjectWithName("Player")->GetTransform2D();
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * _MoveSpeed * InDeltaSeconds;
	player.AddPosition(deltaPosition);

	Transform2D& camera = _GameEngine.FindGameObjectWithName("Camera")->GetTransform2D();
	camera.SetPosition(camera.GetPosition() * (1.f - InDeltaSeconds) + player.GetPosition() * (InDeltaSeconds));

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	////////////////////// 월드 공간 //////////////////////
	Matrix3x3 viewMat = ((Camera2D*)_GameEngine.FindGameObjectWithName("Camera"))->GetViewMatrix();

	for (auto& go : _GameEngine.GetGameObjects())
	{
		const Mesh2D* mesh = go->GetMesh();
		if (!mesh) continue;

		size_t vertexCount = mesh->_Vertices.size();
		size_t indexCount = mesh->_Indices.size();
		size_t triangleCount = indexCount / 3;

		// 정점 배열과 인덱스 배열 생성
		Vector2* vertices = new Vector2[vertexCount];
		memcpy(vertices, &mesh->_Vertices[0], sizeof(Vector2) * vertexCount);
		int* indices = new int[indexCount];
		memcpy(indices, &mesh->_Indices[0], sizeof(int) * indexCount);

		//_RSI->PushStatisticText(playerTransform.GetPosition().ToString());

		// 변환 행렬의 설계
		Matrix3x3 finalMat = viewMat * go->GetTransform2D().GetModelingMatrix();

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

	}
}