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
TArray<int32> FDepthMapGenerator::GetQuantizeMap()
{
	if (GenerationLevels.Num() == 0)
		GenerationLevels.Add(FDepthLevelConfig());

	Map.SetNum(Height * Width);
	OperatorMap.SetNum(Height * Width);
	NumThreads = FMath::Max(1, FTaskGraphInterface::Get().GetNumWorkerThreads());
	IterationsPerThread = FMath::Max(1, Height / NumThreads);

	for (int32 i = 0; i < GenerationLevels.Num(); i++)
		if (GenerationLevels[i].bEnabled) ApplyNoiseLevel(i);

	int32 Levels = FMath::Max(1, LevelsCount);
	TArray<int32> Quantized;
	Quantized.SetNum(Height * Width);
	for (int32 i = 0; i < Height; i++)
	{
		for (int32 j = 0; j < Width; j++)
			Quantized[i * Width + j] = QuantizeDepth(Map[i * Width + j], Levels);
	}
	return Quantized;
}

TArray<int32> FDepthMapGenerator::GetQuantizeMapParallel()
{
	NumThreads = FMath::Max(1, FTaskGraphInterface::Get().GetNumWorkerThreads());
	IterationsPerThread = FMath::Max(1, Height / NumThreads);

	if (GenerationLevels.Num() == 0)
		GenerationLevels.Add(FDepthLevelConfig());

	Map.SetNum(Height * Width);
	OperatorMap.SetNum(Height * Width);

	for (int32 i = 0; i < GenerationLevels.Num(); i++) {
		if (GenerationLevels[i].bEnabled) ApplyNoiseLevel(i);
	}

	int32 Levels = FMath::Max(1, LevelsCount);
	TArray<int32> Quantized;
	Quantized.SetNum(Height * Width);

	ParallelFor(NumThreads, [&](int32 Index)
	{
		int32 StartIDX = Index * IterationsPerThread;
		int32 EndIDX = FMath::Min(StartIDX + IterationsPerThread, Height);
		for (int32 i = StartIDX; i < EndIDX; i++)
		{
			for (int32 j = 0; j < Width; j++)
				Quantized[i * Width + j] = QuantizeDepth(Map[i * Width + j], Levels);
		}
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

	ApplyOperatorMap(Level);
}

// ai generated
void FDepthMapGenerator::ApplyOperatorMap(const FDepthLevelConfig& Level)
{
	if (OperatorMap.Num() == 0) return;

	NumThreads = FTaskGraphInterface::Get().GetNumWorkerThreads();
	IterationsPerThread = Height / NumThreads;

	ParallelFor(NumThreads, [&](int32 Index)
	{
		int32 StartIDX = Index * IterationsPerThread;
		int32 EndIDX = FMath::Min(StartIDX + IterationsPerThread, Height);
		for (int32 i = StartIDX; i < EndIDX; i++)
		{
			for (int32 j = 0; j < Width; j++)
			{
				if (Map[i * Width + j] < Level.ApplyWindowLeft || Map[i * Width + j] > Level.ApplyWindowRight)
					continue;
	
				float OperatorValue = OperatorMap[i * Width + j];
				OperatorValue = FMath::Lerp(Level.AmplitudeLeft, Level.AmplitudeRight, OperatorValue);
				OperatorValue = Level.Invert ? 1.0f - OperatorValue : OperatorValue;
	
				switch (Level.ApplyMode)
				{
				case EApplyMode::Add:
					Map[i * Width + j] += OperatorValue;
					break;
				case EApplyMode::Subtract:
					Map[i * Width + j] -= OperatorValue;
					break;
				case EApplyMode::Replace:
					Map[i * Width + j] = OperatorValue;
					break;
				case EApplyMode::ReplaceIfAbove:
					if (Map[i * Width + j] < OperatorValue)
						Map[i * Width + j] = OperatorValue;
					break;
				case EApplyMode::ReplaceIfBelow:
					if (Map[i * Width + j] > OperatorValue)
						Map[i * Width + j] = OperatorValue;
					break;
				case EApplyMode::Lerp:
					Map[i * Width + j] = FMath::Lerp(Map[i * Width + j], OperatorValue, 0.5f);
					break;
				case EApplyMode::Multiply:
					Map[i * Width + j] *= OperatorValue;
					break;
				}
			}
		}
	});

}

// ai generated
void FDepthMapGenerator::ApplyPerlinLevel(const FDepthLevelConfig& Level)
{
	float XShift, YShift;
	ShiftsFromSeed(Level.Seed, XShift, YShift);

	ParallelFor(NumThreads, [&](int32 Index)
	{
		int32 StartIDX = Index * IterationsPerThread;
		int32 EndIDX = FMath::Min(StartIDX + IterationsPerThread, Height);
		for (int32 j = StartIDX; j < EndIDX; j++)
		{
			for (int32 i = 0; i < Width; i++)
			{
				float N = (FMath::PerlinNoise2D(FVector2D(
					i * Level.ScaleX * SCALE_X_BASE + XShift,
					j * Level.ScaleY * SCALE_Y_BASE + YShift)) + 1.0f) * 0.5f;
				OperatorMap[j * Width + i] = FMath::Pow(FMath::Clamp(N, 0.0f, 1.0f), Level.PerlinPower);
			}
		}
	});
}

// ai generated
void FDepthMapGenerator::ApplyEuclideanLevel(const FDepthLevelConfig& Level)
{
	
	NumThreads = FTaskGraphInterface::Get().GetNumWorkerThreads();
	IterationsPerThread = Height / NumThreads;

	ParallelFor(NumThreads, [&](int32 Index)
	{
		int32 StartIDX = Index * IterationsPerThread;
		int32 EndIDX = FMath::Min(StartIDX + IterationsPerThread, Height);
		for (int32 j = StartIDX; j < EndIDX; j++)
		{
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
				OperatorMap[j * Width + i] = FMath::Pow(Value, Level.EuclideanPower);
			}
		}
	});
}

// ai generated
void FDepthMapGenerator::ApplyValueLayer(const FDepthLevelConfig& Level)
{
	NumThreads = FTaskGraphInterface::Get().GetNumWorkerThreads();
	IterationsPerThread = Height / NumThreads;

	ParallelFor(NumThreads, [&](int32 Index)
	{
		int32 StartIDX = Index * IterationsPerThread;
		int32 EndIDX = FMath::Min(StartIDX + IterationsPerThread, Height);
		for (int32 j = StartIDX; j < EndIDX; j++)
		{
			for (int32 i = 0; i < Width; i++)
			{
				OperatorMap[j * Width + i] = Level.Value;
			}
		}
	});
}

// ai generated
void FDepthMapGenerator::ApplyPerlinEuclideanLevel(const FDepthLevelConfig& Level)
{
	float XShift, YShift;
 	ShiftsFromSeed(Level.Seed, XShift, YShift);

	NumThreads = FTaskGraphInterface::Get().GetNumWorkerThreads();
	IterationsPerThread = Height / NumThreads;
 
	ParallelFor(NumThreads, [&](int32 Index)
	{
		 int32 StartIDX = Index * IterationsPerThread;
		 int32 EndIDX = FMath::Min(StartIDX + IterationsPerThread, Height);
		 for (int32 j = StartIDX; j < EndIDX; j++)
		 {
			 for (int32 i = 0; i < Width; i++)
			 {
				 float N = (FMath::PerlinNoise2D(FVector2D(
					 i * Level.ScaleX * SCALE_X_BASE + XShift,
					 j * Level.ScaleY * SCALE_Y_BASE + YShift)) + 1.0f) * 0.5f;
				 float PerlinValue = FMath::Clamp(N, 0.0f, 1.0f);
	 
				 float EuclideanValue = 0.0f;
				 for (const FEuclideanPoint& Point : Level.Points)
				 {
					 if (!Point.bEnabled) continue;
					 int32 PosX = FMath::RoundToInt(Point.X * Width);
					 int32 PosY = FMath::RoundToInt(Point.Y * Height);
					 float Distance = HexCellDistance(i, j, PosX, PosY, 1.0f);
					 
					 if (Distance < Point.Radius) {
						 EuclideanValue = 1.0f;
						 break;
					 }
	 
					 float V = 1.0f / ((Distance - Point.Radius) * (Distance - Point.Radius) + 1.0f);
					 if (V >= 0.0f && V <= 1.0f && V >= EuclideanValue)
						 EuclideanValue = V;
				 }
	 
	 
				 PerlinValue = FMath::Pow(PerlinValue, Level.PerlinPower);
				 EuclideanValue = FMath::Pow(EuclideanValue, Level.EuclideanPower);
				 OperatorMap[j * Width + i] = PerlinValue * EuclideanValue;
			 }
		 }
	});

}
