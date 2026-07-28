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

	void PrepareMap();
	inline float Apply(const FDepthLevelConfig& Level, float LeftValue, float RightValue);
	void ApplyNoiseLevel(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Map, int32* RESTRICT Quantized);
	void ApplyPerlinLevel(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Map, int32* RESTRICT Quantized);
	void ApplyEuclideanLevel(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Map, int32* RESTRICT Quantized);
	void ApplyValueLayer(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Map, int32* RESTRICT Quantized);
	void ApplyPerlinEuclideanLevel(const FDepthLevelConfig& Level, int32 Row, float* RESTRICT Map, int32* RESTRICT Quantized);
	static void ShiftsFromSeed(int32 Seed, float& OutX, float& OutY);
};
