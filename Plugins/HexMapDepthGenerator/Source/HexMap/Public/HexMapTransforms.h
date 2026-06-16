#pragma once

#include "CoreMinimal.h"

struct HEXMAP_API FHexMapTransforms
{
	static FIntPoint AlphaBettaToXY(int32 Alpha, int32 Beta);
	static FIntPoint XYToBetaAlfa(int32 X, int32 Y);
	static int32 XYToAlpha(int32 X, int32 Y);
	static int32 XYToBeta(int32 X, int32 Y);
	static float AlphaBettaToPx(int32 Alpha, int32 Beta, float HexRadius);
	static float AlphaBettaToPz(int32 Alpha, int32 Beta, float HexRadius);
	static bool TryNearestAlphaBetaFromLocalXZ(float Lx, float Lz, float HexRadius, int32 MapWidth, int32 MapHeight, int32& OutAlpha, int32& OutBeta);
	static float Distance(int32 X1, int32 Y1, int32 X2, int32 Y2, float HexRadius);
	static float DistanceInHex(int32 X1, int32 Y1, int32 X2, int32 Y2);
};
