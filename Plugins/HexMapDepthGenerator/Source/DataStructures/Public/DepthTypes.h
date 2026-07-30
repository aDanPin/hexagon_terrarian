// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

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
	Replace,
	ReplaceIfAbove,
	ReplaceIfBelow
};

UENUM()
enum class ENoiseLayerType : uint8
{
	Perlin,
	Euclidean,
	PerlinEuclidean,
	Value
};

USTRUCT()
struct DATASTRUCTURES_API FEuclideanPoint
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
struct DATASTRUCTURES_API FDepthLevelConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	ENoiseLayerType Type = ENoiseLayerType::Perlin;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	bool bEnabled = true;

	UPROPERTY(meta=(Hidden))
	int32 Seed = 12345;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float AmplitudeLeft = 0.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float AmplitudeRight = 1.0f;
	
	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float ApplyWindowLeft = 0.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	float ApplyWindowRight = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Perlin||Type==ENoiseLayerType::PerlinEuclidean"))
	float PerlinPower = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Euclidean||Type==ENoiseLayerType::PerlinEuclidean"))
	float EuclideanPower = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	bool Invert = false;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator")
	EApplyMode ApplyMode = EApplyMode::Add;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Perlin||Type==ENoiseLayerType::PerlinEuclidean"))
	float ScaleX = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Perlin||Type==ENoiseLayerType::PerlinEuclidean"))
	float ScaleY = 1.0f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Value"))
	float Value = 0.5f;

	UPROPERTY(EditAnywhere, Category="Advanced Hex Map Depth Generator", meta=(EditCondition="Type==ENoiseLayerType::Euclidean||Type==ENoiseLayerType::PerlinEuclidean"))
	TArray<FEuclideanPoint> Points;
};
