#include "HexMapGeneratorComponent.h"
#include "DepthMapGenerator.h"
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
bool UHexMapGeneratorComponent::HasAnyMesh() const
{
	for (const TObjectPtr<UStaticMesh>& Mesh : DepthLevelMeshes)
		if (Mesh) return true;
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
	if (!Anchor || !HasAnyMesh()) return;

	ApplySeeds();

	FHexFieldMetadata Meta;
	Meta.MapWidth = MapWidth;
	Meta.MapHeight = MapHeight;
	Meta.HexRadius = HexRadius;

	Manager.Initialize(GetWorld());
	Manager.SetMetadata(Meta);

	TArray<UStaticMesh*> Meshes;
	for (const TObjectPtr<UStaticMesh>& Mesh : DepthLevelMeshes)
		Meshes.Add(Mesh);

	Manager.InitMap(Anchor, BuildDepthMap(), Meshes);
}
