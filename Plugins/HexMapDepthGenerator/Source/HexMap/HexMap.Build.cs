// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

using UnrealBuildTool;

public class HexMap: ModuleRules
{
    public HexMap(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "DataStructures"});
        PrivateDependencyModuleNames.AddRange(new string[] {"DepthGenerator"});
    }
}
