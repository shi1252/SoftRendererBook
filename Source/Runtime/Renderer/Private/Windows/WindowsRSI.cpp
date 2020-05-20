
#include "Precompiled.h"
#include "Windows/WindowsRSI.h"
#include "Windows/WindowsGDI.h"

WindowsRSI::~WindowsRSI()
{
}

bool WindowsRSI::Init(const ScreenPoint& InScreenSize)
{
	return InitializeGDI(InScreenSize);
}

void WindowsRSI::Shutdown()
{
	ReleaseGDI();
}

void WindowsRSI::Clear(const LinearColor & InClearColor)
{
	FillBuffer(InClearColor.ToColor32());
	ClearDepthBuffer();
}

void WindowsRSI::BeginFrame()
{
}

void WindowsRSI::EndFrame()
{
	SwapBuffer();
}

void WindowsRSI::DrawFullVerticalLine(int InX, const LinearColor & InColor)
{
	if (InX < 0 || InX >= _ScreenSize.X)
	{
		return;
	}

	for (int y = 0; y < _ScreenSize.Y; ++y)
	{
		SetPixel(ScreenPoint(InX, y), InColor);
	}
}

void WindowsRSI::DrawFullHorizontalLine(int InY, const LinearColor & InColor)
{
	if (InY < 0 || InY >= _ScreenSize.Y)
	{
		return;
	}

	for (int x = 0; x < _ScreenSize.X; ++x)
	{
		SetPixel(ScreenPoint(x, InY), InColor);
	}
}

void WindowsRSI::DrawPoint(const Vector2& InVectorPos, const LinearColor& InColor)
{
	SetPixel(ScreenPoint::ToScreenCoordinate(_ScreenSize, InVectorPos), InColor);
}

void WindowsRSI::DrawLine(const Vector2& InStartPos, const Vector2& InEndPos, const LinearColor& InColor)
{
	Vector2 clippedStart = InStartPos;
	Vector2 clippedEnd = InEndPos;
	Vector2 screenExtend = Vector2(_ScreenSize.X, _ScreenSize.Y) * 0.5f;
	Vector2 minScreen = -screenExtend;
	Vector2 maxScreen = screenExtend;
	if (!CohenSutherlandLineClip(clippedStart, clippedEnd, minScreen, maxScreen))
	{
		return;
	}

	ScreenPoint startPosition = ScreenPoint::ToScreenCoordinate(_ScreenSize, clippedStart);
	ScreenPoint endPosition = ScreenPoint::ToScreenCoordinate(_ScreenSize, clippedEnd);

	//PushStatisticText(clippedEnd.ToString());

	int width = endPosition.X - startPosition.X;
	int height = endPosition.Y - startPosition.Y;

	bool isGradualSlope = (Math::Abs(width) >= Math::Abs(height));
	int dx = (width >= 0) ? 1 : -1;
	int dy = (height > 0) ? 1 : -1;
	int fw = dx * width;
	int fh = dy * height;

	int f = isGradualSlope ? fh * 2 - fw : 2 * fw - fh;
	int f1 = isGradualSlope ? 2 * fh : 2 * fw;
	int f2 = isGradualSlope ? 2 * (fh - fw) : 2 * (fw - fh);
	int x = startPosition.X;
	int y = startPosition.Y;

	if (isGradualSlope)
	{
		while (x != endPosition.X)
		{
			SetPixel(ScreenPoint(x, y), InColor);

			if (f < 0)
			{
				f += f1;
			}
			else
			{
				f += f2;
				y += dy;
			}

			x += dx;
		}
	}
	else
	{
		while (y != endPosition.Y)
		{
			SetPixel(ScreenPoint(x, y), InColor);

			if (f < 0)
			{
				f += f1;
			}
			else
			{
				f += f2;
				x += dx;
			}

			y += dy;
		}
	}
}

void WindowsRSI::PushStatisticText(std::string && InText)
{
	_StatisticTexts.emplace_back(InText);
}

