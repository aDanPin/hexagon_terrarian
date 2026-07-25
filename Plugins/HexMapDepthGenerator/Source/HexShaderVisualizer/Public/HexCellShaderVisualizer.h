// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HexCellInfo.h"
#include "HexCellShaderVisualizer.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEXSHADERVISUALIZER_API UHexCellShaderVisualizer : public UActorComponent
{
	GENERATED_BODY()

public:
	UHexCellShaderVisualizer();

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName CellsTextureParam = TEXT("HexCells");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexRadiusParam = TEXT("HexRadius");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexRotationParam = TEXT("HexRotation");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexWaterLevelParam = TEXT("HexWaterLevel");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexMaxDepthParam = TEXT("HexMaxDepth");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexWaterColorParam = TEXT("HexWaterColor");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexLandColorParam = TEXT("HexLandColor");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexMapWidthParam = TEXT("HexMapWidth");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexMapHeightParam = TEXT("HexMapHeight");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FName HexOriginParam = TEXT("HexOrigin");

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	float HexRadius = 1.7f;

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	float HexRotation = 0.f;

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	int32 WaterLevel = 0;

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FLinearColor WaterColor = FLinearColor(0.05f, 0.15f, 0.45f);

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FLinearColor LandColor = FLinearColor(0.15f, 0.65f, 0.35f);

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	int32 MapWidth = 1;

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	int32 MapHeight = 1;

	UPROPERTY(EditAnywhere, Category="Hex Shader")
	FVector Origin = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, Category="Hex Shader")
	void SetCells(const TArray<FHexCellInfo>& Cells);

	UFUNCTION(BlueprintCallable, Category="Hex Shader")
	void Clear();

	UFUNCTION(BlueprintCallable, Category="Hex Shader")
	UTexture2D* GetCellsTexture() const { return CellsTexture; }

	UFUNCTION(BlueprintCallable, Category="Hex Shader")
	UMaterialInstanceDynamic* GetMaterialInstance() const { return MaterialInstance; }

private:
	UPROPERTY()
	TObjectPtr<UTexture2D> CellsTexture;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> DisplayMesh;

	void EnsureMaterial();
	void EnsureDisplayMesh(const TArray<FHexCellInfo>& Cells);
	void UploadCells(const TArray<FHexCellInfo>& Cells);
	static UMaterialInterface* GetOrCreateDefaultMaterial();
};
