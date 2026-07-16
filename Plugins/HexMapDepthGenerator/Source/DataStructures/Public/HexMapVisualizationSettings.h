// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HexMapVisualizationSettings.generated.h"

class UMaterialInterface;

UENUM(BlueprintType)
enum class EHexMapVisualizationMode : uint8
{
	Mesh UMETA(DisplayName = "Instanced Meshes"),
	Shader UMETA(DisplayName = "Shader")
};

USTRUCT(BlueprintType)
struct DATASTRUCTURES_API FHexMapVisualizationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	EHexMapVisualizationMode VisualizationMode = EHexMapVisualizationMode::Shader;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator", meta=(EditCondition="VisualizationMode==EHexMapVisualizationMode::Shader", DisplayName="Shader Material (optional)"))
	TObjectPtr<UMaterialInterface> ShaderMaterial;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	bool bDrawDebugGrid = false;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator", meta=(ClampMin="0.1"))
	float DebugGridLineThickness = 1.f;
};
