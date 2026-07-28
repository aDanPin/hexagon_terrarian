// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DepthTypes.h"

class DEPTHGENERATOR_API FDepthMapGenerator
{
public:
	int32 Width = 64;
	int32 Height = 64;
	int32 LevelsCount = 5;
	TArray<FDepthLevelConfig> GenerationLevels;

	TArray<int32> GetQuantizeMap();
	TArray<int32> GetQuantizeMapParallel();
	static int32 QuantizeDepth(float Value, int32 InLevelsCount);

private:
	TArray<float> Map;
	TArray<float> OperatorMap;

	int32 NumThreads;
	int32 IterationsPerThread;

	void ApplyNoiseLevel(int32 Index);
	void ApplyOperatorMap(const FDepthLevelConfig& Level);
	void ApplyPerlinLevel(const FDepthLevelConfig& Level);
	void ApplyEuclideanLevel(const FDepthLevelConfig& Level);
	void ApplyValueLayer(const FDepthLevelConfig& Level);
	void ApplyPerlinEuclideanLevel(const FDepthLevelConfig& Level);
	static void ShiftsFromSeed(int32 Seed, float& OutX, float& OutY);
};
