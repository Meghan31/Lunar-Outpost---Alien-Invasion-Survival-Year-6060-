# Lunar Outpost - Alien Invasion Survival (Year 6060)

**Author:** Meghasrivardhan P

**Course:** Computer Graphics  
**Base Framework:** Willem A. Schreuder's CSCIx229 library

## Project Overview

A first-person/third-person exploration experience set on the lunar surface in the year 6060. Navigate a desolate moon landscape populated with alien lifeforms, mysterious energy-harvesting UFOs, abandoned military vehicles, and bioluminescent flora while surviving the harsh environment.

## Build Instructions

**Run:**

```bash
make clean && make && ./final
```

**Preferred view:**

`Press 'v'` and wander around

## Technical Implementation

### Modified Files

- `LunarOutpost.c` (main program)
- `projection.c`
- `UFOSpacecraft.c` - Energy-harvesting spacecraft with tractor beams
- `JeepWrangler.c` - Abandoned military transport
- `Helicopter.c` - Helicopter Model

### Custom Objects (AI-Assisted Development)

- `FloatingCrystalShard.c` - Energy source with volumetric lighting
- `AlienScoutDrone.c` - Autonomous patrol units
- `LunarisVex.c` - Bioluminescent alien character with dust particles
- `ArmoredTransportVehicle.c` - Ground and aerial patrol vehicles
- `AlienFloraCollection.c` - Six unique alien plant species
- `shader_loader.c` and shaders/

### Reused Assignment Files

- `Ball.c`
- `Ball.h`
- `Boulder.c`
- `Boulder.h`
- `BrokenTable.c`
- `BrokenTable.h`
- `Grenade.c`
- `Grenade.h`
- `Ground.c`
- `Ground.h`
- `HealthKit.h`
- `Helicopter.c`
- `Helicopter.h`
- `House.c`
- `House.h`
- `Human.c`
- `Human.h`
- `OldCar.c`
- `OldCar.h`
- `Plane.c`
- `FrontPlane.c`
- `Primitives.c`
- `Primitives.h`

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

| Key   | Function      |
| ----- | ------------- |
| `W`           | Move forward  |
| `S`           | Move backward |
| `A`           | Strafe left   |
| `D`           | Strafe right  |
| Mouse         | Look around   |
| `UP_ARROW`    | helicopter will move up   |
| `DOWN_ARROW`  | helicopter will move down |
| `LEFT_ARROW`  | helicopter will Strafe left   |
| `RIGHT_ARROW` | helicopter will Strafe right |

### Camera & View

| Key | Function                                |
| --- | --------------------------------------- |
| `V` | Toggle First-Person / Third-Person view |
| `M` | Toggle mouse look on/off                |
| `E` | Toggle Helicopter simulator             |

### Environment

| Key | Function               |
| --- | ---------------------- |
| `X` | Toggle coordinate axes |

### Debug Features

| Key   | Function     |
| ----- | ------------ |
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

## Project Timeline

**Estimated Development Time:** ~40+ days
