// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

#include "HexMapGeneratorComponent.h"
#include "HexMapTransforms.h"
#include "HexCellShaderVisualizer.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/LineBatchComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "HAL/PlatformMisc.h"
#include "Async/TaskGraphInterfaces.h"

UHexMapGeneratorComponent::UHexMapGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Settings.SyncDepthLevelMeshes();
}
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
void UHexMapGeneratorComponent::Regenerate()
{
	AActor* Anchor = GetOwner();
	if (!Anchor) return;

	Manager.SetMetadata(FieldMetadata);
	Manager.SetSettings(Settings.GlobalSeed, Settings.GenerationLevels);

	Settings.SyncDepthLevelMeshes();
	const int32 LevelCount = Settings.DepthLevelCount;
	const TArray<int32> DepthMap = Manager.BuildDepthMap(LevelCount);

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
void UHexMapGeneratorComponent::NativeValueGenerationTest()
{
	FHexMapManager LocalManager;
	LocalManager.SetMetadata(FieldMetadata);
	LocalManager.SetSettings(0, Settings.GenerationLevels);
	const int32 LevelCount = FMath::Max(1, Settings.DepthLevelCount);

	const double Start = FPlatformTime::Seconds();
	for (int32 i = 0; i < 10000; i++)
	{
		LocalManager.SetSettings(i, Settings.GenerationLevels);
		LocalManager.BuildDepthMap(LevelCount);
	}
	const double End = FPlatformTime::Seconds();

	UE_LOG(LogTemp, Display, TEXT("Native Value Generation Test: %.3f ms for 10.000 iterations"), (End - Start) * 1000.0);
}
void UHexMapGeneratorComponent::ParallelValueGenerationTest()
{
	int32 NumCores = FPlatformMisc::NumberOfCores();
	int32 NumCoresIncludingHyperthreads = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
	UE_LOG(LogTemp, Display, TEXT("Number of cores: %d, Number of cores including hyperthreads: %d"), NumCores, NumCoresIncludingHyperthreads);

	int32 NumWorkers = FTaskGraphInterface::Get().GetNumWorkerThreads();
	UE_LOG(LogTemp, Display, TEXT("Number of workers: %d"), NumWorkers);

	FHexMapManager LocalManager;
	LocalManager.SetMetadata(FieldMetadata);
	LocalManager.SetSettings(0, Settings.GenerationLevels);
	const int32 LevelCount = FMath::Max(1, Settings.DepthLevelCount);

	const double Start = FPlatformTime::Seconds();
	for (int32 i = 0; i < 10000; i++)
	{
		LocalManager.SetSettings(i, Settings.GenerationLevels);
		LocalManager.BuildDepthMapParallel(LevelCount);
	}
	const double End = FPlatformTime::Seconds();

	UE_LOG(LogTemp, Display, TEXT("Parallel Value Generation Test: %.3f ms for 10.000 iterations"), (End - Start) * 1000.0);
}
void UHexMapGeneratorComponent::ClearHISM(AActor* Anchor) const
{
	TArray<UHierarchicalInstancedStaticMeshComponent*> Existing;
	Anchor->GetComponents(Existing);
	for (UHierarchicalInstancedStaticMeshComponent* Comp : Existing)
		if (Comp) Comp->DestroyComponent();
}
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
void UHexMapGeneratorComponent::ClearDebugGrid(AActor* Anchor) const
{
	TArray<ULineBatchComponent*> Existing;
	Anchor->GetComponents(Existing);
	for (ULineBatchComponent* Comp : Existing)
		if (Comp) Comp->DestroyComponent();
	if (UWorld* World = GetWorld())
		FlushPersistentDebugLines(World);
}
FVector UHexMapGeneratorComponent::GetHexCellWorldLocation(const FVector& Origin, const TArray<int32>& DepthMap, int32 Beta, int32 Alpha) const
{
	return Origin + FVector(
		-FHexMapTransforms::AlphaBettaToPz(Alpha, Beta, FieldMetadata.HexRadius),
		FHexMapTransforms::AlphaBettaToPx(Alpha, Beta, FieldMetadata.HexRadius),
		GetHeightOffset(DepthMap, Beta, Alpha));
}

void UHexMapGeneratorComponent::DrawDebugGrid(AActor* Anchor, const TArray<int32>& DepthMap) const
{
	UWorld* World = GetWorld();
	if (!World || !Anchor || !Visualization.bDrawDebugGrid) return;

	ULineBatchComponent* LineBatch = NewObject<ULineBatchComponent>(Anchor, NAME_None, RF_Transient);
	LineBatch->SetupAttachment(Anchor->GetRootComponent());
	LineBatch->RegisterComponent();

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
int32 UHexMapGeneratorComponent::PickMeshIndex(const TArray<int32>& DepthMap, int32 MapWidth, int32 MeshCount, int32 Beta, int32 Alpha)
{
	const int32 Idx = Beta * MapWidth + Alpha;
	if (!DepthMap.IsValidIndex(Idx)) return 0;
	return FMath::Clamp(DepthMap[Idx], 1, MeshCount) - 1;
}
float UHexMapGeneratorComponent::GetHeightOffset(const TArray<int32>& DepthMap, int32 Beta, int32 Alpha) const
{
	const int32 Idx = Beta * FieldMetadata.MapWidth + Alpha;
	if (DepthMap.IsValidIndex(Idx) && DepthMap[Idx] > Settings.WaterLevel)
		return Settings.heightOffset * (DepthMap[Idx] - Settings.WaterLevel);
	return 0.f;
}
void UHexMapGeneratorComponent::InitMap(AActor* Anchor, const TArray<int32>& DepthMap, const TArray<UStaticMesh*>& DepthMeshes)
{
	if (!GetWorld() || !Anchor || DepthMeshes.Num() == 0) return;

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
			Anchor, NAME_None, RF_Transient);
		HISM->SetStaticMesh(DepthMeshes[i]);
		HISM->SetupAttachment(Anchor->GetRootComponent());
		HISM->RegisterComponent();
		Components[i] = HISM;
	}

	FVector Origin = Anchor->GetActorLocation();
	for (int32 Beta = 0; Beta < FieldMetadata.MapHeight; Beta++)
	{
		for (int32 Alpha = 0; Alpha < FieldMetadata.MapWidth; Alpha++)
		{
			int32 Idx = PickMeshIndex(DepthMap, FieldMetadata.MapWidth, DepthMeshes.Num(), Beta, Alpha);
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
}
void UHexMapGeneratorComponent::InitMapShader(AActor* Anchor, const TArray<int32>& DepthMap)
{
	if (!GetWorld() || !Anchor) return;

	ClearHISM(Anchor);
	ClearShaderVisualizer(Anchor);
	ClearDebugGrid(Anchor);

	UHexCellShaderVisualizer* Viz = NewObject<UHexCellShaderVisualizer>(Anchor, NAME_None, RF_Transient);
	Viz->Material = Visualization.ShaderMaterial;
	Viz->HexRadius = FieldMetadata.HexRadius;
	Viz->HexRotation = FieldMetadata.HexRotation;
	Viz->WaterLevel = Settings.WaterLevel;
	Viz->DepthLevelCount = Settings.DepthLevelCount;
	Viz->WaterColor = Settings.WaterColor;
	Viz->LandColor = Settings.LandColor;
	Viz->LightnessMin = Settings.LightnessMin;
	Viz->LightnessMax = Settings.LightnessMax;
	Viz->MapWidth = FieldMetadata.MapWidth;
	Viz->MapHeight = FieldMetadata.MapHeight;
	Viz->Origin = Anchor->GetActorLocation();
	Viz->RegisterComponent();
	Viz->SetCells(GetHexCells());

	if (Visualization.bDrawDebugGrid)
		DrawDebugGrid(Anchor, DepthMap);
}
TArray<FHexCellInfo> UHexMapGeneratorComponent::GetHexCells() const
{
	TArray<FHexCellInfo> Result;
	const AActor* Anchor = GetOwner();
	if (!Anchor) return Result;

	FHexMapManager LocalManager;
	LocalManager.SetMetadata(FieldMetadata);
	LocalManager.SetSettings(Settings.GlobalSeed, Settings.GenerationLevels);
	const int32 LevelCount = FMath::Max(1, Settings.DepthLevelCount);
	const TArray<int32> DepthMap = LocalManager.BuildDepthMap(LevelCount);

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
			const int32 Idx = Beta * FieldMetadata.MapWidth + Alpha;
			Cell.DepthLevel = DepthMap.IsValidIndex(Idx) ? DepthMap[Idx] : 0;
			Result.Add(Cell);
		}
	}

	return Result;
}
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
void UHexMapGeneratorComponent::OnRegister()
{
	Super::OnRegister();

	UWorld* World = GetWorld();
	if (!World || World->IsGameWorld()) return;

	World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		Regenerate();
	}));
}
void UHexMapGeneratorComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	Settings.SyncDepthLevelMeshes();
	if (bSuppressPropertyRegen) return;
	Regenerate();
}
#endif
