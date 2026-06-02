#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/SubclassOf.h"
#include "DepthTypes.h"
#include "HexMapManager.h"
#include "HexMapGeneratorComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEXMAPDEPTHGENERATOR_API UHexMapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHexMapGeneratorComponent();

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	float HexRadius = 1.7f;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 MapWidth = 11;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 MapHeight = 11;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TSubclassOf<AActor> DefaultHexAsset;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 DepthLevelsCount = 5;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 GlobalSeed = 12345;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TArray<FDepthLevelConfig> NoiseLevels;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TArray<TSubclassOf<AActor>> DepthLevelPrefabs;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TSubclassOf<AActor> BrushHexAsset;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	int32 BrushRadius = 5;

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void Regenerate();

	UFUNCTION(BlueprintCallable, Category="Hex Map Generator")
	void PaintHex(int32 Alpha, int32 Beta);

private:
	FHexMapManager Manager;

	void ApplySeeds();
	bool HasAnyDepthPrefab() const;
	TArray<TArray<int32>> BuildDepthMap();
};
