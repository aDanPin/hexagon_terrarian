// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

using UnrealBuildTool;

public class DepthGenerator: ModuleRules
{
    public DepthGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "DataStructures"});
    }
}
