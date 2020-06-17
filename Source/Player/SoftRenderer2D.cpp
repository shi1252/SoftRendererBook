#include "Precompiled.h"
#include "SoftRenderer.h"
#include <chrono>

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = _GameEngine.GetCamera2D()->GetTransform2D().GetLocalPosition();
		//_GameEngine.FindGameObjectWithName("Camera")->GetTransform2D().GetPosition();
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

	if (input.SpacePressed())
		_QuadCullOn = true;
	else
		_QuadCullOn = false;

	Transform2D& player =_GameEngine.FindGameObjectWithName2D("Player")->GetTransform2D();
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * _MoveSpeed * InDeltaSeconds;
	player.AddLocalPosition(deltaPosition);
	player.AddWorldRotation(90.f * InDeltaSeconds);

	Transform2D& earth = _GameEngine.FindGameObjectWithName2D("Earth")->GetTransform2D();
	earth.AddWorldRotation(45.f * InDeltaSeconds);

	Transform2D& moon = _GameEngine.FindGameObjectWithName2D("Moon")->GetTransform2D();
	moon.AddWorldRotation(20.f * InDeltaSeconds);

	Transform2D& camera = _GameEngine.GetCamera2D()->GetTransform2D();
		//_GameEngine.FindGameObjectWithName("Camera")->GetTransform2D();
	camera.SetLocalPosition(camera.GetLocalPosition() * (1.f - InDeltaSeconds) + player.GetWorldPosition() * (InDeltaSeconds));

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	Camera2D* camera = _GameEngine.GetCamera2D();//(Camera2D*)_GameEngine.FindGameObjectWithName("Camera");

	Circle camCircle = camera->GetCircleBound();
	camCircle.Center += camera->GetTransform2D().GetWorldPosition();

	Vector2 min = Vector2(camera->GetTransform2D().GetWorldPosition() //+ Vector2::One * -100.f);
																 + camera->GetRectBound().Min);
	Vector2 max = Vector2(camera->GetTransform2D().GetWorldPosition() //+ Vector2::One * 100.f);
																 + camera->GetRectBound().Max);
	Rectangle camRect = Rectangle(min, max);

	int numCircleCulled = 0;
	int numCircleDrawed = 0;
	int numRectCulled = 0;
	int numRectDrawed = 0;

	////////////////////// 월드 공간 //////////////////////
	Matrix3x3 viewMat = _GameEngine.GetCamera2D()->GetViewMatrix();
	//((Camera2D*)_GameEngine.FindGameObjectWithName("Camera"))->GetViewMatrix();

	auto start = std::chrono::high_resolution_clock::now();
	if (_QuadCullOn)
	{
		std::vector<GameObject2D*> quadtreeCull;
		_GameEngine.GetQuadtree()->GetIntersectingObjects(camRect, quadtreeCull);

		for (auto& go : quadtreeCull)
		{
			const Mesh2D* mesh = go->GetMesh();
			if (!mesh) continue;

			Circle goCircle = mesh->GetCircleBound();
			goCircle.Center += go->GetTransform2D().GetWorldPosition();
			goCircle.Radius = goCircle.Radius * go->GetTransform2D().GetWorldScale().Max();
			if (!camCircle.Intersect(goCircle))
			{
				++numCircleCulled;
				continue;
			}

			++numCircleDrawed;

			Vector2 min = mesh->GetRectBound().Min * go->GetTransform2D().GetWorldScale().Max();
			Vector2 max = mesh->GetRectBound().Max * go->GetTransform2D().GetWorldScale().Max();
			min = min + go->GetTransform2D().GetWorldPosition();
			max = max + go->GetTransform2D().GetWorldPosition();
			Rectangle goRect = Rectangle(min, max);

			if (!camRect.Intersect(goRect))
			{
				++numRectCulled;
				continue;
			}

			++numRectDrawed;

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
			Matrix3x3 finalMat = viewMat * go->GetTransform2D().GetWorldModelingMatrix();

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
	else
	{
		for (auto& go : _GameEngine.GetGameObjects2D())
		{
			const Mesh2D* mesh = go->GetMesh();
			if (!mesh) continue;

			Circle goCircle = mesh->GetCircleBound();
			goCircle.Center += go->GetTransform2D().GetWorldPosition();
			goCircle.Radius = goCircle.Radius * go->GetTransform2D().GetWorldScale().Max();
			if (!camCircle.Intersect(goCircle))
			{
				++numCircleCulled;
				continue;
			}

			++numCircleDrawed;

			Vector2 min = mesh->GetRectBound().Min * go->GetTransform2D().GetWorldScale().Max();
			Vector2 max = mesh->GetRectBound().Max * go->GetTransform2D().GetWorldScale().Max();
			min = min + go->GetTransform2D().GetWorldPosition();
			max = max + go->GetTransform2D().GetWorldPosition();
			Rectangle goRect = Rectangle(min, max);

			if (!camRect.Intersect(goRect))
			{
				++numRectCulled;
				continue;
			}

			++numRectDrawed;

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
			Matrix3x3 finalMat = viewMat * go->GetTransform2D().GetWorldModelingMatrix();

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
	auto duration = std::chrono::high_resolution_clock::now() - start;
	auto t = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	_RSI->PushStatisticText("t (ns): " + std::to_string(t.count()));

	//std::vector<GameObject2D*> quadtreeCull;
	//_GameEngine.GetQuadtree()->GetWholeObjects(quadtreeCull);
	
	//min = Vector2(-100.f, -100.f);
	//max = Vector2(100.f, 100.f);
	//_RSI->DrawLine(min, Vector2(max.X, min.Y), _CurrentColor);
	//_RSI->DrawLine(min, Vector2(min.X, max.Y), _CurrentColor);
	//_RSI->DrawLine(Vector2(min.X, max.Y), max, _CurrentColor);
	//_RSI->DrawLine(Vector2(max.X, min.Y), max, _CurrentColor);

	_RSI->PushStatisticText("Total: " + std::to_string(numCircleCulled + numCircleDrawed));
	_RSI->PushStatisticText("CircleCulled: " + std::to_string(numCircleCulled));
	_RSI->PushStatisticText("CircleDrawed: " + std::to_string(numCircleDrawed));
	_RSI->PushStatisticText("RectCulled: " + std::to_string(numRectCulled));
	_RSI->PushStatisticText("RectDrawed: " + std::to_string(numRectDrawed));
}