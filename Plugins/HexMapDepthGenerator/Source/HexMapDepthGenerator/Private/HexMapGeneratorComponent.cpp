// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#include "HexMapGeneratorComponent.h"
#include "HexMapTransforms.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

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

	TArray<UStaticMesh*> Meshes;
	for (const TObjectPtr<UStaticMesh>& Mesh : Settings.DepthLevelMeshes)
		Meshes.Add(Mesh);

	Manager.SetMetadata(FieldMetadata);
	Manager.SetSettings(Settings.GlobalSeed, Settings.GenerationLevels);
	InitMap(Anchor, Manager.BuildDepthMap(Settings.DepthLevelMeshes.Num()), Meshes);
}

// ai generated
void UHexMapGeneratorComponent::ClearHISM(AActor* Anchor) const
{
	TArray<UHierarchicalInstancedStaticMeshComponent*> Existing;
	Anchor->GetComponents(Existing);
	for (UHierarchicalInstancedStaticMeshComponent* Comp : Existing)
		if (Comp) Comp->DestroyComponent();
}

// ai generated
int32 UHexMapGeneratorComponent::PickMeshIndex(const TArray<TArray<int32>>& DepthMap, int32 MeshCount, int32 Beta, int32 Alpha)
{
	if (DepthMap.Num() == 0 || Beta < 0 || Beta >= DepthMap.Num()) return 0;
	if (Alpha < 0 || Alpha >= DepthMap[Beta].Num()) return 0;
	int32 Level = DepthMap[Beta][Alpha];
	return FMath::Clamp(Level, 1, MeshCount) - 1;
}

// ai generated
float UHexMapGeneratorComponent::GetHeightOffset(const TArray<TArray<int32>>& DepthMap, int32 Beta, int32 Alpha) const
{
	if (DepthMap.IsValidIndex(Beta) && DepthMap[Beta].IsValidIndex(Alpha)
		&& DepthMap[Beta][Alpha] > Settings.InitialLineLevel)
	{
		return Settings.heightOffset * (DepthMap[Beta][Alpha] - Settings.InitialLineLevel);
	}
	return 0.f;
}

// ai generated
void UHexMapGeneratorComponent::InitMap(AActor* Anchor, const TArray<TArray<int32>>& DepthMap, const TArray<UStaticMesh*>& DepthMeshes)
{
	if (!GetWorld() || !Anchor || DepthMeshes.Num() == 0) return;

#if WITH_EDITOR
	Anchor->Modify();
#endif

	ClearHISM(Anchor);

	TArray<UHierarchicalInstancedStaticMeshComponent*> Components;
	TArray<TArray<FTransform>> Batches;
	Components.SetNum(DepthMeshes.Num());
	Batches.SetNum(DepthMeshes.Num());

	for (int32 i = 0; i < DepthMeshes.Num(); i++)
	{
		if (!DepthMeshes[i]) continue;
		UHierarchicalInstancedStaticMeshComponent* HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(
			Anchor, NAME_None, WITH_EDITOR ? RF_Transactional : RF_NoFlags);
		HISM->SetStaticMesh(DepthMeshes[i]);
		HISM->SetupAttachment(Anchor->GetRootComponent());
		HISM->RegisterComponent();
		Anchor->AddInstanceComponent(HISM);
#if WITH_EDITOR
		HISM->Modify();
#endif
		Components[i] = HISM;
	}

	FVector Origin = Anchor->GetActorLocation();
	for (int32 Beta = 0; Beta < FieldMetadata.MapHeight; Beta++)
	{
		for (int32 Alpha = 0; Alpha < FieldMetadata.MapWidth; Alpha++)
		{
			int32 Idx = PickMeshIndex(DepthMap, DepthMeshes.Num(), Beta, Alpha);
			if (!Components.IsValidIndex(Idx) || !Components[Idx]) continue;
			FVector Location = Origin + FVector(
				-FHexMapTransforms::AlphaBettaToPz(Alpha, Beta, FieldMetadata.HexRadius),
				FHexMapTransforms::AlphaBettaToPx(Alpha, Beta, FieldMetadata.HexRadius),
				GetHeightOffset(DepthMap, Beta, Alpha));
			Batches[Idx].Add(FTransform(FRotator(0.0f, FieldMetadata.HexRotation, 0.0f), Location));
		}
	}

	for (int32 i = 0; i < Components.Num(); i++)
	{
		if (!Components[i] || Batches[i].Num() == 0) continue;
		Components[i]->AddInstances(Batches[i], false, true);
#if WITH_EDITOR
		Components[i]->MarkRenderStateDirty();
#endif
	}

#if WITH_EDITOR
	Anchor->MarkPackageDirty();
#endif
}

// ai generated
TArray<FHexCellInfo> UHexMapGeneratorComponent::GetHexCells() const
{
	TArray<FHexCellInfo> Result;
	const AActor* Anchor = GetOwner();
	if (!Anchor) return Result;

	FHexMapManager LocalManager;
	LocalManager.SetMetadata(FieldMetadata);
	LocalManager.SetSettings(Settings.GlobalSeed, Settings.GenerationLevels);
	const TArray<TArray<int32>> DepthMap = LocalManager.BuildDepthMap(Settings.DepthLevelMeshes.Num());

	const FVector Origin = Anchor->GetActorLocation();
	Result.Reserve(FieldMetadata.MapWidth * FieldMetadata.MapHeight);

	for (int32 Beta = 0; Beta < FieldMetadata.MapHeight; Beta++)
	{
		for (int32 Alpha = 0; Alpha < FieldMetadata.MapWidth; Alpha++)
		{
			const FVector Location = Origin + FVector(
				FHexMapTransforms::AlphaBettaToPz(Alpha, Beta, FieldMetadata.HexRadius),
				-FHexMapTransforms::AlphaBettaToPx(Alpha, Beta, FieldMetadata.HexRadius),
				GetHeightOffset(DepthMap, Beta, Alpha));

			FHexCellInfo Cell;
			Cell.X = Location.X;
			Cell.Y = Location.Y;
			Cell.Z = Location.Z;
			Cell.DepthLevel = (DepthMap.IsValidIndex(Beta) && DepthMap[Beta].IsValidIndex(Alpha))
				? DepthMap[Beta][Alpha]
				: 0;
			Result.Add(Cell);
		}
	}

	return Result;
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
