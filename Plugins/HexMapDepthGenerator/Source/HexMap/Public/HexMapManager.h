// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DepthTypes.h"
#include "HexFieldMetadata.h"

class HEXMAP_API FHexMapManager
{
public:
	void SetMetadata(const FHexFieldMetadata& InMetadata) { Metadata = InMetadata; }
	void SetSettings(int32 InGlobalSeed, const TArray<FDepthLevelConfig>& InGenerationLevels);
	const FHexFieldMetadata& GetActualMetadata() const { return Metadata; }

	TArray<TArray<int32>> BuildDepthMap(int32 LevelsCount) const;
	TArray<TArray<int32>> BuildDepthMapParallel(int32 LevelsCount) const;

	void NativeValueGenerationTest();
	void ParallelValueGenerationTest();

private:
	void ApplySeeds(int32 GlobalSeed, TArray<FDepthLevelConfig>& GenerationLevels) const;
	FHexFieldMetadata Metadata;
	int32 GlobalSeed = 12345;
	TArray<FDepthLevelConfig> GenerationLevels;
};
