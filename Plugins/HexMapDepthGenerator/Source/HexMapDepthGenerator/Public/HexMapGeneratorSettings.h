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
	TArray<TObjectPtr<UStaticMesh>> DepthLevelMeshes;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 InitialLineLevel = 0;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	float heightOffset = 0.0f;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 GlobalSeed = 12345;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TArray<FDepthLevelConfig> GenerationLevels;
};
