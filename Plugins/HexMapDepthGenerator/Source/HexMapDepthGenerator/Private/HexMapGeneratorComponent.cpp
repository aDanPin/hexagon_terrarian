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

// ai generated
void UHexMapGeneratorComponent::SavePreset()
{
	if (!Preset) return;
#if WITH_EDITOR
	Preset->Modify();
#endif
	Preset->FieldMetadata = FieldMetadata;
	Preset->Settings = Settings;
#if WITH_EDITOR
	Preset->MarkPackageDirty();
#endif
}

// ai generated
void UHexMapGeneratorComponent::LoadPreset()
{
	if (!Preset) return;
#if WITH_EDITOR
	bSuppressPropertyRegen = true;
#endif
	FieldMetadata = Preset->FieldMetadata;
	Settings = Preset->Settings;
#if WITH_EDITOR
	bSuppressPropertyRegen = false;
	Regenerate();
#endif
}

#if WITH_EDITOR
// ai generated
void UHexMapGeneratorComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	if (bSuppressPropertyRegen) return;
	Regenerate();
}
#endif
