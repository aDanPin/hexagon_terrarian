#pragma once

#include "CoreMinimal.h"
#include "DepthTypes.h"
#include "HexFieldMetadata.h"

class AActor;
class UWorld;
class UStaticMesh;

class HEXMAP_API FHexMapManager
{
public:
	void Initialize(UWorld* InWorld) { World = InWorld; }
	void SetMetadata(const FHexFieldMetadata& InMetadata) { Metadata = InMetadata; }
	const FHexFieldMetadata& GetActualMetadata() const { return Metadata; }

	static void ApplySeeds(int32 GlobalSeed, TArray<FDepthLevelConfig>& GenerationLevels);
	TArray<TArray<int32>> BuildDepthMap(int32 LevelsCount, const TArray<FDepthLevelConfig>& GenerationLevels) const;

	void InitMap(AActor* Anchor, const TArray<TArray<int32>>& DepthMap, const TArray<UStaticMesh*>& DepthMeshes);

private:
	UWorld* World = nullptr;
	FHexFieldMetadata Metadata;

	static int32 PickMeshIndex(const TArray<TArray<int32>>& DepthMap, int32 MeshCount, int32 Beta, int32 Alpha);
	void ClearHISM(AActor* Anchor) const;
};
