using UnrealBuildTool;

public class HexMap: ModuleRules
{
    public HexMap(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});
    }
}
