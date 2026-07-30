// Copyright (c) 2026 Pinaev Danil (DaniElRuso). MIT License.

using UnrealBuildTool;

public class HexMap: ModuleRules
{
    public HexMap(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "DataStructures"});
        PrivateDependencyModuleNames.AddRange(new string[] {"DepthGenerator"});
    }
}
