// Copyright Pinaev Danil (DaniElRuso) 2026. All Rights Reserved.

#include "HexShaderVisualizerModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

IMPLEMENT_MODULE(FHexShaderVisualizer, HexShaderVisualizer);

void FHexShaderVisualizer::StartupModule()
{
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("HexMapDepthGenerator"));
	if (Plugin.IsValid())
	{
		const FString ShaderDir = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders"));
		AddShaderSourceDirectoryMapping(TEXT("/Plugin/HexMapDepthGenerator"), ShaderDir);
	}
}

void FHexShaderVisualizer::ShutdownModule()
{
}
