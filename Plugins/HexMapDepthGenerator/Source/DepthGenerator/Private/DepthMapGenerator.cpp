// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

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

// ai generated
void FDepthMapGenerator::ParallelRows(TFunctionRef<void(int32 StartRow, int32 EndRow)> Body) const
{
	const int32 NumThreads = FMath::Max(1, FTaskGraphInterface::Get().GetNumWorkerThreads() + 1);
	const int32 RowsPerChunk = FMath::DivideAndRoundUp(Height, NumThreads);
	if (RowsPerChunk <= 0) return;

	ParallelFor(FMath::DivideAndRoundUp(Height, RowsPerChunk), [&](int32 Chunk)
	{
		Body(Chunk * RowsPerChunk, FMath::Min((Chunk + 1) * RowsPerChunk, Height));
	});
}

// ai generated
TArray<int32> FDepthMapGenerator::GetQuantizeMap()
{
	if (GenerationLevels.Num() == 0)
		GenerationLevels.Add(FDepthLevelConfig());

	Map.Reset();
	Map.SetNumZeroed(Height * Width);

	for (int32 i = 0; i < GenerationLevels.Num(); i++)
		if (GenerationLevels[i].bEnabled) ApplyNoiseLevel(i);

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

	Map.Reset();
	Map.SetNumZeroed(Height * Width);
	TArray<int32> Quantized;
	Quantized.SetNumUninitialized(Height * Width);
	const int32 Levels = FMath::Max(1, LevelsCount);
	const int32 W = Width;

	for (int32 i = 0; i < GenerationLevels.Num(); i++)
		if (GenerationLevels[i].bEnabled) ApplyNoiseLevel(i);

	ParallelRows([&](int32 StartRow, int32 EndRow)
	{
		const float* RESTRICT Src = Map.GetData() + StartRow * W;
		int32* RESTRICT Dst = Quantized.GetData() + StartRow * W;
		for (int32 i = 0, Num = (EndRow - StartRow) * W; i < Num; i++)
			Dst[i] = QuantizeDepth(Src[i], Levels);
	});
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
	case ENoiseLayerType::PerlinEuclidean:
		ApplyPerlinEuclideanLevel(Level);
		break;
	}
}

// ai generated
void FDepthMapGenerator::ApplyRow(const FDepthLevelConfig& Level, int32 Row, const float* Values)
{
	const int32 W = Width;
	const float WinL = Level.ApplyWindowLeft;
	const float WinR = Level.ApplyWindowRight;
	// Lerp(AmplitudeLeft, AmplitudeRight, V) with Invert folded in
	const float Base = Level.Invert ? 1.0f - Level.AmplitudeLeft : Level.AmplitudeLeft;
	const float Delta = Level.Invert ? Level.AmplitudeLeft - Level.AmplitudeRight : Level.AmplitudeRight - Level.AmplitudeLeft;
	float* RESTRICT M = Map.GetData() + Row * W;

	const auto Run = [&](auto Op)
	{
		for (int32 j = 0; j < W; j++)
		{
			const float D = M[j];
			if (D < WinL || D > WinR) continue;
			M[j] = Op(D, Base + Delta * Values[j]);
		}
	};

	switch (Level.ApplyMode)
	{
	case EApplyMode::Add:            Run([](float D, float V) { return D + V; }); break;
	case EApplyMode::Subtract:       Run([](float D, float V) { return D - V; }); break;
	case EApplyMode::Multiply:       Run([](float D, float V) { return D * V; }); break;
	case EApplyMode::Lerp:           Run([](float D, float V) { return FMath::Lerp(D, V, 0.5f); }); break;
	case EApplyMode::Replace:        Run([](float, float V) { return V; }); break;
	case EApplyMode::ReplaceIfAbove: Run([](float D, float V) { return FMath::Max(D, V); }); break;
	case EApplyMode::ReplaceIfBelow: Run([](float D, float V) { return FMath::Min(D, V); }); break;
	}
}

