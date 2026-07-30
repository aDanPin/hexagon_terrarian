// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

#include "DepthMapGenerator.h"
#include "Math/RandomStream.h"
#include "HAL/PlatformMisc.h"


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
TArray<int32> FDepthMapGenerator::GetQuantizeMap()
{
	if (GenerationLevels.Num() == 0)
		GenerationLevels.Add(FDepthLevelConfig());

	Map.Reset();
	Map.SetNumZeroed(Height * Width);

	// for (int32 i = 0; i < GenerationLevels.Num(); i++)
		//if (GenerationLevels[i].bEnabled) ApplyNoiseLevel(i);

	const int32 Levels = FMath::Max(1, LevelsCount);
	TArray<int32> Quantized;
	Quantized.SetNumUninitialized(Height * Width);
	for (int32 i = 0; i < Height * Width; i++)
		Quantized[i] = QuantizeDepth(Map[i], Levels);

	return Quantized;
}

TArray<int32> FDepthMapGenerator::GetQuantizeMapParallel()
{
	if (GenerationLevels.Num() == 0)
		GenerationLevels.Add(FDepthLevelConfig());

	TArray<int32> Quantized;
	Quantized.SetNumUninitialized(Height * Width);
	Map.Reset();
	Map.SetNumZeroed(Height * Width);
	
	const int32 Levels = FMath::Max(1, LevelsCount);
	const int32 W = Width;

	ParallelFor(Height, [&, Levels, W](int32 index) {
		float* RESTRICT Src = Map.GetData() + index * W;
		int32* RESTRICT Dst = Quantized.GetData() + index * W;
		
		for (int32 i = 0; i < GenerationLevels.Num(); i++) {
			if (GenerationLevels[i].bEnabled) ApplyNoiseLevel(GenerationLevels[i], index, Src, Dst);
		}
	});

	return Quantized;
}
int32 FDepthMapGenerator::QuantizeDepth(float Value, int32 InLevelsCount)
{
	for (int32 i = 0; i < InLevelsCount; i++)
	{
		if (Value < (i + 1.0f) / InLevelsCount)
			return i + 1;
	}
	return InLevelsCount;
}
void FDepthMapGenerator::ShiftsFromSeed(int32 Seed, float& OutX, float& OutY)
{
	FRandomStream Rng(Seed);
	OutX = Rng.FRand() * 1000.0f;
	OutY = Rng.FRand() * 1000.0f;
}
inline void FDepthMapGenerator::ApplyNoiseLevel(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Src, int32* RESTRICT Dst)
{
	switch (Level.Type)
	{
	case ENoiseLayerType::Perlin:
		ApplyPerlinLevel(Level, Row, Src, Dst);
		break;
	case ENoiseLayerType::Euclidean:
		ApplyEuclideanLevel(Level, Row, Src, Dst);
		break;
	case ENoiseLayerType::Value:
		ApplyValueLayer(Level, Row, Src, Dst);
		break;
	case ENoiseLayerType::PerlinEuclidean:
		ApplyPerlinEuclideanLevel(Level, Row, Src, Dst);
		break;
	}
}

