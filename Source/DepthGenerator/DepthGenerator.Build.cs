using UnrealBuildTool;

public class DepthGenerator: ModuleRules
{
    public DepthGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});
    }
}
