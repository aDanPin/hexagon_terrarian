// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

using UnrealBuildTool;

public class HexMapDepthGenerator : ModuleRules
{
	public HexMapDepthGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "DataStructures", "DepthGenerator", "HexMap", "HexShaderVisualizer" });
	}
}
