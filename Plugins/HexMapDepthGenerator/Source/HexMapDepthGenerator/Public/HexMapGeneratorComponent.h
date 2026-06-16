#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/SubclassOf.h"
#include "HexMapGeneratorSettings.h"
#include "HexMapManager.h"
#include "HexMapGeneratorComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEXMAPDEPTHGENERATOR_API UHexMapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHexMapGeneratorComponent();

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FHexMapGeneratorSettings Settings;

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
