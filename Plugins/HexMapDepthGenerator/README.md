# Advanced Hex Map Depth Generator

Advanced hex depth map generator. 
Seed-based procedural generation. 
Support for custom patterns that allow creating template maps: continents, islands, one large island, plains with lakes, etc. 
Support for custom meshes for hexes. 
Support for generating hex map data without generating the map on the scene, for manual map creation by the user.

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

Example meshes and a demo map are in `Content/Demo/`.
