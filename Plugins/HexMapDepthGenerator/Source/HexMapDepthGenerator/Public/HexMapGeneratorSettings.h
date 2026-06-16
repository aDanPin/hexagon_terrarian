#pragma once

#include "CoreMinimal.h"
#include "DepthTypes.h"
#include "HexMapGeneratorSettings.generated.h"

class UStaticMesh;

USTRUCT(BlueprintType)
struct HEXMAPDEPTHGENERATOR_API FHexMapGeneratorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	float HexRadius = 1.7f;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	float HexRotation = 0.0f;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TArray<TObjectPtr<UStaticMesh>> DepthLevelMeshes;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 MapWidth = 11;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 MapHeight = 11;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 GlobalSeed = 12345;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TArray<FDepthLevelConfig> GenerationLevels;
};
