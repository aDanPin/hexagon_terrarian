// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DepthTypes.h"
#include "HexMapGeneratorSettings.generated.h"

class UStaticMesh;

USTRUCT(BlueprintType)
struct DATASTRUCTURES_API FHexMapGeneratorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Hex Map Generator", meta=(ClampMin="1"))
	int32 DepthLevelCount = 5;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMesh>> DepthLevelMeshes;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 WaterLevel = 0;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FLinearColor WaterColor = FLinearColor(0.05f, 0.15f, 0.45f);

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FLinearColor LandColor = FLinearColor(0.15f, 0.65f, 0.35f);

	UPROPERTY()
	float heightOffset = 0.0f;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 GlobalSeed = 12345;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TArray<FDepthLevelConfig> GenerationLevels;

	// ai generated
	void SyncDepthLevelMeshes()
	{
		DepthLevelCount = FMath::Max(1, DepthLevelCount);
		DepthLevelMeshes.SetNum(DepthLevelCount);
	}
};
