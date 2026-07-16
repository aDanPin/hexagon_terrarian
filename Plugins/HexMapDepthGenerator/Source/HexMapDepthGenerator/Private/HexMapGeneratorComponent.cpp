// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#include "HexMapGeneratorComponent.h"
#include "HexMapTransforms.h"
#include "HexCellShaderVisualizer.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/LineBatchComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

UHexMapGeneratorComponent::UHexMapGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Settings.SyncDepthLevelMeshes();
}

// ai generated
void UHexMapGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();
	Regenerate();
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
	if (!Anchor) return;

	Manager.SetMetadata(FieldMetadata);
	Manager.SetSettings(Settings.GlobalSeed, Settings.GenerationLevels);

	Settings.SyncDepthLevelMeshes();
	const int32 LevelCount = Settings.DepthLevelCount;
	const TArray<TArray<int32>> DepthMap = Manager.BuildDepthMap(LevelCount);

	if (Visualization.VisualizationMode == EHexMapVisualizationMode::Shader)
	{
		InitMapShader(Anchor, DepthMap);
		return;
	}
	else if (Visualization.VisualizationMode == EHexMapVisualizationMode::Mesh)
	{
		if (!HasAnyMesh()) return;
		TArray<UStaticMesh*> Meshes;
		for (const TObjectPtr<UStaticMesh>& Mesh : Settings.DepthLevelMeshes)
			Meshes.Add(Mesh);
		InitMap(Anchor, DepthMap, Meshes);
		return;
	}

	if (!HasAnyMesh()) return;


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
void UHexMapGeneratorComponent::ClearShaderVisualizer(AActor* Anchor) const
{
	TArray<UHexCellShaderVisualizer*> Existing;
	Anchor->GetComponents(Existing);
	for (UHexCellShaderVisualizer* Comp : Existing)
	{
		if (!Comp) continue;
		Comp->Clear();
		Comp->DestroyComponent();
	}
}

// ai generated
void UHexMapGeneratorComponent::ClearDebugGrid(AActor* Anchor) const
{
	TArray<ULineBatchComponent*> Existing;
	Anchor->GetComponents(Existing);
	for (ULineBatchComponent* Comp : Existing)
		if (Comp) Comp->DestroyComponent();
	if (UWorld* World = GetWorld())
		FlushPersistentDebugLines(World);
}

// ai generated
FVector UHexMapGeneratorComponent::GetHexCellWorldLocation(const FVector& Origin, const TArray<TArray<int32>>& DepthMap, int32 Beta, int32 Alpha) const
{
	return Origin + FVector(
		-FHexMapTransforms::AlphaBettaToPz(Alpha, Beta, FieldMetadata.HexRadius),
		FHexMapTransforms::AlphaBettaToPx(Alpha, Beta, FieldMetadata.HexRadius),
		GetHeightOffset(DepthMap, Beta, Alpha));
}

void UHexMapGeneratorComponent::DrawDebugGrid(AActor* Anchor, const TArray<TArray<int32>>& DepthMap) const
{
	UWorld* World = GetWorld();
	if (!World || !Anchor || !Visualization.bDrawDebugGrid) return;

	ULineBatchComponent* LineBatch = NewObject<ULineBatchComponent>(
		Anchor, NAME_None, WITH_EDITOR ? RF_Transactional : RF_NoFlags);
	LineBatch->SetupAttachment(Anchor->GetRootComponent());
	LineBatch->RegisterComponent();
	Anchor->AddInstanceComponent(LineBatch);
#if WITH_EDITOR
	LineBatch->Modify();
#endif

	const float R = FieldMetadata.HexRadius;
	const float RotRad = FMath::DegreesToRadians(FieldMetadata.HexRotation + 30.f);
	const FVector Origin = Anchor->GetActorLocation();
	const FLinearColor GridColor(1.f, 1.f, 1.f);
	const int32 LevelsAboveInitial = FMath::Max(0, Settings.DepthLevelCount - Settings.WaterLevel + 2);
	const float GridZ = Origin.Z + LevelsAboveInitial * Settings.heightOffset;

	for (int32 Beta = 0; Beta < FieldMetadata.MapHeight; Beta++)
	{
		for (int32 Alpha = 0; Alpha < FieldMetadata.MapWidth; Alpha++)
		{
			FVector Center = GetHexCellWorldLocation(Origin, DepthMap, Beta, Alpha);
			Center.Z = GridZ;

			FVector Prev;
			for (int32 i = 0; i <= 6; i++)
			{
				const float Angle = RotRad + FMath::DegreesToRadians(60.f * (i % 6));
				const FVector V = Center + FVector(R * FMath::Cos(Angle), R * FMath::Sin(Angle), 0.f);
				if (i > 0)
				{
					LineBatch->DrawLine(Prev, V, GridColor, SDPG_World, Visualization.DebugGridLineThickness);
				}
				Prev = V;
			}
		}
	}

#if WITH_EDITOR
	LineBatch->MarkRenderStateDirty();
#endif
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
		&& DepthMap[Beta][Alpha] > Settings.WaterLevel)
	{
		return Settings.heightOffset * (DepthMap[Beta][Alpha] - Settings.WaterLevel);
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

	ClearShaderVisualizer(Anchor);
	ClearHISM(Anchor);
	ClearDebugGrid(Anchor);

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
			const FVector Location = GetHexCellWorldLocation(Origin, DepthMap, Beta, Alpha);
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

	if (Visualization.bDrawDebugGrid)
		DrawDebugGrid(Anchor, DepthMap);

#if WITH_EDITOR
	Anchor->MarkPackageDirty();
#endif
}

// ai generated
void UHexMapGeneratorComponent::InitMapShader(AActor* Anchor, const TArray<TArray<int32>>& DepthMap)
{
	if (!GetWorld() || !Anchor) return;

#if WITH_EDITOR
	Anchor->Modify();
#endif

	ClearHISM(Anchor);
	ClearShaderVisualizer(Anchor);
	ClearDebugGrid(Anchor);

	UHexCellShaderVisualizer* Viz = NewObject<UHexCellShaderVisualizer>(
		Anchor, NAME_None, WITH_EDITOR ? RF_Transactional : RF_NoFlags);
	Viz->Material = Visualization.ShaderMaterial;
	Viz->HexRadius = FieldMetadata.HexRadius;
	Viz->HexRotation = FieldMetadata.HexRotation;
	Viz->MapWidth = FieldMetadata.MapWidth;
	Viz->MapHeight = FieldMetadata.MapHeight;
	Viz->Origin = Anchor->GetActorLocation();
	Viz->RegisterComponent();
	Anchor->AddInstanceComponent(Viz);
#if WITH_EDITOR
	Viz->Modify();
#endif
	Viz->SetCells(GetHexCells());

	if (Visualization.bDrawDebugGrid)
		DrawDebugGrid(Anchor, DepthMap);

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
	const int32 LevelCount = FMath::Max(1, Settings.DepthLevelCount);
	const TArray<TArray<int32>> DepthMap = LocalManager.BuildDepthMap(LevelCount);

	const FVector Origin = Anchor->GetActorLocation();
	Result.Reserve(FieldMetadata.MapWidth * FieldMetadata.MapHeight);

	for (int32 Beta = 0; Beta < FieldMetadata.MapHeight; Beta++)
	{
		for (int32 Alpha = 0; Alpha < FieldMetadata.MapWidth; Alpha++)
		{
			const FVector Location = GetHexCellWorldLocation(Origin, DepthMap, Beta, Alpha);

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
	Settings.SyncDepthLevelMeshes();
	if (bSuppressPropertyRegen) return;
	Regenerate();
}
#endif
