#pragma once

#include "CoreMinimal.h"
#include "DepthTypes.h"
#include "HexFieldMetadata.h"

class HEXMAP_API FHexMapManager
{
public:
	void SetMetadata(const FHexFieldMetadata& InMetadata) { Metadata = InMetadata; }
	const FHexFieldMetadata& GetActualMetadata() const { return Metadata; }

	static void ApplySeeds(int32 GlobalSeed, TArray<FDepthLevelConfig>& GenerationLevels);
	TArray<TArray<int32>> BuildDepthMap(int32 LevelsCount, const TArray<FDepthLevelConfig>& GenerationLevels) const;

private:
	FHexFieldMetadata Metadata;
};
