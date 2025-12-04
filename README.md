# Lunar Outpost - Alien Invasion Survival (Year 6060)

**Author:** Meghasrivardhan P

**Course:** Computer Graphics  
**Base Framework:** Willem A. Schreuder's CSCIx229 library

## Project Overview

A first-person/third-person survival experience set on the lunar surface in the year 6060. Navigate a desolate moon landscape populated with alien lifeforms, mysterious energy-harvesting UFOs, abandoned military vehicles, and bioluminescent flora while surviving the harsh environment.

## What remains to be done

I haven’t yet used any of my old objects (**House**, **Tree**, **Ground**, **Target**, **Human**, **Broken Table**, **Old Car**, **Boulder**, **Grenade**, **Health-kit**, **Wrecked Plane**, **Front-plane**) from the lighting assignment, but I’ll be adding them soon. Some of these assets are still under construction and need a bit more time to finish. After that, I plan to introduce a shooting-game style experience with a main character at center with specific tasks to complete. Once the shooting section is finished, players will be able to board a plane (also currently under construction) and fly it like a simulator across the scene for about a minute before the game ends. I’ll also be adding collision detection and creating new ruins of large buildings, crashed planes, skeletons, and additional flora to enhance the environment.

## Build Instructions

**Run:**

```bash
make clean && make && ./L
```

**Preferred view:**

`Press 'v'` and wander around

## Technical Implementation

### Modified Files
- `LunarOutpost.c` (main program)
- `projection.c`
- `UFOSpacecraft.c` - Energy-harvesting spacecraft with tractor beams
- `JeepWrangler.c` - Abandoned military transport

### Custom Objects (AI-Assisted Development)
- `FloatingCrystalShard.c` - Energy source with volumetric lighting
- `AlienScoutDrone.c` - Autonomous patrol units
- `LunarisVex.c` - Bioluminescent alien character with dust particles
- `ArmoredTransportVehicle.c` - Ground and aerial patrol vehicles
- `AlienFloraCollection.c` - Six unique alien plant species
- `shader_loader.c` and shaders/


### Environment Assets
- 70 textured lunar rocks (rounded and cylindrical variants)
- 15 falling asteroids
- 20-star starfield backdrop
- Giant animated sun (150 unit radius)
- 5 UFO-crystal energy absorption pairs
- 12 wandering scout drones
- 25 dispersed flora instances (Will increase for aesthetics)

## Controls

### Movement (First-Person)
| Key | Function |
|-----|----------|
| `W` | Move forward |
| `S` | Move backward |
| `A` | Strafe left |
| `D` | Strafe right |
| Mouse | Look around |

### Camera & View
| Key | Function |
|-----|----------|
| `V` | Toggle First-Person / Third-Person view |
| `M` | Toggle mouse look on/off |

### Environment
| Key | Function |
|-----|----------|
| `X` | Toggle coordinate axes |

### Debug Features
| Key | Function |
|-----|----------|
| `P` | Cycle through object showcase mode (12 objects) |
| `ESC` | Exit program |

## Scene Statistics

- **Entities:** 8 aliens, 6 jeeps, 12 armored vehicles, 10 flying patrols, 12 scout drones
- **Environmental Objects:** 70 rocks, 15 asteroids, 25 flora instances
- **Interactive Pairs:** 5 UFO-crystal energy absorption sites

## Development Notes

### Texture Assets
- Moon surface textures (6 variants)
- Alien skin textures (3 types)
- Flora textures (3 variants + moss)
- Military vehicle textures (rusty metal)

### Planned Features
- Collision detection system
- Shooting mechanics with mission objectives
- Flyable plane vehicle (in-progress)
- Ruins of large buildings and structures
- Additional skeletal remains
- Enhanced flora variety

## Project Timeline

**Estimated Development Time:** ~25 days

