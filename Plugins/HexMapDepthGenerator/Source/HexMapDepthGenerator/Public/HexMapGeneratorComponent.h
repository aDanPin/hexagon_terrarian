#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/SubclassOf.h"
#include "HexFieldMetadata.h"
#include "HexMapGeneratorSettings.h"
#include "HexMapManager.h"
#include "HexMapGeneratorPreset.h"
#include "HexMapGeneratorComponent.generated.h"

USTRUCT(BlueprintType)
struct FHexCellInfo
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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEXMAPDEPTHGENERATOR_API UHexMapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHexMapGeneratorComponent();

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FHexFieldMetadata FieldMetadata;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	FHexMapGeneratorSettings Settings;

	UPROPERTY(EditAnywhere, Category="Hex Map Generator")
	TObjectPtr<UHexMapGeneratorPreset> Preset;

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void Regenerate();

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void SavePreset();

	UFUNCTION(CallInEditor, Category="Hex Map Generator")
	void LoadPreset();

	UFUNCTION(BlueprintCallable, Category="Hex Map Generator")
	TArray<FHexCellInfo> GetHexCells() const;


#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	bool bSuppressPropertyRegen = false;
#endif

private:
	FHexMapManager Manager;

	bool HasAnyMesh() const;
	void InitMap(AActor* Anchor, const TArray<TArray<int32>>& DepthMap, const TArray<UStaticMesh*>& DepthMeshes);
	void ClearHISM(AActor* Anchor) const;
	static int32 PickMeshIndex(const TArray<TArray<int32>>& DepthMap, int32 MeshCount, int32 Beta, int32 Alpha);
};
