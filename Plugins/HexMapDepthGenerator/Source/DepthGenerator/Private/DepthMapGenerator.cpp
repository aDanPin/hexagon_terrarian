// Copyright DaniElRuso 2026. All Rights Reserved.

#include "DepthMapGenerator.h"
#include "Math/RandomStream.h"

namespace
{
	constexpr float SCALE_X_BASE = 0.08f;
	constexpr float SCALE_Y_BASE = 0.04f;

	float AlphaBettaToPx(int32 Alpha, int32 Beta, float HexRadius)
	{
		float X = 3.0f * HexRadius * Alpha;
		if ((Beta & 1) != 0)
			X += 1.5f * HexRadius;
		return X;
	}

	float AlphaBettaToPz(int32 Alpha, int32 Beta, float HexRadius)
	{
		return -FMath::Cos(FMath::DegreesToRadians(30.0f)) * HexRadius * Beta;
	}

	float HexCellDistance(int32 X1, int32 Y1, int32 X2, int32 Y2, float HexRadius)
	{
		float PX1 = AlphaBettaToPx(X1, Y1, HexRadius);
		float PZ1 = AlphaBettaToPz(X1, Y1, HexRadius);
		float PX2 = AlphaBettaToPx(X2, Y2, HexRadius);
		float PZ2 = AlphaBettaToPz(X2, Y2, HexRadius);
		return FMath::Sqrt((PX1 - PX2) * (PX1 - PX2) + (PZ1 - PZ2) * (PZ1 - PZ2)) / (2.0f * HexRadius);
	}
}

// ai generated
TArray<TArray<int32>> FDepthMapGenerator::GetQuantizeMap()
{
	if (GenerationLevels.Num() == 0)
		GenerationLevels.Add(FDepthLevelConfig());

	Map.SetNum(Height);
	for (int32 j = 0; j < Height; j++)
		Map[j].Init(0.0f, Width);

	for (int32 i = 0; i < GenerationLevels.Num(); i++)
		if (GenerationLevels[i].bEnabled) ApplyNoiseLevel(i);

	int32 Levels = FMath::Max(1, LevelsCount);
	TArray<TArray<int32>> Quantized;
	Quantized.SetNum(Height);
	for (int32 i = 0; i < Height; i++)
	{
		Quantized[i].SetNum(Width);
		for (int32 j = 0; j < Width; j++)
			Quantized[i][j] = QuantizeDepth(Map[i][j], Levels);
	}
	return Quantized;
}

// ai generated
int32 FDepthMapGenerator::QuantizeDepth(float Value, int32 InLevelsCount)
{
	for (int32 i = 0; i < InLevelsCount; i++)
	{
		if (Value < (i + 1.0f) / InLevelsCount)
			return i + 1;
	}
	return InLevelsCount;
}

// ai generated
void FDepthMapGenerator::ShiftsFromSeed(int32 Seed, float& OutX, float& OutY)
{
	FRandomStream Rng(Seed);
	OutX = Rng.FRand() * 1000.0f;
	OutY = Rng.FRand() * 1000.0f;
}

// ai generated
void FDepthMapGenerator::ApplyNoiseLevel(int32 Index)
{
	const FDepthLevelConfig& Level = GenerationLevels[Index];

	switch (Level.Type)
	{
	case ENoiseLayerType::Perlin:
		ApplyPerlinLevel(Level);
		break;
	case ENoiseLayerType::Euclidean:
		ApplyEuclideanLevel(Level);
		break;
	case ENoiseLayerType::Value:
		ApplyValueLayer(Level);
		break;
	}

	ApplyOperatorMap(Level);
}

// ai generated
void FDepthMapGenerator::ApplyOperatorMap(const FDepthLevelConfig& Level)
{
	if (OperatorMap.Num() == 0) return;

	for (int32 i = 0; i < Height; i++)
	{
		for (int32 j = 0; j < Width; j++)
		{
			float OperatorValue = OperatorMap[i][j];

			if (!Level.bIncludeLeft && OperatorValue <= 0.0f) continue;
			if (!Level.bIncludeRight && OperatorValue >= 1.0f) continue;

			OperatorValue = FMath::Pow(OperatorValue, Level.Power);
			OperatorValue = FMath::Clamp(OperatorValue, Level.AmplitudeLeft, Level.AmplitudeRight);

			switch (Level.ApplyMode)
			{
			case EApplyMode::Add:
				Map[i][j] += OperatorValue;
				break;
			case EApplyMode::Subtract:
				Map[i][j] -= OperatorValue;
				break;
			case EApplyMode::Replace:
				Map[i][j] = OperatorValue;
				break;
			case EApplyMode::Lerp:
				Map[i][j] = FMath::Lerp(Map[i][j], OperatorValue, Level.Power);
				break;
			case EApplyMode::Multiply:
				Map[i][j] *= OperatorValue;
				break;
			}
		}
	}
}

// ai generated
void FDepthMapGenerator::ApplyPerlinLevel(const FDepthLevelConfig& Level)
{
	float XShift, YShift;
	ShiftsFromSeed(Level.Seed, XShift, YShift);

	OperatorMap.SetNum(Height);
	for (int32 j = 0; j < Height; j++)
	{
		OperatorMap[j].SetNum(Width);
		for (int32 i = 0; i < Width; i++)
		{
			float N = (FMath::PerlinNoise2D(FVector2D(
				i * Level.ScaleX * SCALE_X_BASE + XShift,
				j * Level.ScaleY * SCALE_Y_BASE + YShift)) + 1.0f) * 0.5f;
			OperatorMap[j][i] = FMath::Clamp(N, 0.0f, 1.0f);
		}
	}
}

// ai generated
void FDepthMapGenerator::ApplyEuclideanLevel(const FDepthLevelConfig& Level)
{
	OperatorMap.SetNum(Height);
	for (int32 j = 0; j < Height; j++)
	{
		OperatorMap[j].SetNum(Width);
		for (int32 i = 0; i < Width; i++)
		{
			float Value = 0.0f;
			for (const FEuclideanPoint& Point : Level.Points)
			{
				if (!Point.bEnabled) continue;
				int32 PosX = FMath::RoundToInt(Point.X * Width);
				int32 PosY = FMath::RoundToInt(Point.Y * Height);
				float Distance = HexCellDistance(i, j, PosX, PosY, 1.0f);
				
				if (Distance < Point.Radius) {
					Value = 1.0f;
					break;
				}

				float V = 1.0f / ((Distance - Point.Radius) * (Distance - Point.Radius) + 1.0f);
				if (V >= 0.0f && V <= 1.0f && V >= Value)
					Value = V;
			}
			OperatorMap[j][i] = Value;
		}
	}
}

// ai generated
void FDepthMapGenerator::ApplyValueLayer(const FDepthLevelConfig& Level)
{
	OperatorMap.SetNum(Height);
	for (int32 j = 0; j < Height; j++)
	{
		OperatorMap[j].Init(Level.Value, Width);
	}
}
