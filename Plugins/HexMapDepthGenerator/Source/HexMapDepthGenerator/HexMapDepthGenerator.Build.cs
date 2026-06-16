using UnrealBuildTool;

public class HexMapDepthGenerator : ModuleRules
{
	public HexMapDepthGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "DepthGenerator", "HexMap" });
	}
}
