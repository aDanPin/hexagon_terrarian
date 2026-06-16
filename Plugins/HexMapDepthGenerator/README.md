# Hex Map Depth Generator

Procedural hexagonal map generator for Unreal Engine. It builds a seeded depth map from configurable noise layers and places hex meshes per depth level using a Hierarchical Instanced Static Mesh, right in the editor.

## Install

1. Copy the `HexMapDepthGenerator` folder into your project's `Plugins/` directory.
2. Open the project and let the editor build the plugin modules.
3. Enable the plugin in Edit > Plugins if needed.

## Usage

1. Add a `Hex Map Generator` component to any Actor.
2. Set the meshes for each depth level in `DepthLevelMeshes` (index 0 = lowest level).
3. Tune the map and noise settings, then press `Regenerate` (also re-runs automatically when properties change in the editor).

## Parameters

- `MapWidth` / `MapHeight` - grid size in hexes.
- `HexRadius` / `HexRotation` - placement of each hex.
- `GlobalSeed` - seed; derives per-layer seeds deterministically.
- `DepthLevelMeshes` - mesh used for each depth level; the level count equals the number of meshes.
- `GenerationLevels` - noise layers (Perlin / Euclidean / Value) with amplitude, power and apply mode (Add, Subtract, Multiply, Lerp, Replace) combined into the final depth.
- `Regenerate` - rebuilds the map.

## Modules

- `HexMap` - hex grid math and mesh placement.
- `DepthGenerator` - depth map generation from noise layers.
- `HexMapDepthGenerator` - the `UHexMapGeneratorComponent` user-facing component.

Example meshes and a demo map are provided in the accompanying demo project.
