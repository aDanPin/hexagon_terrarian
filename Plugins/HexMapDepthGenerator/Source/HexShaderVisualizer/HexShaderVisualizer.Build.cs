// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

using UnrealBuildTool;

public class HexShaderVisualizer : ModuleRules
{
	public HexShaderVisualizer(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DataStructures",
			"RenderCore",
			"Projects"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("MaterialEditor");
		}
	}
}
