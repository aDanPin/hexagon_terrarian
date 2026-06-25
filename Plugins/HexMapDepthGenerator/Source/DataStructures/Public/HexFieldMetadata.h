// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HexFieldMetadata.generated.h"

USTRUCT(BlueprintType)
struct DATASTRUCTURES_API FHexFieldMetadata
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	int32 MapWidth = 11;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	int32 MapHeight = 11;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float HexRadius = 1.7f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float HexRotation = 0.0f;
};
