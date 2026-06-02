#pragma once

#include "CoreMinimal.h"
#include "DepthTypes.generated.h"

UENUM()
enum class EApplyMode : uint8
{
	Add,
	Subtract,
	Multiply,
	Interpolate,
	Replace
};

UENUM()
enum class ENoiseLayerType : uint8
{
	Perlin,
	Euclidean,
	Value
};

USTRUCT()
struct DEPTHGENERATOR_API FEuclideanPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere)
	float X = 0.5f;

	UPROPERTY(EditAnywhere)
	float Y = 0.5f;

	UPROPERTY(EditAnywhere)
	float Radius = 1.0f;
};

USTRUCT()
struct DEPTHGENERATOR_API FDepthLevelConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ENoiseLayerType Type = ENoiseLayerType::Perlin;

	UPROPERTY(EditAnywhere)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere)
	int32 Seed = 12345;

	UPROPERTY(EditAnywhere)
	float AmplitudeLeft = 0.0f;

	UPROPERTY(EditAnywhere)
	float AmplitudeRight = 1.0f;

	UPROPERTY(EditAnywhere)
	bool bIncludeLeft = true;

	UPROPERTY(EditAnywhere)
	bool bIncludeRight = true;

	UPROPERTY(EditAnywhere)
	float Power = 1.0f;

	UPROPERTY(EditAnywhere)
	EApplyMode ApplyMode = EApplyMode::Add;

	UPROPERTY(EditAnywhere, meta=(EditCondition="Type==ENoiseLayerType::Perlin"))
	float ScaleX = 1.0f;

	UPROPERTY(EditAnywhere, meta=(EditCondition="Type==ENoiseLayerType::Perlin"))
	float ScaleY = 1.0f;

	UPROPERTY(EditAnywhere, meta=(EditCondition="Type==ENoiseLayerType::Value"))
	float Value = 0.5f;

	UPROPERTY(EditAnywhere, meta=(EditCondition="Type==ENoiseLayerType::Euclidean"))
	TArray<FEuclideanPoint> Points;
};
