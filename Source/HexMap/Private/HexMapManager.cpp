#include "HexMapManager.h"
#include "HexMapTransforms.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// ai generated
void FHexMapManager::ClearHISM(AActor* Anchor) const
{
	TArray<UHierarchicalInstancedStaticMeshComponent*> Existing;
	Anchor->GetComponents(Existing);
	for (UHierarchicalInstancedStaticMeshComponent* Comp : Existing)
		if (Comp) Comp->DestroyComponent();
}

// ai generated
int32 FHexMapManager::PickMeshIndex(const TArray<TArray<int32>>& DepthMap, int32 MeshCount, int32 Beta, int32 Alpha)
{
	if (DepthMap.Num() == 0 || Beta < 0 || Beta >= DepthMap.Num()) return 0;
	if (Alpha < 0 || Alpha >= DepthMap[Beta].Num()) return 0;
	int32 Level = DepthMap[Beta][Alpha];
	return FMath::Clamp(Level, 1, MeshCount) - 1;
}

// ai generated
void FHexMapManager::InitMap(AActor* Anchor, const TArray<TArray<int32>>& DepthMap, const TArray<UStaticMesh*>& DepthMeshes)
{
	if (!World || !Anchor || DepthMeshes.Num() == 0) return;

	ClearHISM(Anchor);

	TArray<UHierarchicalInstancedStaticMeshComponent*> Components;
	TArray<TArray<FTransform>> Batches;
	Components.SetNum(DepthMeshes.Num());
	Batches.SetNum(DepthMeshes.Num());

	for (int32 i = 0; i < DepthMeshes.Num(); i++)
	{
		if (!DepthMeshes[i]) continue;
		UHierarchicalInstancedStaticMeshComponent* HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(Anchor);
		HISM->SetStaticMesh(DepthMeshes[i]);
		HISM->SetupAttachment(Anchor->GetRootComponent());
		HISM->RegisterComponent();
		Anchor->AddInstanceComponent(HISM);
		Components[i] = HISM;
	}

	FVector Origin = Anchor->GetActorLocation();
	for (int32 Beta = 0; Beta < Metadata.MapHeight; Beta++)
	{
		for (int32 Alpha = 0; Alpha < Metadata.MapWidth; Alpha++)
		{
			int32 Idx = PickMeshIndex(DepthMap, DepthMeshes.Num(), Beta, Alpha);
			if (!Components.IsValidIndex(Idx) || !Components[Idx]) continue;
			FVector Location = Origin + FVector(
				FHexMapTransforms::AlphaBettaToPx(Alpha, Beta, Metadata.HexRadius),
				FHexMapTransforms::AlphaBettaToPz(Alpha, Beta, Metadata.HexRadius),
				0.0f);
			Batches[Idx].Add(FTransform(Location));
		}
	}

	for (int32 i = 0; i < Components.Num(); i++)
		if (Components[i] && Batches[i].Num() > 0)
			Components[i]->AddInstances(Batches[i], false, true);
}