void WindowsRSI::PushStatisticTexts(std::vector<std::string> && InTexts)
{
	std::move(InTexts.begin(), InTexts.end(), std::back_inserter(_StatisticTexts));
}

int WindowsRSI::TestRegion(const Vector2& InVectorPos, const Vector2& InMinPos, const Vector2& InMaxPos)
{
	int result = 0;

	if (InVectorPos.X < InMinPos.X)
		result = result | 0b0001;
	else if (InVectorPos.X > InMaxPos.X)
		result = result | 0b0010;

	if (InVectorPos.Y < InMinPos.Y)
		result = result | 0b1000;
	else if (InVectorPos.Y > InMaxPos.Y)
		result = result | 0b0100;

	return result;
}

bool WindowsRSI::CohenSutherlandLineClip(Vector2& InOutStartPos, Vector2& InOutEndPos, const Vector2& InMinPos, const Vector2& InMaxPos)
{
	int t0 = TestRegion(InOutStartPos, InMinPos, InMaxPos);
	int t1 = TestRegion(InOutEndPos, InMinPos, InMaxPos);

	if ((t0 & t1) == 0)
	{
		float baseDegree = Math::Rad2Deg(acosf(InMaxPos.Normalize().Dot(Vector2::UnitX)));
		float degreeT0 = Math::Rad2Deg(acosf(InOutStartPos.Normalize().Dot(Vector2::UnitX)));
		float degreeT1 = Math::Rad2Deg(acosf(InOutEndPos.Normalize().Dot(Vector2::UnitX)));

		float slope = (InOutEndPos.Y - InOutStartPos.Y) / (InOutEndPos.X - InOutStartPos.X);
		if ((degreeT0 < baseDegree) || (degreeT0 > (180.f - baseDegree)))
		{
			if (t0 & 0b0001)
			{
				float y = slope * (InMinPos.X - InOutStartPos.X) + InOutStartPos.Y;
				InOutStartPos.X = InMinPos.X;
				InOutStartPos.Y = y;
			}
			else if (t0 & 0b0010)
			{
				float y = slope * (InMaxPos.X - InOutStartPos.X) + InOutStartPos.Y;
				InOutStartPos.X = InMaxPos.X;
				InOutStartPos.Y = y;
			}
		}
		else
		{
			if (t0 & 0b1000)
			{
				float x = (InMinPos.Y - InOutStartPos.Y) / slope + InOutStartPos.X;

				InOutStartPos.X = x;
				InOutStartPos.Y = InMinPos.Y;
			}
			else if (t0 & 0b0100)
			{
				float x = (InMaxPos.Y - InOutStartPos.Y) / slope + InOutStartPos.X;

				InOutStartPos.X = x;
				InOutStartPos.Y = InMaxPos.Y;
			}
		}

		if ((degreeT1 < baseDegree) || (degreeT1 > (180.f - baseDegree)))
		{
			if (t1 & 0b0001)
			{
				float y = slope * (InMinPos.X - InOutEndPos.X) + InOutEndPos.Y;
				InOutEndPos.X = InMinPos.X;
				InOutEndPos.Y = y;
			}
			else if (t1 & 0b0010)
			{
				float y = slope * (InMaxPos.X - InOutEndPos.X) + InOutEndPos.Y;
				InOutEndPos.X = InMaxPos.X;
				InOutEndPos.Y = y;
			}
		}
		else
		{
			if (t1 & 0b1000)
			{
				float x = (InMinPos.Y - InOutEndPos.Y) / slope + InOutEndPos.X;

				InOutEndPos.X = x;
				InOutEndPos.Y = InMinPos.Y;
			}
			else if (t1 & 0b0100)
			{
				float x = (InMaxPos.Y - InOutEndPos.Y) / slope + InOutEndPos.X;

				InOutEndPos.X = x;
				InOutEndPos.Y = InMaxPos.Y;
			}
		}

		return true;
	}

	return false;
}