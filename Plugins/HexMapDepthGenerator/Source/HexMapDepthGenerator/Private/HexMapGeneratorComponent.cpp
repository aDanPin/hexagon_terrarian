#include "HexMapGeneratorComponent.h"
#include "DepthMapGenerator.h"
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"

UHexMapGeneratorComponent::UHexMapGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHexMapGeneratorComponent::ApplySeeds()
{
	FRandomStream Rng(Settings.GlobalSeed);
	for (int32 i = 0; i < Settings.GenerationLevels.Num(); i++)
		Settings.GenerationLevels[i].Seed = static_cast<int32>(Rng.GetUnsignedInt());
}

bool UHexMapGeneratorComponent::HasAnyMesh() const
{
	for (const TObjectPtr<UStaticMesh>& Mesh : Settings.DepthLevelMeshes)
		if (Mesh) return true;
	return false;
}

// ai generated
TArray<TArray<int32>> UHexMapGeneratorComponent::BuildDepthMap()
{
	FDepthMapGenerator Generator;
	Generator.Width = FieldMetadata.MapWidth;
	Generator.Height = FieldMetadata.MapHeight;
	Generator.LevelsCount = Settings.DepthLevelMeshes.Num();
	Generator.GenerationLevels = Settings.GenerationLevels;
	return Generator.GetQuantizeMap();
}

// ai generated
void UHexMapGeneratorComponent::Regenerate()
{
	AActor* Anchor = GetOwner();
	if (!Anchor || !HasAnyMesh()) return;

#if WITH_EDITOR
	bSuppressPropertyRegen = true;
#endif
	ApplySeeds();
#if WITH_EDITOR
	bSuppressPropertyRegen = false;
#endif

	Manager.Initialize(GetWorld());
	Manager.SetMetadata(FieldMetadata);

	TArray<UStaticMesh*> Meshes;
	for (const TObjectPtr<UStaticMesh>& Mesh : Settings.DepthLevelMeshes)
		Meshes.Add(Mesh);

	Manager.InitMap(Anchor, BuildDepthMap(), Meshes);
}

#if WITH_EDITOR
// ai generated
void UHexMapGeneratorComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	Regenerate();
}
#endif
