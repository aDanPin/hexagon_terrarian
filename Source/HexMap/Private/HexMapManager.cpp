#include "HexMapManager.h"
#include "HexMapTransforms.h"
#include "HexChipComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

// ai generated
bool FHexMapManager::TryGetHex(int32 Alpha, int32 Beta, AActor*& OutHex) const
{
	OutHex = nullptr;
	if (HexStorage.Num() == 0 || Beta < 0 || Beta >= HexStorage.Num()) return false;
	if (Alpha < 0 || Alpha >= HexStorage[Beta].Num()) return false;
	OutHex = HexStorage[Beta][Alpha];
	return OutHex != nullptr;
}

// ai generated
AActor* FHexMapManager::SpawnHex(TSubclassOf<AActor> Prefab, const FVector& Location, AActor* Anchor, int32 Alpha, int32 Beta)
{
	if (!World || !*Prefab) return nullptr;

	AActor* Hex = World->SpawnActor<AActor>(Prefab, Location, FRotator::ZeroRotator);
	if (!Hex) return nullptr;

	if (Anchor)
		Hex->AttachToActor(Anchor, FAttachmentTransformRules::KeepWorldTransform);

	UHexChipComponent* Chip = Hex->FindComponentByClass<UHexChipComponent>();
	if (!Chip)
	{
		Chip = NewObject<UHexChipComponent>(Hex);
		Chip->RegisterComponent();
	}
	Chip->Alpha = Alpha;
	Chip->Beta = Beta;
	FIntPoint XY = FHexMapTransforms::AlphaBettaToXY(Alpha, Beta);
	Chip->X = XY.X;
	Chip->Y = XY.Y;
	return Hex;
}

// ai generated
void FHexMapManager::ReplaceHexPrefab(UHexChipComponent* Chip, TSubclassOf<AActor> Prefab)
{
	if (!Chip || !*Prefab || HexStorage.Num() == 0) return;

	int32 Alpha = Chip->Alpha;
	int32 Beta = Chip->Beta;
	if (Beta < 0 || Beta >= HexStorage.Num() || Alpha < 0 || Alpha >= HexStorage[Beta].Num()) return;

	AActor* Owner = Chip->GetOwner();
	if (!Owner) return;
	AActor* Anchor = Owner->GetAttachParentActor();
	FVector Pos = Owner->GetActorLocation();
	Owner->Destroy();

	HexStorage[Beta][Alpha] = SpawnHex(Prefab, Pos, Anchor, Alpha, Beta);
}

// ai generated
void FHexMapManager::InitMap(AActor* Anchor, const TArray<TArray<int32>>& DepthMap, const TArray<TSubclassOf<AActor>>& DepthPrefabs)
{
	if (!World || !Anchor) return;

	FVector Origin = Anchor->GetActorLocation();
	HexStorage.SetNum(Metadata.MapHeight);
	for (int32 Beta = 0; Beta < Metadata.MapHeight; Beta++)
	{
		HexStorage[Beta].SetNum(Metadata.MapWidth);
		for (int32 Alpha = 0; Alpha < Metadata.MapWidth; Alpha++)
		{
			TSubclassOf<AActor> Prefab = PickDepthPrefab(DepthMap, DepthPrefabs, Beta, Alpha, Metadata.HexAsset);
			FVector Location = Origin + FVector(
				FHexMapTransforms::AlphaBettaToPx(Alpha, Beta, Metadata.HexRadius),
				FHexMapTransforms::AlphaBettaToPz(Alpha, Beta, Metadata.HexRadius),
				0.0f);
			HexStorage[Beta][Alpha] = SpawnHex(Prefab, Location, Anchor, Alpha, Beta);
		}
	}
}

// ai generated
TSubclassOf<AActor> FHexMapManager::PickDepthPrefab(const TArray<TArray<int32>>& DepthMap, const TArray<TSubclassOf<AActor>>& DepthPrefabs, int32 Beta, int32 Alpha, TSubclassOf<AActor> Fallback)
{
	if (DepthMap.Num() == 0 || DepthPrefabs.Num() == 0) return Fallback;
	if (Beta < 0 || Beta >= DepthMap.Num()) return Fallback;
	if (Alpha < 0 || Alpha >= DepthMap[Beta].Num()) return Fallback;
	int32 Level = DepthMap[Beta][Alpha];
	int32 Idx = FMath::Clamp(Level, 1, DepthPrefabs.Num()) - 1;
	return *DepthPrefabs[Idx] ? DepthPrefabs[Idx] : Fallback;
}

// ai generated
bool FHexMapManager::IsValidCoord(int32 Nx, int32 Ny) const
{
	int32 Alpha = FHexMapTransforms::XYToAlpha(Nx, Ny);
	int32 Beta = FHexMapTransforms::XYToBeta(Nx, Ny);

	return Beta >= 0 && Beta < HexStorage.Num()
		&& Alpha >= 0 && Alpha < HexStorage[Beta].Num()
		&& HexStorage[Beta][Alpha] != nullptr
		&& HexStorage[Beta][Alpha]->FindComponentByClass<UHexChipComponent>() != nullptr;
}

// ai generated
TArray<AActor*> FHexMapManager::GetHexesNearHexes(UHexChipComponent* Center) const
{
	int32 X = Center->X;
	int32 Y = Center->Y;
	const FIntPoint Neighbors[6] = {
		FIntPoint(X + 1, Y),
		FIntPoint(X + 1, Y - 1),
		FIntPoint(X, Y - 1),
		FIntPoint(X - 1, Y),
		FIntPoint(X - 1, Y + 1),
		FIntPoint(X, Y + 1)
	};

	TArray<AActor*> Hexes;
	for (const FIntPoint& Coord : Neighbors)
	{
		if (IsValidCoord(Coord.X, Coord.Y))
		{
			Hexes.Add(HexStorage[FHexMapTransforms::XYToBeta(Coord.X, Coord.Y)]
				[FHexMapTransforms::XYToAlpha(Coord.X, Coord.Y)]);
		}
	}
	return Hexes;
}

// ai generated
TSet<AActor*> FHexMapManager::GetHexesInRadius(UHexChipComponent* Center, int32 R)
{
	TSet<AActor*> Result;
	if (!Center || HexStorage.Num() == 0 || R < 1) return Result;

	Result.Add(Center->GetOwner());
	TArray<AActor*> Near = GetHexesNearHexes(Center);
	Result.Append(Near);

	TSet<AActor*> CurrentRadius(Near);
	TSet<AActor*> NewRadius;
	for (int32 i = 2; i < R; ++i)
	{
		for (AActor* Hex : CurrentRadius)
		{
			UHexChipComponent* Chip = Hex ? Hex->FindComponentByClass<UHexChipComponent>() : nullptr;
			if (!Chip) continue;
			for (AActor* HexNear : GetHexesNearHexes(Chip))
			{
				if (!Result.Contains(HexNear))
					NewRadius.Add(HexNear);
			}
		}

		for (AActor* Hex : NewRadius)
			Result.Add(Hex);

		CurrentRadius = NewRadius;
		NewRadius.Empty();
	}
	return Result;
}
