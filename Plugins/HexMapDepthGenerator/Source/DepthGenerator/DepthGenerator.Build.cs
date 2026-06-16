using UnrealBuildTool;

public class DepthGenerator: ModuleRules
{
    public DepthGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});
    }
}
