// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

using UnrealBuildTool;

public class HexMapDepthGenerator : ModuleRules
{
	public HexMapDepthGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "DataStructures", "DepthGenerator", "HexMap" });
	}
}