inline float FDepthMapGenerator::Apply(const FDepthLevelConfig& Level, float LeftValue, float RightValue)
{
	if (LeftValue < Level.ApplyWindowLeft || LeftValue > Level.ApplyWindowRight)
		return LeftValue;

	float OperatorValue = FMath::Lerp(Level.AmplitudeLeft, Level.AmplitudeRight, RightValue);
	if (Level.Invert)
		OperatorValue = 1.0f - OperatorValue;

	switch (Level.ApplyMode)
	{
	case EApplyMode::Add:            return LeftValue + OperatorValue;
	case EApplyMode::Subtract:       return LeftValue - OperatorValue;
	case EApplyMode::Multiply:       return LeftValue * OperatorValue;
	case EApplyMode::Lerp:           return FMath::Lerp(LeftValue, OperatorValue, 0.5f);
	case EApplyMode::Replace:        return OperatorValue;
	case EApplyMode::ReplaceIfAbove: return FMath::Max(LeftValue, OperatorValue);
	case EApplyMode::ReplaceIfBelow: return FMath::Min(LeftValue, OperatorValue);
	}
	return LeftValue;
}
void FDepthMapGenerator::ApplyPerlinLevel(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Src, int32* RESTRICT Dst)
{
	float XShift, YShift;
	ShiftsFromSeed(Level.Seed, XShift, YShift);
	const float StepX = Level.ScaleX * SCALE_X_BASE;
	const float StepY = Level.ScaleY * SCALE_Y_BASE;
	const float Y = Row * StepY + YShift;

	auto is = Src;
	auto id = Dst;
	for (int32 Col = 0; is < Src + Width; is++, id++, Col++)
	{
		const float N = (FMath::PerlinNoise2D(FVector2D(Col * StepX + XShift, Y)) + 1.0f) * 0.5f;
		const float PerlinValue = FMath::Pow(FMath::Clamp(N, 0.0f, 1.0f), Level.PerlinPower);
		const float Value = Apply(Level, *is, PerlinValue);
		*is = Value;
		*id = QuantizeDepth(Value, FMath::Max(1, LevelsCount));
	}
}
void FDepthMapGenerator::ApplyEuclideanLevel(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Src, int32* RESTRICT Dst)
{
	auto is = Src;
	auto id = Dst;
	for (int32 Col = 0; is < Src + Width; is++, id++, Col++)
	{
		float Value = 0.0f;
		for (const FEuclideanPoint& Point : Level.Points)
		{
			if (!Point.bEnabled) continue;
			const int32 PosX = FMath::RoundToInt(Point.X * Width);
			const int32 PosY = FMath::RoundToInt(Point.Y * Height);
			const float Distance = HexCellDistance(Col, Row, PosX, PosY, 1.0f);

			if (Distance < Point.Radius)
			{
				Value = 1.0f;
				break;
			}

			const float V = 1.0f / ((Distance - Point.Radius) * (Distance - Point.Radius) + 1.0f);
			if (V >= Value)
				Value = V;
		}
		Value = FMath::Pow(Value, Level.EuclideanPower);
		Value = Apply(Level, *is, Value);
		*is = Value;
		*id = QuantizeDepth(Value, FMath::Max(1, LevelsCount));
	}
}
void FDepthMapGenerator::ApplyValueLayer(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Src, int32* RESTRICT Dst)
{
	auto is = Src;
	auto id = Dst;
	for (; is < Src + Width; is++, id++)
	{
		const float Value = Apply(Level, *is, Level.Value);
		*is = Value;
		*id = QuantizeDepth(Value, FMath::Max(1, LevelsCount));
	}
}
void FDepthMapGenerator::ApplyPerlinEuclideanLevel(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Src, int32* RESTRICT Dst)
{
	float XShift, YShift;
	ShiftsFromSeed(Level.Seed, XShift, YShift);
	const float StepX = Level.ScaleX * SCALE_X_BASE;
	const float StepY = Level.ScaleY * SCALE_Y_BASE;
	const float Y = Row * StepY + YShift;

	auto is = Src;
	auto id = Dst;
	for (int32 Col = 0; is < Src + Width; is++, id++, Col++)
	{
		const float N = (FMath::PerlinNoise2D(FVector2D(Col * StepX + XShift, Y)) + 1.0f) * 0.5f;
		float PerlinValue = FMath::Clamp(N, 0.0f, 1.0f);

		float EuclideanValue = 0.0f;
		for (const FEuclideanPoint& Point : Level.Points)
		{
			if (!Point.bEnabled) continue;
			const int32 PosX = FMath::RoundToInt(Point.X * Width);
			const int32 PosY = FMath::RoundToInt(Point.Y * Height);
			const float Distance = HexCellDistance(Col, Row, PosX, PosY, 1.0f);

			if (Distance < Point.Radius)
			{
				EuclideanValue = 1.0f;
				break;
			}

			const float V = 1.0f / ((Distance - Point.Radius) * (Distance - Point.Radius) + 1.0f);
			if (V >= EuclideanValue)
				EuclideanValue = V;
		}

		PerlinValue = FMath::Pow(PerlinValue, Level.PerlinPower);
		EuclideanValue = FMath::Pow(EuclideanValue, Level.EuclideanPower);
		const float Value = Apply(Level, *is, PerlinValue * EuclideanValue);
		*is = Value;
		*id = QuantizeDepth(Value, FMath::Max(1, LevelsCount));
	}
}
