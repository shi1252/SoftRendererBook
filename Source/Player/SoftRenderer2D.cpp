
#include "Precompiled.h"
#include "SoftRenderer.h"

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = _GameEngine.FindGameObjectWithName("Camera")->GetTransform2D().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// ���� �ϴܿ������� ���� �׸���
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
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

	// ������ ����
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	_RSI->DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}


// ���� ����
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
	// ���� �׸���
	DrawGrid2D();

	////////////////////// ���� ���� //////////////////////
	Matrix3x3 viewMat = ((Camera2D*)_GameEngine.FindGameObjectWithName("Camera"))->GetViewMatrix();

	for (auto& go : _GameEngine.GetGameObjects())
	{
		const Mesh2D* mesh = go->GetMesh();
		if (!mesh) continue;

		size_t vertexCount = mesh->_Vertices.size();
		size_t indexCount = mesh->_Indices.size();
		size_t triangleCount = indexCount / 3;

		// ���� �迭�� �ε��� �迭 ����
		Vector2* vertices = new Vector2[vertexCount];
		memcpy(vertices, &mesh->_Vertices[0], sizeof(Vector2) * vertexCount);
		int* indices = new int[indexCount];
		memcpy(indices, &mesh->_Indices[0], sizeof(int) * indexCount);

		//_RSI->PushStatisticText(playerTransform.GetPosition().ToString());

		// ��ȯ ����� ����
		Matrix3x3 finalMat = viewMat * go->GetTransform2D().GetModelingMatrix();

		// ������ ����� ����
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = finalMat * vertices[vi];
		}

		// ��ȯ�� ������ �մ� �� �׸���
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi = ti * 3;
			_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 1]], _CurrentColor);
			_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 2]], _CurrentColor);
			_RSI->DrawLine(vertices[indices[bi + 1]], vertices[indices[bi + 2]], _CurrentColor);
		}

	}
}