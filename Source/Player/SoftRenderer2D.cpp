
#include "Precompiled.h"
#include "SoftRenderer.h"

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 가로 세로 라인 그리기
	ScreenPoint screenHalfSize = _ScreenSize.GetHalf();

	for (int x = screenHalfSize._X; x <= _ScreenSize._X; x += _Grid2DUnit)
	{
		_RSI->DrawFullVerticalLine(x, gridColor);
		if (x > screenHalfSize._X)
		{
			_RSI->DrawFullVerticalLine(2 * screenHalfSize._X - x, gridColor);
		}
	}

	for (int y = screenHalfSize._Y; y <= _ScreenSize._Y; y += _Grid2DUnit)
	{
		_RSI->DrawFullHorizontalLine(y, gridColor);
		if (y > screenHalfSize._Y)
		{
			_RSI->DrawFullHorizontalLine(2 * screenHalfSize._Y - y, gridColor);
		}
	}

	// 월드 축 그리기
	_RSI->DrawFullHorizontalLine(screenHalfSize._Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(screenHalfSize._X, LinearColor::Green);
}

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float time = 0.f;

	time += InDeltaSeconds;
	if (time >= 4.f) time -= 4.f;

	float y = 0.f;
	if (time <= 2.f)
	{
		y = sinf(time * Math::PI);
		_Transform.SetPosition(Vector2(time, y * 0.5f));
	}
	else
	{
		float x = 4 - time;
		y = -sinf(x * Math::PI);
		_Transform.SetPosition(Vector2(x, y * 0.5f));
	}

	float div = time / 4.f;
	Vector3 r(1.f, 0.f, 0.f), g(0.f, 1.f, 0.f), b(0.f, 0.f, 1.f);
	Vector3 color = (r * powf((1.f - div), 2.f) + g * 2.f * div * (1.f - div) + b * powf(div, 2.f));
	_CurrentColor._R = color._X;
	_CurrentColor._G = color._Y;
	_CurrentColor._B = color._Z;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	Vector3 curPos(0.f, 0.f, 1.f);

	for (int i = 0; i < 12; ++i)
	{
		float cos = cosf(Math::Deg2Rad(i * 30.f));
		float sin = sinf(Math::Deg2Rad(i * 30.f));
		Vector3 newPos = (_Transform.GetModelingMatrix() * curPos) * 100.f;
		Matrix3x3 rotMat(Vector3(cos, sin, 0.f), Vector3(-sin, cos, 0.f), Vector3(0.f, 0.f, 1.f));
		newPos = rotMat * newPos;
		_RSI->DrawPoint(newPos.ToVector2(), _CurrentColor);
		_RSI->DrawPoint((newPos + Vector3::UnitX).ToVector2(), _CurrentColor);
		_RSI->DrawPoint((newPos - Vector3::UnitX).ToVector2(), _CurrentColor);
		_RSI->DrawPoint((newPos + Vector3::UnitY).ToVector2(), _CurrentColor);
		_RSI->DrawPoint((newPos - Vector3::UnitY).ToVector2(), _CurrentColor);
	}
}