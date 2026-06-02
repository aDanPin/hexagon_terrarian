#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HexChipComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEXMAP_API UHexChipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	int32 X = 0;

	UPROPERTY(VisibleAnywhere)
	int32 Y = 0;

	UPROPERTY(VisibleAnywhere)
	int32 Alpha = 0;

	UPROPERTY(VisibleAnywhere)
	int32 Beta = 0;
};
