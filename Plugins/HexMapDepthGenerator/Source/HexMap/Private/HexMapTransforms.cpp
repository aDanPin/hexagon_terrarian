// Copyright DaniElRuso 2026. All Rights Reserved.

#include "HexMapTransforms.h"

// ai generated
FIntPoint FHexMapTransforms::AlphaBettaToXY(int32 Alpha, int32 Beta)
{
	int32 AlphaBased = Beta / 2;
	int32 Step = Alpha - AlphaBased;
	return FIntPoint(Step, Beta + Step);
}

// ai generated
FIntPoint FHexMapTransforms::XYToBetaAlfa(int32 X, int32 Y)
{
	return FIntPoint(XYToBeta(X, Y), XYToAlpha(X, Y));
}

// ai generated
int32 FHexMapTransforms::XYToAlpha(int32 X, int32 Y)
{
	return (X + Y) / 2;
}

// ai generated
int32 FHexMapTransforms::XYToBeta(int32 X, int32 Y)
{
	return Y - X;
}

// ai generated
float FHexMapTransforms::AlphaBettaToPx(int32 Alpha, int32 Beta, float HexRadius)
{
	float X = 3.0f * HexRadius * Alpha;
	if ((Beta & 1) != 0)
		X += 1.5f * HexRadius;
	return X;
}

// ai generated
float FHexMapTransforms::AlphaBettaToPz(int32 Alpha, int32 Beta, float HexRadius)
{
	return -FMath::Cos(FMath::DegreesToRadians(30.0f)) * HexRadius * Beta;
}

// ai generated
bool FHexMapTransforms::TryNearestAlphaBetaFromLocalXZ(float Lx, float Lz, float HexRadius, int32 MapWidth, int32 MapHeight, int32& OutAlpha, int32& OutBeta)
{
	float Best = TNumericLimits<float>::Max();
	OutAlpha = OutBeta = 0;
	int32 BetaGuess = FMath::RoundToInt(-2.0f * Lz / HexRadius);
	float HalfSqrt3 = HexRadius * FMath::Sqrt(3.0f) / 2.0f;
	for (int32 Db = -2; Db <= 2; Db++)
	{
		int32 B = BetaGuess + Db;
		if (B < 0 || B >= MapHeight) continue;
		float Stagger = ((B & 1) != 0) ? 0.5f * HexRadius : 0.0f;
		int32 AGuess = FMath::RoundToInt((Lx - Stagger) / HalfSqrt3);
		for (int32 Da = -2; Da <= 2; Da++)
		{
			int32 A = AGuess + Da;
			if (A < 0 || A >= MapWidth) continue;
			float Px = AlphaBettaToPx(A, B, HexRadius);
			float Pz = AlphaBettaToPz(A, B, HexRadius);
			float D = (Lx - Px) * (Lx - Px) + (Lz - Pz) * (Lz - Pz);
			if (D < Best)
			{
				Best = D;
				OutAlpha = A;
				OutBeta = B;
			}
		}
	}
	return Best < TNumericLimits<float>::Max() * 0.5f;
}

// ai generated
float FHexMapTransforms::Distance(int32 X1, int32 Y1, int32 X2, int32 Y2, float HexRadius)
{
	float PX1 = AlphaBettaToPx(X1, Y1, HexRadius);
	float PZ1 = AlphaBettaToPz(X1, Y1, HexRadius);
	float PX2 = AlphaBettaToPx(X2, Y2, HexRadius);
	float PZ2 = AlphaBettaToPz(X2, Y2, HexRadius);
	return FMath::Sqrt((PX1 - PX2) * (PX1 - PX2) + (PZ1 - PZ2) * (PZ1 - PZ2)) / (2.0f * HexRadius);
}

// ai generated
float FHexMapTransforms::DistanceInHex(int32 X1, int32 Y1, int32 X2, int32 Y2)
{
	return (X2 - X1) + (Y2 - Y1);
}
