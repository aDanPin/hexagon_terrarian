#pragma once

#include "CoreMinimal.h"
#include "HexFieldMetadata.generated.h"

USTRUCT(BlueprintType)
struct FHexFieldMetadata
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 MapWidth = 64;

	UPROPERTY(EditAnywhere)
	int32 MapHeight = 64;

	UPROPERTY(EditAnywhere)
	float HexRadius = 1.7f;

	UPROPERTY(EditAnywhere)
	float HexRotation = 0.0f;
};
