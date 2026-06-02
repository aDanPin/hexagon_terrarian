#include "HexMapGeneratorComponent.h"
#include "DepthMapGenerator.h"
#include "HexChipComponent.h"
#include "HexFieldMetadata.h"
#include "GameFramework/Actor.h"

// ai generated
static int32 DeriveLevelSeed(int32 Seed, int32 Index)
{
	uint32 S = (uint32)Seed * 2654435761u + (uint32)Index * 40503u + 1u;
	return (int32)S;
}

UHexMapGeneratorComponent::UHexMapGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// ai generated
void UHexMapGeneratorComponent::ApplySeeds()
{
	for (int32 i = 0; i < NoiseLevels.Num(); i++)
		NoiseLevels[i].Seed = DeriveLevelSeed(GlobalSeed, i);
}

// ai generated
bool UHexMapGeneratorComponent::HasAnyDepthPrefab() const
{
	for (const TSubclassOf<AActor>& Prefab : DepthLevelPrefabs)
		if (*Prefab) return true;
	return false;
}

// ai generated
TArray<TArray<int32>> UHexMapGeneratorComponent::BuildDepthMap()
{
	FDepthMapGenerator Generator;
	Generator.Width = MapWidth;
	Generator.Height = MapHeight;
	Generator.LevelsCount = DepthLevelsCount;
	Generator.NoiseLevels = NoiseLevels;
	return Generator.GetQuantizeMap();
}

// ai generated
void UHexMapGeneratorComponent::Regenerate()
{
	AActor* Anchor = GetOwner();
	if (!Anchor) return;

	ApplySeeds();

	TSubclassOf<AActor> Fallback = DefaultHexAsset;
	if (!*Fallback && DepthLevelPrefabs.Num() > 0)
		Fallback = DepthLevelPrefabs[0];
	if (!*Fallback && !HasAnyDepthPrefab()) return;

	FHexFieldMetadata Meta;
	Meta.MapWidth = MapWidth;
	Meta.MapHeight = MapHeight;
	Meta.HexRadius = HexRadius;
	Meta.RenderHexRadius = HexRadius;
	Meta.HexAsset = Fallback;

	Manager.Initialize(GetWorld());
	Manager.SetMetadata(Meta);
	Manager.ClearMapStorage();

	TArray<AActor*> Attached;
	Anchor->GetAttachedActors(Attached);
	for (AActor* Child : Attached)
		if (Child) Child->Destroy();

	Manager.InitMap(Anchor, BuildDepthMap(), DepthLevelPrefabs);
}

// ai generated
void UHexMapGeneratorComponent::PaintHex(int32 Alpha, int32 Beta)
{
	if (!*BrushHexAsset) return;

	AActor* Hex = nullptr;
	if (!Manager.TryGetHex(Alpha, Beta, Hex) || !Hex) return;
	UHexChipComponent* Chip = Hex->FindComponentByClass<UHexChipComponent>();
	if (!Chip) return;

	int32 Radius = FMath::Max(1, BrushRadius);
	if (Radius <= 1)
	{
		Manager.ReplaceHexPrefab(Chip, BrushHexAsset);
		return;
	}

	for (AActor* H : Manager.GetHexesInRadius(Chip, Radius))
	{
		if (!H) continue;
		UHexChipComponent* C = H->FindComponentByClass<UHexChipComponent>();
		if (C) Manager.ReplaceHexPrefab(C, BrushHexAsset);
	}
}