// ai generated
void FDepthMapGenerator::ApplyPerlinLevel(const FDepthLevelConfig& Level)
{
	float XShift, YShift;
	ShiftsFromSeed(Level.Seed, XShift, YShift);

	const int32 W = Width;
	const float StepX = Level.ScaleX * SCALE_X_BASE;
	const float StepY = Level.ScaleY * SCALE_Y_BASE;
	const float Power = Level.PerlinPower;

	ParallelRows([&](int32 StartRow, int32 EndRow)
	{
		TArray<float> Values;
		Values.SetNumUninitialized(W);

		for (int32 j = StartRow; j < EndRow; j++)
		{
			const float Y = j * StepY + YShift;
			for (int32 i = 0; i < W; i++)
			{
				const float N = (FMath::PerlinNoise2D(FVector2D(i * StepX + XShift, Y)) + 1.0f) * 0.5f;
				Values[i] = FMath::Pow(FMath::Clamp(N, 0.0f, 1.0f), Power);
			}
			ApplyRow(Level, j, Values.GetData());
		}
	});
}

// ai generated
void FDepthMapGenerator::ApplyEuclideanLevel(const FDepthLevelConfig& Level)
{
	const int32 W = Width;
	const int32 H = Height;
	const float Power = Level.EuclideanPower;
	const TArray<FEuclideanPoint>& Points = Level.Points;

	ParallelRows([&](int32 StartRow, int32 EndRow)
	{
		TArray<float> Values;
		Values.SetNumUninitialized(W);

		for (int32 j = StartRow; j < EndRow; j++)
		{
			for (int32 i = 0; i < W; i++)
			{
				float Value = 0.0f;
				for (const FEuclideanPoint& Point : Points)
				{
					if (!Point.bEnabled) continue;
					const int32 PosX = FMath::RoundToInt(Point.X * W);
					const int32 PosY = FMath::RoundToInt(Point.Y * H);
					const float Distance = HexCellDistance(i, j, PosX, PosY, 1.0f);

					if (Distance < Point.Radius)
					{
						Value = 1.0f;
						break;
					}

					const float V = 1.0f / ((Distance - Point.Radius) * (Distance - Point.Radius) + 1.0f);
					if (V >= Value)
						Value = V;
				}
				Values[i] = FMath::Pow(Value, Power);
			}
			ApplyRow(Level, j, Values.GetData());
		}
	});
}

// ai generated
void FDepthMapGenerator::ApplyValueLayer(const FDepthLevelConfig& Level)
{
	const int32 W = Width;
	const float Value = Level.Value;

	ParallelRows([&](int32 StartRow, int32 EndRow)
	{
		TArray<float> Values;
		Values.Init(Value, W);

		for (int32 j = StartRow; j < EndRow; j++)
			ApplyRow(Level, j, Values.GetData());
	});
}

// ai generated
void FDepthMapGenerator::ApplyPerlinEuclideanLevel(const FDepthLevelConfig& Level)
{
	float XShift, YShift;
	ShiftsFromSeed(Level.Seed, XShift, YShift);

	const int32 W = Width;
	const int32 H = Height;
	const float StepX = Level.ScaleX * SCALE_X_BASE;
	const float StepY = Level.ScaleY * SCALE_Y_BASE;
	const float PerlinPower = Level.PerlinPower;
	const float EuclideanPower = Level.EuclideanPower;
	const TArray<FEuclideanPoint>& Points = Level.Points;

	ParallelRows([&](int32 StartRow, int32 EndRow)
	{
		TArray<float> Values;
		Values.SetNumUninitialized(W);

		for (int32 j = StartRow; j < EndRow; j++)
		{
			const float Y = j * StepY + YShift;
			for (int32 i = 0; i < W; i++)
			{
				const float N = (FMath::PerlinNoise2D(FVector2D(i * StepX + XShift, Y)) + 1.0f) * 0.5f;
				float PerlinValue = FMath::Clamp(N, 0.0f, 1.0f);

				float EuclideanValue = 0.0f;
				for (const FEuclideanPoint& Point : Points)
				{
					if (!Point.bEnabled) continue;
					const int32 PosX = FMath::RoundToInt(Point.X * W);
					const int32 PosY = FMath::RoundToInt(Point.Y * H);
					const float Distance = HexCellDistance(i, j, PosX, PosY, 1.0f);

					if (Distance < Point.Radius)
					{
						EuclideanValue = 1.0f;
						break;
					}

					const float V = 1.0f / ((Distance - Point.Radius) * (Distance - Point.Radius) + 1.0f);
					if (V >= EuclideanValue)
						EuclideanValue = V;
				}

				PerlinValue = FMath::Pow(PerlinValue, PerlinPower);
				EuclideanValue = FMath::Pow(EuclideanValue, EuclideanPower);
				Values[i] = PerlinValue * EuclideanValue;
			}
			ApplyRow(Level, j, Values.GetData());
		}
	});
}
