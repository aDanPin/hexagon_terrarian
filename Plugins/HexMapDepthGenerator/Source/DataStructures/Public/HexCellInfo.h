// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HexCellInfo.generated.h"

USTRUCT(BlueprintType)
struct DATASTRUCTURES_API FHexCellInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Hex")
	float X = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Hex")
	float Y = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Hex")
	float Z = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Hex")
	int32 DepthLevel = 0;
};
