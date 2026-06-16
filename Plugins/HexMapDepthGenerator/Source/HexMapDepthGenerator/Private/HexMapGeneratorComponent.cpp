#include "HexMapGeneratorComponent.h"
#include "GameFramework/Actor.h"

UHexMapGeneratorComponent::UHexMapGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UHexMapGeneratorComponent::HasAnyMesh() const
{
	for (const TObjectPtr<UStaticMesh>& Mesh : Settings.DepthLevelMeshes)
		if (Mesh) return true;
	return false;
}

// ai generated
void UHexMapGeneratorComponent::Regenerate()
{
	AActor* Anchor = GetOwner();
	if (!Anchor || !HasAnyMesh()) return;

#if WITH_EDITOR
	bSuppressPropertyRegen = true;
#endif
	FHexMapManager::ApplySeeds(Settings.GlobalSeed, Settings.GenerationLevels);
#if WITH_EDITOR
	bSuppressPropertyRegen = false;
#endif

	Manager.Initialize(GetWorld());
	Manager.SetMetadata(FieldMetadata);

	TArray<UStaticMesh*> Meshes;
	for (const TObjectPtr<UStaticMesh>& Mesh : Settings.DepthLevelMeshes)
		Meshes.Add(Mesh);

	Manager.InitMap(Anchor, Manager.BuildDepthMap(Settings.DepthLevelMeshes.Num(), Settings.GenerationLevels), Meshes);
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
