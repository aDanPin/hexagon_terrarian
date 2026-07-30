// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

#include "HexCellShaderVisualizer.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionTextureObjectParameter.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionWorldPosition.h"

#if WITH_EDITOR
#include "MaterialEditingLibrary.h"
#endif

namespace HexCellViz
{
	static TWeakObjectPtr<UMaterial> GDefaultMaterial;
	static const TCHAR* CustomCode = TEXT(
		"float4 Result = HexCellViz_Shade(WorldPos, HexCells, HexCellsSampler, HexRadius, HexRotation, HexWaterLevel, HexMaxDepth, HexWaterColor.rgb, HexLandColor.rgb, HexLightnessMin, HexLightnessMax, HexMapWidth, HexMapHeight, HexOrigin);\n"
		"Mask = Result.a;\n"
		"return Result.rgb;\n"
	);
}

UHexCellShaderVisualizer::UHexCellShaderVisualizer()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UHexCellShaderVisualizer::SetCells(const TArray<FHexCellInfo>& Cells)
{
	UploadCells(Cells);
	EnsureMaterial();
	EnsureDisplayMesh(Cells);

	if (MaterialInstance)
	{
		MaterialInstance->SetTextureParameterValue(CellsTextureParam, CellsTexture);
		MaterialInstance->SetScalarParameterValue(HexRadiusParam, HexRadius);
		MaterialInstance->SetScalarParameterValue(HexRotationParam, HexRotation);
		MaterialInstance->SetScalarParameterValue(HexWaterLevelParam, static_cast<float>(WaterLevel));
		MaterialInstance->SetScalarParameterValue(HexMaxDepthParam, static_cast<float>(FMath::Max(1, DepthLevelCount)));
		MaterialInstance->SetVectorParameterValue(HexWaterColorParam, WaterColor);
		MaterialInstance->SetVectorParameterValue(HexLandColorParam, LandColor);
		MaterialInstance->SetScalarParameterValue(HexLightnessMinParam, LightnessMin);
		MaterialInstance->SetScalarParameterValue(HexLightnessMaxParam, LightnessMax);
		MaterialInstance->SetScalarParameterValue(HexMapWidthParam, static_cast<float>(MapWidth));
		MaterialInstance->SetScalarParameterValue(HexMapHeightParam, static_cast<float>(MapHeight));
		MaterialInstance->SetVectorParameterValue(HexOriginParam, FLinearColor(Origin.X, Origin.Y, Origin.Z, 0.f));
	}
	if (DisplayMesh && MaterialInstance)
		DisplayMesh->SetMaterial(0, MaterialInstance);
}
void UHexCellShaderVisualizer::Clear()
{
	if (DisplayMesh)
	{
		DisplayMesh->DestroyComponent();
		DisplayMesh = nullptr;
	}
	CellsTexture = nullptr;
	MaterialInstance = nullptr;
}
void UHexCellShaderVisualizer::EnsureMaterial()
{
	if (!Material)
		Material = GetOrCreateDefaultMaterial();
	if (!Material) return;
	if (!MaterialInstance || MaterialInstance->Parent != Material)
		MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
}
UMaterialInterface* UHexCellShaderVisualizer::GetOrCreateDefaultMaterial()
{
#if WITH_EDITOR
	if (UMaterial* Existing = HexCellViz::GDefaultMaterial.Get())
		return Existing;

	UMaterial* Mat = NewObject<UMaterial>(GetTransientPackage(), TEXT("M_HexCellVisualizer_v5"), RF_Transient | RF_Public);
	Mat->MaterialDomain = MD_Surface;
	Mat->BlendMode = BLEND_Masked;
	Mat->TwoSided = true;
	Mat->SetShadingModel(MSM_Unlit);

	auto* TexParam = Cast<UMaterialExpressionTextureObjectParameter>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionTextureObjectParameter::StaticClass(), -600, -200));
	TexParam->ParameterName = TEXT("HexCells");

	auto MakeScalar = [Mat](FName Name, float Default, int32 Y) -> UMaterialExpressionScalarParameter*
	{
		auto* P = Cast<UMaterialExpressionScalarParameter>(
			UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionScalarParameter::StaticClass(), -600, Y));
		P->ParameterName = Name;
		P->DefaultValue = Default;
		return P;
	};

	UMaterialExpressionScalarParameter* RadiusParam = MakeScalar(TEXT("HexRadius"), 1.7f, -40);
	UMaterialExpressionScalarParameter* RotParam = MakeScalar(TEXT("HexRotation"), 0.f, 40);
	UMaterialExpressionScalarParameter* WaterLevelParam = MakeScalar(TEXT("HexWaterLevel"), 0.f, 120);
	UMaterialExpressionScalarParameter* MaxDepthParam = MakeScalar(TEXT("HexMaxDepth"), 1.f, 160);

	auto MakeColor = [Mat](FName Name, FLinearColor Default, int32 Y) -> UMaterialExpressionVectorParameter*
	{
		auto* P = Cast<UMaterialExpressionVectorParameter>(
			UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionVectorParameter::StaticClass(), -600, Y));
		P->ParameterName = Name;
		P->DefaultValue = Default;
		return P;
	};

	UMaterialExpressionVectorParameter* WaterColorParam = MakeColor(TEXT("HexWaterColor"), FLinearColor(0.05f, 0.15f, 0.45f), 240);
	UMaterialExpressionVectorParameter* LandColorParam = MakeColor(TEXT("HexLandColor"), FLinearColor(0.15f, 0.65f, 0.35f), 320);
	UMaterialExpressionScalarParameter* LightnessMinParam = MakeScalar(TEXT("HexLightnessMin"), 0.35f, 360);
	UMaterialExpressionScalarParameter* LightnessMaxParam = MakeScalar(TEXT("HexLightnessMax"), 1.0f, 400);
	UMaterialExpressionScalarParameter* MapWParam = MakeScalar(TEXT("HexMapWidth"), 1.f, 440);
	UMaterialExpressionScalarParameter* MapHParam = MakeScalar(TEXT("HexMapHeight"), 1.f, 520);

	auto* OriginParam = Cast<UMaterialExpressionVectorParameter>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionVectorParameter::StaticClass(), -600, 600));
	OriginParam->ParameterName = TEXT("HexOrigin");

	auto* WorldPos = Cast<UMaterialExpressionWorldPosition>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionWorldPosition::StaticClass(), -600, 680));

	auto* Custom = Cast<UMaterialExpressionCustom>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionCustom::StaticClass(), -200, 0));
	Custom->Code = HexCellViz::CustomCode;
	Custom->OutputType = CMOT_Float3;
	Custom->Description = TEXT("HexCellViz");
	Custom->IncludeFilePaths.Add(TEXT("/Plugin/HexMapDepthGenerator/Private/HexCellVisualizer.usf"));

	FCustomOutput MaskOut;
	MaskOut.OutputName = TEXT("Mask");
	MaskOut.OutputType = CMOT_Float1;
	Custom->AdditionalOutputs.Reset();
	Custom->AdditionalOutputs.Add(MaskOut);
	Custom->RebuildOutputs();

	Custom->Inputs.Reset();
	auto AddInput = [Custom](FName Name)
	{
		FCustomInput& Input = Custom->Inputs.AddDefaulted_GetRef();
		Input.InputName = Name;
	};
	AddInput(TEXT("HexCells"));
	AddInput(TEXT("HexRadius"));
	AddInput(TEXT("HexRotation"));
	AddInput(TEXT("HexWaterLevel"));
	AddInput(TEXT("HexMaxDepth"));
	AddInput(TEXT("HexWaterColor"));
	AddInput(TEXT("HexLandColor"));
	AddInput(TEXT("HexLightnessMin"));
	AddInput(TEXT("HexLightnessMax"));
	AddInput(TEXT("HexMapWidth"));
	AddInput(TEXT("HexMapHeight"));
	AddInput(TEXT("HexOrigin"));
	AddInput(TEXT("WorldPos"));

	UMaterialEditingLibrary::ConnectMaterialExpressions(TexParam, FString(), Custom, TEXT("HexCells"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(RadiusParam, FString(), Custom, TEXT("HexRadius"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(RotParam, FString(), Custom, TEXT("HexRotation"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(WaterLevelParam, FString(), Custom, TEXT("HexWaterLevel"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(MaxDepthParam, FString(), Custom, TEXT("HexMaxDepth"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(WaterColorParam, FString(), Custom, TEXT("HexWaterColor"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(LandColorParam, FString(), Custom, TEXT("HexLandColor"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(LightnessMinParam, FString(), Custom, TEXT("HexLightnessMin"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(LightnessMaxParam, FString(), Custom, TEXT("HexLightnessMax"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(MapWParam, FString(), Custom, TEXT("HexMapWidth"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(MapHParam, FString(), Custom, TEXT("HexMapHeight"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(OriginParam, FString(), Custom, TEXT("HexOrigin"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(WorldPos, FString(), Custom, TEXT("WorldPos"));
	UMaterialEditingLibrary::ConnectMaterialProperty(Custom, FString(), MP_EmissiveColor);
	UMaterialEditingLibrary::ConnectMaterialProperty(Custom, TEXT("Mask"), MP_OpacityMask);
	UMaterialEditingLibrary::RecompileMaterial(Mat);

	HexCellViz::GDefaultMaterial = Mat;
	return Mat;
#else
	return nullptr;
#endif
}
void UHexCellShaderVisualizer::EnsureDisplayMesh(const TArray<FHexCellInfo>& Cells)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (!DisplayMesh)
	{
		DisplayMesh = NewObject<UStaticMeshComponent>(Owner, NAME_None, WITH_EDITOR ? RF_Transactional : RF_NoFlags);
		if (UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane")))
			DisplayMesh->SetStaticMesh(PlaneMesh);
		DisplayMesh->SetupAttachment(Owner->GetRootComponent());
		DisplayMesh->RegisterComponent();
		Owner->AddInstanceComponent(DisplayMesh);
	}

	FVector2D Min(FLT_MAX, FLT_MAX), Max(-FLT_MAX, -FLT_MAX);
	for (const FHexCellInfo& Cell : Cells)
	{
		Min.X = FMath::Min(Min.X, Cell.X);
		Min.Y = FMath::Min(Min.Y, Cell.Y);
		Max.X = FMath::Max(Max.X, Cell.X);
		Max.Y = FMath::Max(Max.Y, Cell.Y);
	}

	if (Cells.Num() == 0)
	{
		Min = FVector2D::ZeroVector;
		Max = FVector2D::ZeroVector;
	}

	const float Pad = HexRadius * 2.f;
	Min.X -= Pad;
	Min.Y -= Pad;
	Max.X += Pad;
	Max.Y += Pad;

	const FVector2D Center = (Min + Max) * 0.5f;
	const FVector2D Extents = (Max - Min).GetAbs();
	DisplayMesh->SetWorldLocation(FVector(Center.X, Center.Y, Origin.Z + 1.f));
	DisplayMesh->SetWorldScale3D(FVector(FMath::Max(1.f, Extents.X * 0.01f), FMath::Max(1.f, Extents.Y * 0.01f), 1.f));
}
void UHexCellShaderVisualizer::UploadCells(const TArray<FHexCellInfo>& Cells)
{
	const int32 Width = FMath::Max(1, MapWidth);
	const int32 Height = FMath::Max(1, MapHeight);

	if (!CellsTexture || CellsTexture->GetSizeX() != Width || CellsTexture->GetSizeY() != Height)
	{
		CellsTexture = UTexture2D::CreateTransient(Width, Height, PF_A32B32G32R32F);
		CellsTexture->Filter = TF_Nearest;
		CellsTexture->AddressX = TA_Clamp;
		CellsTexture->AddressY = TA_Clamp;
		CellsTexture->SRGB = false;
		CellsTexture->UpdateResource();
	}

	FTexture2DMipMap& Mip = CellsTexture->GetPlatformData()->Mips[0];
	float* Data = reinterpret_cast<float*>(Mip.BulkData.Lock(LOCK_READ_WRITE));

	const int32 TexelCount = Width * Height;
	for (int32 i = 0; i < TexelCount; i++)
	{
		const int32 Base = i * 4;
		const int32 Alpha = i % Width;
		const int32 Beta = i / Width;
		const int32 CellIndex = Beta * Width + Alpha;

		if (Cells.IsValidIndex(CellIndex))
		{
			Data[Base + 0] = Cells[CellIndex].X;
			Data[Base + 1] = Cells[CellIndex].Y;
			Data[Base + 2] = Cells[CellIndex].Z;
			Data[Base + 3] = static_cast<float>(Cells[CellIndex].DepthLevel);
		}
		else
		{
			Data[Base + 0] = 0.f;
			Data[Base + 1] = 0.f;
			Data[Base + 2] = 0.f;
			Data[Base + 3] = 0.f;
		}
	}

	Mip.BulkData.Unlock();
	CellsTexture->UpdateResource();
}
