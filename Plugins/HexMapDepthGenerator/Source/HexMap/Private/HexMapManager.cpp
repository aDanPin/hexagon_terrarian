// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#include "HexMapManager.h"
#include "DepthMapGenerator.h"
#include "Math/RandomStream.h"

// ai generated
void FHexMapManager::SetSettings(int32 InGlobalSeed, const TArray<FDepthLevelConfig>& InGenerationLevels)
{
	GlobalSeed = InGlobalSeed;
	GenerationLevels = InGenerationLevels;
}

// ai generated
void FHexMapManager::ApplySeeds(int32 InGlobalSeed, TArray<FDepthLevelConfig>& InGenerationLevels) const
{
	FRandomStream Rng(InGlobalSeed);
	for (int32 i = 0; i < InGenerationLevels.Num(); i++)
		InGenerationLevels[i].Seed = static_cast<int32>(Rng.GetUnsignedInt());
}

// ai generated
TArray<TArray<int32>> FHexMapManager::BuildDepthMap(int32 LevelsCount) const
{
	FDepthMapGenerator Generator;
	Generator.Width = Metadata.MapWidth;
	Generator.Height = Metadata.MapHeight;
	Generator.LevelsCount = LevelsCount;
	Generator.GenerationLevels = GenerationLevels;
	ApplySeeds(GlobalSeed, Generator.GenerationLevels);
	return Generator.GetQuantizeMap();
}
