// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HexCellInfo.h"
#include "HexFieldMetadata.h"
#include "HexMapGeneratorSettings.h"
#include "HexMapVisualizationSettings.h"
#include "HexMapManager.h"
#include "HexMapGeneratorPreset.h"
#include "HexMapGeneratorComponent.generated.h"

class UHexCellShaderVisualizer;
class UStaticMesh;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEXMAPDEPTHGENERATOR_API UHexMapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHexMapGeneratorComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TObjectPtr<UHexMapGeneratorPreset> Preset;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FHexFieldMetadata FieldMetadata;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FHexMapGeneratorSettings Settings;

	UPROPERTY()
	FHexMapVisualizationSettings Visualization;

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void Regenerate();

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void SavePreset();

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void LoadPreset();

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void NativeValueGenerationTest();

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void ParallelValueGenerationTest();

	UFUNCTION(BlueprintCallable, Category="Hex Map Generator")
	TArray<FHexCellInfo> GetHexCells() const;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	bool bSuppressPropertyRegen = false;
#endif

private:
	FHexMapManager Manager;

	bool HasAnyMesh() const;
	void InitMap(AActor* Anchor, const TArray<int32>& DepthMap, const TArray<UStaticMesh*>& DepthMeshes);
	void InitMapShader(AActor* Anchor, const TArray<int32>& DepthMap);
	void ClearHISM(AActor* Anchor) const;
	void ClearShaderVisualizer(AActor* Anchor) const;
	void ClearDebugGrid(AActor* Anchor) const;
	void DrawDebugGrid(AActor* Anchor, const TArray<int32>& DepthMap) const;
	FVector GetHexCellWorldLocation(const FVector& Origin, const TArray<int32>& DepthMap, int32 Beta, int32 Alpha) const;
	static int32 PickMeshIndex(const TArray<int32>& DepthMap, int32 MapWidth, int32 MeshCount, int32 Beta, int32 Alpha);
	float GetHeightOffset(const TArray<int32>& DepthMap, int32 Beta, int32 Alpha) const;
};
