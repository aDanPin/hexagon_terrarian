// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HexFieldMetadata.h"
#include "HexMapGeneratorSettings.h"
#include "HexMapGeneratorPreset.generated.h"

UCLASS(BlueprintType)
class HEXMAPDEPTHGENERATOR_API UHexMapGeneratorPreset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FHexFieldMetadata FieldMetadata;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FHexMapGeneratorSettings Settings;
};
