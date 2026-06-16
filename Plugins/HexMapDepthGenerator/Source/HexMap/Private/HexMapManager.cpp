#include "HexMapManager.h"
#include "DepthMapGenerator.h"
#include "Math/RandomStream.h"

// ai generated
void FHexMapManager::ApplySeeds(int32 GlobalSeed, TArray<FDepthLevelConfig>& GenerationLevels)
{
	FRandomStream Rng(GlobalSeed);
	for (int32 i = 0; i < GenerationLevels.Num(); i++)
		GenerationLevels[i].Seed = static_cast<int32>(Rng.GetUnsignedInt());
}

// ai generated
TArray<TArray<int32>> FHexMapManager::BuildDepthMap(int32 LevelsCount, const TArray<FDepthLevelConfig>& GenerationLevels) const
{
	FDepthMapGenerator Generator;
	Generator.Width = Metadata.MapWidth;
	Generator.Height = Metadata.MapHeight;
	Generator.LevelsCount = LevelsCount;
	Generator.GenerationLevels = GenerationLevels;
	return Generator.GetQuantizeMap();
}
