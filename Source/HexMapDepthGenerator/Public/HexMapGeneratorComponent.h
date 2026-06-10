#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/SubclassOf.h"
#include "DepthTypes.h"
#include "HexMapManager.h"
#include "HexMapGeneratorComponent.generated.h"

class UStaticMesh;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEXMAPDEPTHGENERATOR_API UHexMapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHexMapGeneratorComponent();

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

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void Regenerate();

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	bool bSuppressPropertyRegen = false;
#endif

private:
	FHexMapManager Manager;

	void ApplySeeds();
	bool HasAnyMesh() const;
	TArray<TArray<int32>> BuildDepthMap();
};
