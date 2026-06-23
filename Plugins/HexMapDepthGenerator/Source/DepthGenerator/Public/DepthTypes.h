// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DepthTypes.generated.h"

UENUM()
enum class EApplyMode : uint8
{
	Add,
	Subtract,
	Multiply,
	Lerp,
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

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float X = 0.5f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float Y = 0.5f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float Radius = 1.0f;
};

USTRUCT()
struct DEPTHGENERATOR_API FDepthLevelConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	ENoiseLayerType Type = ENoiseLayerType::Perlin;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	bool bEnabled = true;

	UPROPERTY(VisibleAnywhere, Category="Advanced Hex Map Depth Generator")
	int32 Seed = 12345;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float AmplitudeLeft = 0.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float AmplitudeRight = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	bool bIncludeLeft = true;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	bool bIncludeRight = true;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float Power = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	EApplyMode ApplyMode = EApplyMode::Add;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Perlin"))
	float ScaleX = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Perlin"))
	float ScaleY = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Value"))
	float Value = 0.5f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Euclidean"))
	TArray<FEuclideanPoint> Points;
};
