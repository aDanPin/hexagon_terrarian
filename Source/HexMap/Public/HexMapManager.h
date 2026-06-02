#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "HexFieldMetadata.h"

class AActor;
class UWorld;
class UHexChipComponent;

class HEXMAP_API FHexMapManager
{
public:
	void Initialize(UWorld* InWorld) { World = InWorld; }
	void SetMetadata(const FHexFieldMetadata& InMetadata) { Metadata = InMetadata; }
	const FHexFieldMetadata& GetActualMetadata() const { return Metadata; }
	void ClearMapStorage() { HexStorage.Empty(); }

	bool TryGetHex(int32 Alpha, int32 Beta, AActor*& OutHex) const;
	void ReplaceHexPrefab(UHexChipComponent* Chip, TSubclassOf<AActor> Prefab);
	void InitMap(AActor* Anchor) { InitMap(Anchor, {}, {}); }
	void InitMap(AActor* Anchor, const TArray<TArray<int32>>& DepthMap, const TArray<TSubclassOf<AActor>>& DepthPrefabs);
	TSet<AActor*> GetHexesInRadius(UHexChipComponent* Center, int32 R);

private:
	UWorld* World = nullptr;
	FHexFieldMetadata Metadata;
	TArray<TArray<AActor*>> HexStorage;

	AActor* SpawnHex(TSubclassOf<AActor> Prefab, const FVector& Location, AActor* Anchor, int32 Alpha, int32 Beta);
	static TSubclassOf<AActor> PickDepthPrefab(const TArray<TArray<int32>>& DepthMap, const TArray<TSubclassOf<AActor>>& DepthPrefabs, int32 Beta, int32 Alpha, TSubclassOf<AActor> Fallback);
	bool IsValidCoord(int32 Nx, int32 Ny) const;
	TArray<AActor*> GetHexesNearHexes(UHexChipComponent* Center) const;
};
