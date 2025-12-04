/*
 *  Lunar Outpost - Alien Invasion Survival (Year 6060)
 *  
 *  First-person survival game set on the Moon
 *  Features: Giant textured sun, starfield, irregular lunar terrain with craters
 *  
 *  Key bindings:
 *  WASD       Moving forward/left/backward/right
 *  Mouse      Looking around (FPP)
 *  v/V        Toggling FPP/Third-person debug view
 *  m/M        Toggling mouse look
 *  x/X        Toggling axes
 *  ESC        Exiting
 */

 // Took assistance from AI in building parts of the codebase such as declaration of objects outside of this file and randomizing the objects

#include "CSCIx229.h"
#include <stdbool.h>
#include "FloatingCrystalShard.h"
#include "AlienScoutDrone.h"
// #include "BioluminescentSporeCloud.h"
// #include "// GeometricAlienFlora.h"
#include "LunarisVex.h"
#include "UFOSpacecraft.h"
#include "ArmoredTransportVehicle.h"
#include "AlienFloraCollection.h"
#include "JeepWrangler.h"

// Forward declaration needed before extra drone initialization uses it
double GetTerrainHeight(double worldX, double worldZ);


// Extra Alien Scout Drones (12 more)

// Extra scout drones in addition to any single test drone
#define NUM_EXTRA_DRONES 12
typedef struct {
   float x,y,z;        // position
   float baseY;        // terrain base plus initial offset
   float facing;       // degrees
   float speed;        // movement speed
   float scale;        // size multiplier
   int   seed;         // per-drone seed for animation variation
   float turnCooldown; // frames or time until next random turn allowed
} ExtraDrone;
static ExtraDrone extraDrones[NUM_EXTRA_DRONES];

static void InitExtraDrones()
{
   srand(246810); // using different seed
   for(int i=0;i<NUM_EXTRA_DRONES;i++)
   {
      // Disperse broadly but keep inside terrain bounds (-230..230)
      extraDrones[i].x = (float)rand()/RAND_MAX * 460.0f - 230.0f;
      extraDrones[i].z = (float)rand()/RAND_MAX * 460.0f - 230.0f;
      double th = GetTerrainHeight(extraDrones[i].x, extraDrones[i].z);
      // Base altitude 3..8 units above ground
      extraDrones[i].baseY = th + 3.0f + (float)rand()/RAND_MAX * 5.0f;
      extraDrones[i].y = extraDrones[i].baseY;
      extraDrones[i].facing = (float)rand()/RAND_MAX * 360.0f; // random heading
      extraDrones[i].speed = 0.10f + (float)rand()/RAND_MAX * 0.08f; // modest drift
      extraDrones[i].scale = 0.45f + (float)rand()/RAND_MAX * 0.25f;
      extraDrones[i].seed = rand()%1000;
      extraDrones[i].turnCooldown = 0.0f;
   }
}

static void UpdateExtraDrones(double time)
{
   for(int i=0;i<NUM_EXTRA_DRONES;i++)
   {
      // Smoothed hover: base + sine waves with per-drone phase
      double phase = time*2.0 + extraDrones[i].seed*0.013;
      extraDrones[i].y = extraDrones[i].baseY + 0.6*sin(phase) + 0.3*sin(phase*0.37 + 1.5);

      // Random slight heading changes
      if(extraDrones[i].turnCooldown <= 0.0f)
      {
         if(rand()%120 == 0) // occasional turn chance
         {
            float delta = ((float)rand()/RAND_MAX * 60.0f) - 30.0f; // -30..+30 deg
            extraDrones[i].facing += delta;
            if(extraDrones[i].facing < 0) extraDrones[i].facing += 360.0f;
            if(extraDrones[i].facing >= 360.0f) extraDrones[i].facing -= 360.0f;
            extraDrones[i].turnCooldown = 0.5f; // half second before another big turn
         }
      }
      else
      {
         extraDrones[i].turnCooldown -= 0.016f; // approximate frame time
      }

      // Forward movement in horizontal plane
      double rad = extraDrones[i].facing * M_PI/180.0;
      extraDrones[i].x += sin(rad) * extraDrones[i].speed;
      extraDrones[i].z += cos(rad) * extraDrones[i].speed;

      // Wrap bounds
      if(extraDrones[i].x > 250.0f) extraDrones[i].x = -250.0f;
      if(extraDrones[i].x < -250.0f) extraDrones[i].x = 250.0f;
      if(extraDrones[i].z > 250.0f) extraDrones[i].z = -250.0f;
      if(extraDrones[i].z < -250.0f) extraDrones[i].z = 250.0f;
   }
}

// Global variable
// SporeCloud* sporeCloud1;

// Window size
int windowWidth = 1200;
int windowHeight = 800;

// Camera and view settings
int axes = 0;           // Display axes for debugging
int light = 1;          // Lighting enabled
int viewMode = 1;       // 1 is FPP, 0 is Third-person debug
int mouseLook = 1;      // Mouse look enabled
double asp = 1.0;       // Aspect ratio
double fov = 70;        // Field of view, wider for dramatic effect

// Player position and movement
double playerX = 0.0;
double playerY = 2.0;   // Height above ground
double playerZ = 30.0;  // Starting further back to see scene
double playerHeight = 1.8;

// Player rotation in degrees
double playerYaw = 0.0;
double playerPitch = 0.0;

// Movement flags
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;

// Mouse tracking
int lastMouseX = 0;
int lastMouseY = 0;
bool firstMouse = true;
double mouseSensitivity = 0.15;  // Responsive mouse control for smooth looking

// Movement constants
const double MOVE_SPEED = 8.0;     // Movement speed in units per second
const double MOUSE_SMOOTH = 0.05;  // Mouse smoothing

// Sun and lighting animation
double sunTime = 0.0;
int animateSun = 0;  // Toggling sun animation


#define SUN_WORLD_X -300.0
#define SUN_WORLD_Z -400.0

// Calculating facing direction towards sun
static double FacingTowardSun(double x, double z)
{
   double dx = SUN_WORLD_X - x;
   double dz = SUN_WORLD_Z - z;
   double ang = atan2(dx, dz) * 180.0 / M_PI; // atan2 for x,z matching sin/cos usage later
   if (ang < 0) ang += 360.0;
   return ang;
}

// Texture IDs
unsigned int moonTexture;
unsigned int sunTexture;
unsigned int rockTexture1;  // moon2.bmp
unsigned int rockTexture2;  // moon11.bmp
unsigned int rockTexture3;  // rock.bmp
unsigned int rockTexture4;  // wall.bmp
unsigned int rockTexture5;  // rusty_car.bmp
unsigned int rockTexture6;  // rusty_plane.bmp

// Flora textures
unsigned int floraTexture1;  // flora-1.bmp
unsigned int floraTexture2;  // flora-2.bmp
unsigned int floraTexture3;  // flora-3.bmp
unsigned int mossTexture;    // moss.bmp

// Alien textures
unsigned int alienTexture1;  // alien-type-1.bmp
unsigned int alienTexture2;  // alien-type-2.bmp
unsigned int alienTexture3;  // alien-type-3.bmp

// Crystal setup
#define NUM_CRYSTALS 5
typedef struct {
   float x, y, z;
   float scale;
   int seed;
} Crystal;
Crystal crystals[NUM_CRYSTALS];

// Aliens wandering around
#define NUM_ALIENS 8  // Cut from 15 to 8 for better performance
typedef struct {
   float x, y, z;
   float facing;
   int emotion;  // 0 is calm, 1 is curious, 2 is alert, 3 is thinking
   float speed;
   float rotationSpeed;
   int isGliding;
} AlienEntity;
AlienEntity aliens[NUM_ALIENS];

// UFO spacecrafts
#define NUM_UFOS 5  // Cut from 8 to 5 for better performance
typedef struct {
   float x, y, z;
   float scale;
   float hoverOffset;
   int hasBeam;
   float rotationSpeed;
} UFOEntity;
UFOEntity ufos[NUM_UFOS];

// Jeep Wranglers
#define NUM_JEEPS 6  // Cut from 10 to 6 for better performance
typedef struct {
   float x, y, z;
   float scale;
   float facing;
   float speed;
   float dirAngle;   // Fixed direction in radians for straight-line movement
} JeepEntity;
JeepEntity jeeps[NUM_JEEPS];

// Armored Transport Vehicles
#define NUM_ARMORED_VEHICLES 12
typedef struct {
   float x, y, z;
   float facing;    // degrees
   float speed;     // units per frame
   float dirAngle;  // direction of travel in radians
   float scale;     // render scale
} ArmoredEntity;
ArmoredEntity armoredVehicles[NUM_ARMORED_VEHICLES];

// Flying Armored Vehicles patrolling in the air
#define NUM_FLYING_ARMORED 10
typedef struct {
   float x, y, z;
   float baseY;     // base altitude
   float facing;    // degrees
   float speed;     // units per frame
   float dirAngle;  // direction of travel in radians
   float scale;     // render scale
   int seed;        // for hover variation
} FlyingArmoredEntity;
FlyingArmoredEntity flyingArmored[NUM_FLYING_ARMORED];

// Flora instances scattered across terrain
#define NUM_FLORA 25  // Scattered flora around the map
typedef struct {
   float x, y, z;
   float scale;
   int type;  // 0 to 5: EggCluster, AntlerBranch, BubbleStack, CoralBranch, BladeFan, MushroomCaps
   int textureIndex;  // Which texture we're using, 0 to 3
   int seed;  // For procedural variation
} FloraEntity;
FloraEntity flora[NUM_FLORA];

// Object showcase settings
int showcaseMode = 0;  // 0 is normal game, 1 is showcase for individual objects
int showcaseObject = 0;  // Which object we're displaying, 0 to 2
#define NUM_SHOWCASE_OBJECTS 12  // FloatingCrystal, AlienDrone, BiolumSporeCloud, GeometricAlienFlora

// Terrain grid
#define TERRAIN_SIZE 65
#define TERRAIN_SCALE 8.0  // Larger scale for rolling dunes
float terrainZ[TERRAIN_SIZE][TERRAIN_SIZE];
float terrainNormals[TERRAIN_SIZE][TERRAIN_SIZE][3];

// Stars in the sky
#define NUM_STARS 20
typedef struct {
   float x, y, z;
   float size;
   float brightness;
} Star;
Star stars[NUM_STARS];

// Asteroids moving through the sky
#define NUM_ASTEROIDS 15
typedef struct {
   float x, z;          // Horizontal position
   float y;             // Vertical position or height
   float size;          // Asteroid size
   float speed;         // Falling speed
   float rotX, rotY, rotZ;  // Rotation angles
   float rotSpeedX, rotSpeedY, rotSpeedZ;  // Rotation speeds
} Asteroid;
Asteroid asteroids[NUM_ASTEROIDS];

double GetTerrainHeight(double worldX, double worldZ);


/*
 * Setting up crystal positions across the map
 */
void InitCrystals()
{
   srand(99999);  // Using a different seed from asteroids and stars
   for (int i = 0; i < NUM_CRYSTALS; i++)
   {
      // Random positions across the terrain
      crystals[i].x = (float)rand() / RAND_MAX * 400.0 - 200.0;
      crystals[i].z = (float)rand() / RAND_MAX * 400.0 - 200.0;
      crystals[i].y = GetTerrainHeight(crystals[i].x, crystals[i].z) + 1.5;  // Hover above ground
      
      // Size variation (0.5 to 2.0)
      crystals[i].scale = 0.5 + (float)rand() / RAND_MAX * 1.5;
      
      // Unique seed for each crystal
      crystals[i].seed = rand() % 1000;
   }
}

/*
 * Drawing focused absorption beam between UFO and crystal
 * Creating a pulsing, slightly widening energy column suggesting UFO is extracting energy
 */
static void DrawAbsorptionBeam(double x, double crystalY, double z, double ufoBottomY, double time)
{
   double length = ufoBottomY - crystalY;  // Vertical span
   if (length <= 0.1) return;

   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glDepthMask(GL_FALSE);

   int segments = 32;
   int layers = 12;

   for (int layer = 0; layer < layers; layer++)
   {
      double t = layer / (double)layers;
      double y0 = crystalY + t * length;
      double y1 = crystalY + (t + 1.0/layers) * length;

      // Radius widens slightly toward UFO to imply suction funnel
      double r0 = 0.25 + 0.15 * t;
      double r1 = 0.25 + 0.15 * (t + 1.0/layers);

      // Pulsing intensity & color shift (cyan -> magenta blend)
      double pulse = 0.7 + 0.3 * sin(time * 4.0 - t * 6.283);
      double phase = fmod(time * 0.5 + t, 1.0);
      float cr = (1.0 - phase) * 0.2 + phase * 1.0;     // R from 0.2 to 1.0
      float cg = (1.0 - phase) * 0.9 + phase * 0.2;     // G from 0.9 to 0.2
      float cb = 1.0;                                   // B stays high for energy look
      float alpha = (1.0 - t) * 0.35 * pulse;           // Fade toward top

      glColor4f(cr, cg, cb, alpha);
      glBegin(GL_QUAD_STRIP);
      for (int s = 0; s <= segments; s++)
      {
         double ang = s * 2.0 * M_PI / segments + time * 2.0; // slight rotation
         glVertex3d(x + r0 * Cos(ang), y0, z + r0 * Sin(ang));
         glVertex3d(x + r1 * Cos(ang), y1, z + r1 * Sin(ang));
      }
      glEnd();
   }

   // Central bright core line
   glLineWidth(2.0);
   glColor4f(1.0, 1.0, 0.9, 0.6);
   glBegin(GL_LINES);
   glVertex3d(x, crystalY, z);
   glVertex3d(x, ufoBottomY, z);
   glEnd();
   glLineWidth(1.0);

   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
}


/*
 * Setting up aliens across the map
 */
void InitAliens()
{
   srand(77777);  // Using different seed
   for (int i = 0; i < NUM_ALIENS; i++)
   {
      // Placing them randomly across the terrain
      aliens[i].x = (float)rand() / RAND_MAX * 400.0 - 200.0;
      aliens[i].z = (float)rand() / RAND_MAX * 400.0 - 200.0;
      aliens[i].y = GetTerrainHeight(aliens[i].x, aliens[i].z) + 2.0;  // Increased from 0.5 to 2.0
      
   // Making them face toward sun on spawn
   aliens[i].facing = FacingTowardSun(aliens[i].x, aliens[i].z);
      
      // Random emotion (0-3)
      aliens[i].emotion = rand() % 4;
      
      // Movement properties
      aliens[i].speed = 0.05 + (float)rand() / RAND_MAX * 0.1;
      aliens[i].rotationSpeed = 10.0 + (float)rand() / RAND_MAX * 20.0;
      aliens[i].isGliding = rand() % 2;
   }
}

/*
 * Setting up UFOs across the sky
 */
void InitUFOs()
{
   srand(88888);  // Using different seed
   for (int i = 0; i < NUM_UFOS; i++)
   {
      // Random positions across the map
      ufos[i].x = (float)rand() / RAND_MAX * 400.0 - 200.0;
      ufos[i].z = (float)rand() / RAND_MAX * 400.0 - 200.0;
      
      // Hovering height
      ufos[i].y = 10.0 + (float)rand() / RAND_MAX * 15.0;
      
      // Size variation
      ufos[i].scale = 1.5 + (float)rand() / RAND_MAX * 2.0;
      
      // Hover offset for animation
      ufos[i].hoverOffset = (float)rand() / RAND_MAX * 6.28;
      
      // Some have beams, some don't
      ufos[i].hasBeam = rand() % 2;
      
      // Rotation speed
      ufos[i].rotationSpeed = 10.0 + (float)rand() / RAND_MAX * 30.0;
   }
}

/*
 * Setting up Jeeps across the map
 */
void InitJeeps()
{
   srand(99999);  // Using different seed
   for (int i = 0; i < NUM_JEEPS; i++)
   {
      // Random positions across the terrain
      jeeps[i].x = (float)rand() / RAND_MAX * 400.0 - 200.0;
      jeeps[i].z = (float)rand() / RAND_MAX * 400.0 - 200.0;
      jeeps[i].y = GetTerrainHeight(jeeps[i].x, jeeps[i].z) + 0.8;  // Added 0.8 offset
      
   // Making them face toward sun on spawn
   jeeps[i].facing = FacingTowardSun(jeeps[i].x, jeeps[i].z);
      
      // Size variation
      jeeps[i].scale = 0.8 + (float)rand() / RAND_MAX * 0.6;
      
      // Movement properties
      jeeps[i].speed = 0.1 + (float)rand() / RAND_MAX * 0.15;
      // Straight-line direction toward sun (match facing)
      jeeps[i].dirAngle = jeeps[i].facing * M_PI / 180.0;
   }
}

/* Setting up two armored transport vehicles 100 units ahead of player, offset left and right */
void InitArmoredVehicles()
{
   // Getting player forward direction, playerYaw already set before this call in main
   double yawRad = playerYaw * M_PI / 180.0;
   double fwdX = sin(yawRad);
   double fwdZ = cos(yawRad);
   // Getting perpendicular vector for lateral offset
   double perpX = cos(yawRad);
   double perpZ = -sin(yawRad);
   double baseDist = 100.0;  // 100 meters or units ahead
   double lateralOffset = 15.0; // spreading them apart
   // Using seed for dispersed random spawns, distinct from others
   srand(424242);

   for (int i = 0; i < NUM_ARMORED_VEHICLES; i++)
   {
      if (i < 2)
      {
         // Original two convoy transports ahead of player
         double side = (i == 0) ? -1.0 : 1.0; // left/right
         armoredVehicles[i].x = playerX + fwdX * baseDist + perpX * lateralOffset * side;
         armoredVehicles[i].z = playerZ + fwdZ * baseDist + perpZ * lateralOffset * side;
         armoredVehicles[i].y = GetTerrainHeight(armoredVehicles[i].x, armoredVehicles[i].z) + 0.5;
         armoredVehicles[i].facing = FacingTowardSun(armoredVehicles[i].x, armoredVehicles[i].z); // face sun
         armoredVehicles[i].dirAngle = armoredVehicles[i].facing * M_PI / 180.0; // move toward sun direction
         armoredVehicles[i].speed = 0.12; // moderate forward speed
         armoredVehicles[i].scale = 1.1;
      }
      else
      {
         // Dispersed transports across terrain
         int attempts = 0;
         double rx, rz;
         do
         {
            rx = (double)rand() / RAND_MAX * 440.0 - 220.0; // within ~terrain bounds
            rz = (double)rand() / RAND_MAX * 440.0 - 220.0;
            attempts++;
            // Avoid spawning too close to player (keep cinematic front clear)
         } while (attempts < 8 && hypot(rx - playerX, rz - playerZ) < 40.0);

         armoredVehicles[i].x = rx;
         armoredVehicles[i].z = rz;
         armoredVehicles[i].y = GetTerrainHeight(armoredVehicles[i].x, armoredVehicles[i].z) + 0.5;

         // Face sun for consistency with earlier global rule
         armoredVehicles[i].facing = FacingTowardSun(armoredVehicles[i].x, armoredVehicles[i].z);
         armoredVehicles[i].dirAngle = armoredVehicles[i].facing * M_PI / 180.0;

         // Slight variation in speed & scale for visual diversity
         armoredVehicles[i].speed = 0.09 + (double)rand() / RAND_MAX * 0.07; // ~0.09 - 0.16
         armoredVehicles[i].scale = 0.95 + (double)rand() / RAND_MAX * 0.30; // ~0.95 - 1.25
      }
   }
}

/* Updating armored transport vehicles straight-line movement */
void UpdateArmoredVehicles(double time)
{
   for (int i = 0; i < NUM_ARMORED_VEHICLES; i++)
   {
      armoredVehicles[i].x += sin(armoredVehicles[i].dirAngle) * armoredVehicles[i].speed;
      armoredVehicles[i].z += cos(armoredVehicles[i].dirAngle) * armoredVehicles[i].speed;
      armoredVehicles[i].y = GetTerrainHeight(armoredVehicles[i].x, armoredVehicles[i].z) + 0.5;

      // Wrap bounds similar to others
      if (armoredVehicles[i].x > 250.0) armoredVehicles[i].x = -250.0;
      if (armoredVehicles[i].x < -250.0) armoredVehicles[i].x = 250.0;
      if (armoredVehicles[i].z > 250.0) armoredVehicles[i].z = -250.0;
      if (armoredVehicles[i].z < -250.0) armoredVehicles[i].z = 250.0;
   }
}

/* Setting up flying armored vehicles dispersed across the sky */
void InitFlyingArmoredVehicles()
{
   srand(987654); // using unique seed for flying vehicles
   for (int i = 0; i < NUM_FLYING_ARMORED; i++)
   {
      // Disperse across terrain bounds
      flyingArmored[i].x = (float)rand() / RAND_MAX * 440.0 - 220.0;
      flyingArmored[i].z = (float)rand() / RAND_MAX * 440.0 - 220.0;
      
      // Flying altitude 8-20 units above terrain
      double th = GetTerrainHeight(flyingArmored[i].x, flyingArmored[i].z);
      flyingArmored[i].baseY = th + 8.0 + (float)rand() / RAND_MAX * 12.0;
      flyingArmored[i].y = flyingArmored[i].baseY;
      
      // Random heading
      flyingArmored[i].facing = (float)rand() / RAND_MAX * 360.0;
      flyingArmored[i].dirAngle = flyingArmored[i].facing * M_PI / 180.0;
      
      // Moderate flight speed
      flyingArmored[i].speed = 0.12 + (float)rand() / RAND_MAX * 0.10; // 0.12-0.22
      flyingArmored[i].scale = 1.0 + (float)rand() / RAND_MAX * 0.3; // 1.0-1.3
      flyingArmored[i].seed = rand() % 1000;
   }
}

/* Updating flying armored vehicles movement */
void UpdateFlyingArmoredVehicles(double time)
{
   for (int i = 0; i < NUM_FLYING_ARMORED; i++)
   {
      // Horizontal movement
      flyingArmored[i].x += sin(flyingArmored[i].dirAngle) * flyingArmored[i].speed;
      flyingArmored[i].z += cos(flyingArmored[i].dirAngle) * flyingArmored[i].speed;
      
      // Gentle altitude oscillation
      double phase = time * 1.5 + flyingArmored[i].seed * 0.01;
      flyingArmored[i].y = flyingArmored[i].baseY + 0.8 * sin(phase) + 0.4 * sin(phase * 0.4 + 2.1);
      
      // Wrap bounds
      if (flyingArmored[i].x > 250.0) flyingArmored[i].x = -250.0;
      if (flyingArmored[i].x < -250.0) flyingArmored[i].x = 250.0;
      if (flyingArmored[i].z > 250.0) flyingArmored[i].z = -250.0;
      if (flyingArmored[i].z < -250.0) flyingArmored[i].z = 250.0;
   }
}

/*
 * Setting up Flora across the terrain
 * Scattered plants with proper height offsets staying above terrain
 */
void InitFlora()
{
   srand(888888);  // Using different seed for flora
   for (int i = 0; i < NUM_FLORA; i++)
   {
      // Random positions across the terrain
      flora[i].x = (float)rand() / RAND_MAX * 500.0 - 250.0;
      flora[i].z = (float)rand() / RAND_MAX * 500.0 - 250.0;
      
      // Get terrain height and add offset to ensure plants are above surface
      // Different offsets based on plant type (some are taller than others)
      int type = rand() % 6;  // 6 flora types
      flora[i].type = type;
      
      // Height offset varies by type
      float heightOffset = 0.0;
      if (type == 0) heightOffset = 0.3;  // EggCluster - sits close to ground
      else if (type == 1) heightOffset = 0.5;  // AntlerBranch - taller base
      else if (type == 2) heightOffset = 0.4;  // BubbleStack
      else if (type == 3) heightOffset = 0.4;  // CoralBranch
      else if (type == 4) heightOffset = 0.2;  // BladeFan - base is minimal
      else if (type == 5) heightOffset = 0.3;  // MushroomCaps
      
      flora[i].y = GetTerrainHeight(flora[i].x, flora[i].z) + heightOffset;
      
      // Scale variation
      flora[i].scale = 0.8 + (float)rand() / RAND_MAX * 0.8;
      
      // Assign texture (cycle through 4 textures)
      flora[i].textureIndex = i % 4;
      
      // Unique seed for each plant for procedural variation
      flora[i].seed = rand();
   }
}


/*
 * Setting up starfield
 * Random positions in the sky with varying sizes
 */
void InitStarfield()
{
   srand(42);  // Using fixed seed for consistent stars
   for (int i = 0; i < NUM_STARS; i++)
   {
      // Position stars in a hemisphere above
      float theta = (float)rand() / RAND_MAX * 360.0;
      float phi = (float)rand() / RAND_MAX * 60.0 + 20.0;  // Upper hemisphere
      float dist = 400.0;  // Far away
      
      stars[i].x = dist * Sin(theta) * Cos(phi);
      stars[i].y = dist * Sin(phi);
      stars[i].z = dist * Cos(theta) * Cos(phi);
      stars[i].size = 1.0 + (float)rand() / RAND_MAX * 3.0;
      stars[i].brightness = 0.6 + (float)rand() / RAND_MAX * 0.4;
   }
}

/*
 * Setting up asteroids moving through the sky
 */
void InitAsteroids()
{
   srand(12345);  // Using different seed from stars
   for (int i = 0; i < NUM_ASTEROIDS; i++)
   {
      // Random horizontal positions spread across the sky
      asteroids[i].x = (float)rand() / RAND_MAX * 400.0 - 200.0;
      asteroids[i].z = (float)rand() / RAND_MAX * 400.0 - 200.0;
      
      // Start at various heights above (200-400 units high)
      asteroids[i].y = 200.0 + (float)rand() / RAND_MAX * 200.0;
      
      // Size variation (1.0 to 4.0)
      asteroids[i].size = 1.0 + (float)rand() / RAND_MAX * 3.0;
      
      // Slow fall speed (0.05 to 0.2 units per frame)
      asteroids[i].speed = 0.05 + (float)rand() / RAND_MAX * 0.15;
      
      // Random initial rotations
      asteroids[i].rotX = (float)rand() / RAND_MAX * 360.0;
      asteroids[i].rotY = (float)rand() / RAND_MAX * 360.0;
      asteroids[i].rotZ = (float)rand() / RAND_MAX * 360.0;
      
      // Slow rotation speeds
      asteroids[i].rotSpeedX = (float)rand() / RAND_MAX * 2.0 - 1.0;
      asteroids[i].rotSpeedY = (float)rand() / RAND_MAX * 2.0 - 1.0;
      asteroids[i].rotSpeedZ = (float)rand() / RAND_MAX * 2.0 - 1.0;
   }
}

/*
 * Updating asteroid positions, call this in idle function
 */
void UpdateAsteroids()
{
   for (int i = 0; i < NUM_ASTEROIDS; i++)
   {
      // Move asteroid downward
      asteroids[i].y -= asteroids[i].speed;
      
      // Update rotation
      asteroids[i].rotX += asteroids[i].rotSpeedX;
      asteroids[i].rotY += asteroids[i].rotSpeedY;
      asteroids[i].rotZ += asteroids[i].rotSpeedZ;
      
      // Reset to top if it goes below ground
      if (asteroids[i].y < -10.0)
      {
         asteroids[i].y = 200.0 + (float)rand() / RAND_MAX * 200.0;
         asteroids[i].x = (float)rand() / RAND_MAX * 400.0 - 200.0;
         asteroids[i].z = (float)rand() / RAND_MAX * 400.0 - 200.0;
      }
   }
}

/*
 * Updating aliens, making them move on the terrain
 */
void UpdateAliens(double time)
{
   for (int i = 0; i < NUM_ALIENS; i++)
   {
      // Update facing direction (slow rotation)
      aliens[i].facing += aliens[i].rotationSpeed * 0.016;  // ~60fps
      if (aliens[i].facing > 360.0) aliens[i].facing -= 360.0;
      
      // Move forward in facing direction
      double radians = aliens[i].facing * M_PI / 180.0;
      aliens[i].x += sin(radians) * aliens[i].speed;
      aliens[i].z += cos(radians) * aliens[i].speed;
      
      // Keep within terrain bounds (-250 to 250)
      if (aliens[i].x > 250.0) aliens[i].x = -250.0;
      if (aliens[i].x < -250.0) aliens[i].x = 250.0;
      if (aliens[i].z > 250.0) aliens[i].z = -250.0;
      if (aliens[i].z < -250.0) aliens[i].z = 250.0;
      
      // Update Y position to stay on terrain
      aliens[i].y = GetTerrainHeight(aliens[i].x, aliens[i].z) + 2.0;  // Increased from 0.5 to 2.0
   }
}

/*
 * Updating jeeps, making them drive on the terrain
 */
void UpdateJeeps(double time)
{
   for (int i = 0; i < NUM_JEEPS; i++)
   {
      // Straight-line movement along fixed direction
      double dx = cos(jeeps[i].dirAngle) * jeeps[i].speed;
      double dz = sin(jeeps[i].dirAngle) * jeeps[i].speed;
      jeeps[i].x += dx;
      jeeps[i].z += dz;
      // Maintain facing aligned with movement direction
      jeeps[i].facing = jeeps[i].dirAngle * 180.0 / M_PI;
      
      // Keep within terrain bounds
      if (jeeps[i].x > 250.0) jeeps[i].x = -250.0;
      if (jeeps[i].x < -250.0) jeeps[i].x = 250.0;
      if (jeeps[i].z > 250.0) jeeps[i].z = -250.0;
      if (jeeps[i].z < -250.0) jeeps[i].z = 250.0;
      
      // Update Y position to stay on terrain
      jeeps[i].y = GetTerrainHeight(jeeps[i].x, jeeps[i].z) + 0.8;  // Added 0.8 offset
   }
}

/*
 * Drawing a single asteroid with irregular rocky shape
 */
void DrawAsteroid(float x, float y, float z, float size, float rotX, float rotY, float rotZ)
{
   // Asteroid material, dark grey and brown
   float color[] = {0.4, 0.35, 0.3, 1.0};
   float spec[] = {0.05, 0.05, 0.05, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 5.0);
   
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(rotX, 1, 0, 0);
   glRotated(rotY, 0, 1, 0);
   glRotated(rotZ, 0, 0, 1);
   glScaled(size, size, size);
   
   // Drawing irregular asteroid shape, like an icosphere
   glColor3f(0.4, 0.35, 0.3);
   for (int lat = -90; lat < 90; lat += 45)
   {
      glBegin(GL_QUAD_STRIP);
      for (int lon = 0; lon <= 360; lon += 45)
      {
         // Make it irregular by varying radius
         double r1 = 1.0 + 0.2 * sin(lon * 3.0 * M_PI / 180.0) * cos(lat * 2.0 * M_PI / 180.0);
         double r2 = 1.0 + 0.2 * sin(lon * 3.0 * M_PI / 180.0) * cos((lat + 45) * 2.0 * M_PI / 180.0);
         
         double x1 = r1 * Cos(lon) * Cos(lat);
         double y1 = r1 * Sin(lat);
         double z1 = r1 * Sin(lon) * Cos(lat);
         
         double x2 = r2 * Cos(lon) * Cos(lat + 45);
         double y2 = r2 * Sin(lat + 45);
         double z2 = r2 * Sin(lon) * Cos(lat + 45);
         
         glNormal3d(x1, y1, z1);
         glVertex3d(x1, y1, z1);
         glNormal3d(x2, y2, z2);
         glVertex3d(x2, y2, z2);
      }
      glEnd();
   }
   
   glPopMatrix();
}

/*
 * Drawing all asteroids
 */
void DrawAsteroids()
{
   glEnable(GL_LIGHTING);
   for (int i = 0; i < NUM_ASTEROIDS; i++)
   {
      DrawAsteroid(asteroids[i].x, asteroids[i].y, asteroids[i].z, 
                   asteroids[i].size, asteroids[i].rotX, asteroids[i].rotY, asteroids[i].rotZ);
   }
}

/*
 * Drawing starfield
 */
void DrawStarfield()
{
   glDisable(GL_LIGHTING);
   glColor3f(1, 1, 1);
   
   for (int i = 0; i < NUM_STARS; i++)
   {
      glPointSize(stars[i].size);
      glColor3f(stars[i].brightness, stars[i].brightness, stars[i].brightness);
      
      glBegin(GL_POINTS);
      glVertex3f(stars[i].x, stars[i].y, stars[i].z);
      glEnd();
   }
}

/*
 * Drawing vertex in polar coordinates for sphere
 */
static void Vertex(int th, int ph)
{
   double x = Sin(th) * Cos(ph);
   double y = Cos(th) * Cos(ph);
   double z = Sin(ph);
   glNormal3d(x, y, z);
   glVertex3d(x, y, z);
}

/*
 * Drawing the giant sun with texture, positioned far away
 */
void DrawSun(double x, double y, double z, double radius)
{
   // Sun emitting its own light
   float emission[] = {1.0, 0.5, 0.2, 1.0};  // Orange glow
   float ambient[] = {1.0, 0.5, 0.2, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambient);
   
   // Enabling texture
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, sunTexture);
   glColor3f(1, 1, 1);
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(radius, radius, radius);
   
   // Drawing textured sphere
   for (int ph = -90; ph < 90; ph += 10)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += 10)
      {
         // Texture coordinates
         float s0 = (float)th / 360.0;
         float t0 = (float)(ph + 90) / 180.0;
         float t1 = (float)(ph + 100) / 180.0;
         
         glTexCoord2f(s0, t0);
         Vertex(th, ph);
         
         glTexCoord2f(s0, t1);
         Vertex(th, ph + 10);
      }
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
   
   // Resetting emission
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 * Setting up lunar terrain with irregular surface and craters
 */
void InitTerrain()
{
   srand(12345);
   
   // Generating base irregular terrain
   for (int i = 0; i < TERRAIN_SIZE; i++)
   {
      for (int j = 0; j < TERRAIN_SIZE; j++)
      {
         double x = (i - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         double z = (j - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         
         // Create irregular, rocky surface with varying heights
         terrainZ[i][j] = 
            2.0 * sin(x * 0.03) * cos(z * 0.04) +
            1.5 * cos(x * 0.05 + 1.2) * sin(z * 0.06 + 0.8) +
            1.0 * sin(x * 0.08) * sin(z * 0.07 + 1.5) +
            0.8 * cos(x * 0.12) * cos(z * 0.11) +
            0.3 * ((float)rand() / RAND_MAX - 0.5);  // Random variation
      }
   }
   
   // Adding craters, making depressions in the surface
   // Setting crater positions and sizes
   struct {double x, z, radius, depth;} craters[] = {
      {-15, 10, 8.0, 3.0},   // Large crater left
      {20, 15, 5.0, 2.0},    // Medium crater right
      {-5, -20, 6.0, 2.5},   // Medium crater
      {30, -10, 4.0, 1.5},   // Small crater
      {-25, -15, 5.5, 2.2},  // Medium crater
   };
   
   int numCraters = 5;
   
   for (int c = 0; c < numCraters; c++)
   {
      for (int i = 0; i < TERRAIN_SIZE; i++)
      {
         for (int j = 0; j < TERRAIN_SIZE; j++)
         {
            double x = (i - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
            double z = (j - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
            
            // Distance from crater center
            double dx = x - craters[c].x;
            double dz = z - craters[c].z;
            double dist = sqrt(dx*dx + dz*dz);
            
            // Apply crater depression with smooth falloff
            if (dist < craters[c].radius)
            {
               double factor = (craters[c].radius - dist) / craters[c].radius;
               // Smooth crater bowl shape
               double depression = craters[c].depth * factor * factor;
               terrainZ[i][j] -= depression;
               
               // Add rim elevation
               if (factor < 0.3)
               {
                  terrainZ[i][j] += craters[c].depth * 0.2 * (0.3 - factor) / 0.3;
               }
            }
         }
      }
   }
   
   // Computing normals
   for (int i = 0; i < TERRAIN_SIZE; i++)
   {
      for (int j = 0; j < TERRAIN_SIZE; j++)
      {
         // X component
         if (i == 0)
            terrainNormals[i][j][0] = terrainZ[i][j] - terrainZ[i+1][j];
         else if (i == TERRAIN_SIZE-1)
            terrainNormals[i][j][0] = terrainZ[i-1][j] - terrainZ[i][j];
         else
            terrainNormals[i][j][0] = (terrainZ[i-1][j] - terrainZ[i+1][j]) / 2.0;
         
         // Y component
         if (j == 0)
            terrainNormals[i][j][1] = terrainZ[i][j] - terrainZ[i][j+1];
         else if (j == TERRAIN_SIZE-1)
            terrainNormals[i][j][1] = terrainZ[i][j-1] - terrainZ[i][j];
         else
            terrainNormals[i][j][1] = (terrainZ[i][j-1] - terrainZ[i][j+1]) / 2.0;
         
         // Z component is grid spacing
         terrainNormals[i][j][2] = TERRAIN_SCALE;
      }
   }
}

/*
 * Getting terrain height at world position
 */
double GetTerrainHeight(double worldX, double worldZ)
{
   // Converting world coordinates to terrain grid coordinates
   double i = (worldX / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
   double j = (worldZ / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
   
   // Checking bounds
   if (i < 0 || i >= TERRAIN_SIZE - 1 || j < 0 || j >= TERRAIN_SIZE - 1)
      return 0.0;
   
   // Getting integer parts
   int i0 = (int)i;
   int j0 = (int)j;
   
   // Getting fractional parts
   double fi = i - i0;
   double fj = j - j0;
   
   // Doing bilinear interpolation
   double h00 = terrainZ[i0][j0];
   double h10 = terrainZ[i0+1][j0];
   double h01 = terrainZ[i0][j0+1];
   double h11 = terrainZ[i0+1][j0+1];
   
   double h0 = h00 * (1 - fi) + h10 * fi;
   double h1 = h01 * (1 - fi) + h11 * fi;
   
   return h0 * (1 - fj) + h1 * fj;
}

/*
 * Drawing terrain with textured surface
 */
void DrawTerrain()
{
   // Disabling lighting to show true texture colors
   glDisable(GL_LIGHTING);
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, moonTexture);
   glColor3f(1.0, 1.0, 1.0);  // Using full white to show true texture colors
   
   for (int i = 0; i < TERRAIN_SIZE - 1; i++)
   {
      glBegin(GL_TRIANGLE_STRIP);
      for (int j = 0; j < TERRAIN_SIZE; j++)
      {
         double x0 = (i - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         double x1 = (i + 1 - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         double z = (j - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         
         // No normals needed since lighting is disabled
         glTexCoord2f((float)i / TERRAIN_SIZE, (float)j / TERRAIN_SIZE);
         glVertex3d(x0, terrainZ[i][j], z);
         
         glTexCoord2f((float)(i+1) / TERRAIN_SIZE, (float)j / TERRAIN_SIZE);
         glVertex3d(x1, terrainZ[i+1][j], z);
      }
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
   
   // Re-enabling lighting for other objects
   glEnable(GL_LIGHTING);
}

// Rocks only, no other environment objects
#define NUM_ROCKS 70  // Raised from 50 to 70

typedef struct {
   double x, y, z;
   double size;
   double rotX, rotY, rotZ;
   int type;  // 0 is angular, 1 is rounded, 2 is tall
   int textureIndex;  // Which texture we're using
} Rock;

Rock rocks[NUM_ROCKS];

/*
 * Getting random float between min and max
 */
float RandFloat(float min, float max)
{
   return min + (float)rand() / RAND_MAX * (max - min);
}

/*
 * Setting up environment objects
 * Rocks scattered across the entire terrain
 */
void InitEnvironment()
{
   srand(54321);
   
   // ROCKS, 70 total with varying sizes from very big to big
   // Scattered across the entire terrain, -250 to +250 in X and Z
   for (int i = 0; i < NUM_ROCKS; i++)
   {
      // Generate random positions across the entire terrain
      rocks[i].x = RandFloat(-250.0, 250.0);
      rocks[i].z = RandFloat(-250.0, 250.0);
      rocks[i].y = GetTerrainHeight(rocks[i].x, rocks[i].z);
      
      // Size variation: very big (3.0-6.0) to big (1.5-3.0)
      // 30% very big, 70% big
      if (RandFloat(0, 1) < 0.3)
         rocks[i].size = RandFloat(3.0, 6.0);  // Very big
      else
         rocks[i].size = RandFloat(1.5, 3.0);  // Big
      
      rocks[i].rotX = RandFloat(0, 360);
      rocks[i].rotY = RandFloat(0, 360);
      rocks[i].rotZ = RandFloat(0, 360);
      rocks[i].type = 1 + (rand() % 2);  // Only types 1 and 2 (rounded and tall)
      
      // Assign texture based on type
      // if (rocks[i].type == 0)  // Angular rocks - COMMENTED OUT
      //    rocks[i].textureIndex = rand() % 4;  // Use moon2, moon11, rock, or wall
      if (rocks[i].type == 1)  // Rounded rocks
         rocks[i].textureIndex = rand() % 4;  // Use moon2, moon11, rock, or wall
      else  // Tall/cylindrical rocks
         rocks[i].textureIndex = 4 + (rand() % 2);  // Use rusty_car or rusty_plane
   }
}

/*
 * Drawing a rock with irregular shape and texture
 */
void DrawRock(double x, double y, double z, double size, 
              double rotX, double rotY, double rotZ, int type, int textureIndex)
{
   // Rock material, white to let texture show through
   float white[] = {1.0, 1.0, 1.0, 1.0};
   float spec[] = {0.1, 0.1, 0.1, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0);
   
   // Selecting and binding appropriate texture
   glEnable(GL_TEXTURE_2D);
   unsigned int textures[] = {rockTexture1, rockTexture2, rockTexture3, rockTexture4, rockTexture5, rockTexture6};
   glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);
   glColor3f(1, 1, 1);
   
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(rotX, 1, 0, 0);
   glRotated(rotY, 0, 1, 0);
   glRotated(rotZ, 0, 0, 1);
   glScaled(size, size, size);
   
   // Angular rock type removed, no sharp rocks
   /*
   if (type == 0)  // Angular rock
   {
      glBegin(GL_TRIANGLES);
      // Front faces
      glNormal3f(0, 0.7, 0.7);
      glTexCoord2f(0.5, 1.0); glVertex3f(0, 1, 0);
      glTexCoord2f(0.0, 0.0); glVertex3f(-0.7, 0, 0.7);
      glTexCoord2f(1.0, 0.0); glVertex3f(0.7, 0, 0.7);
      
      glNormal3f(0.7, 0.5, 0);
      glTexCoord2f(0.5, 1.0); glVertex3f(0, 1, 0);
      glTexCoord2f(0.0, 0.0); glVertex3f(0.7, 0, 0.7);
      glTexCoord2f(1.0, 0.0); glVertex3f(0.8, 0, -0.5);
      
      glNormal3f(-0.7, 0.5, 0);
      glTexCoord2f(0.5, 1.0); glVertex3f(0, 1, 0);
      glTexCoord2f(0.0, 0.0); glVertex3f(0.8, 0, -0.5);
      glTexCoord2f(1.0, 0.0); glVertex3f(-0.7, 0, 0.7);
      
      // Bottom
      glNormal3f(0, -1, 0);
      glTexCoord2f(0.0, 0.0); glVertex3f(-0.7, 0, 0.7);
      glTexCoord2f(1.0, 0.0); glVertex3f(0.8, 0, -0.5);
      glTexCoord2f(0.5, 1.0); glVertex3f(0.7, 0, 0.7);
      glEnd();
   }
   else */
   if (type == 1)  // Rounded rock
   {
      // Simplified icosphere-like rock
      for (int lat = -90; lat < 90; lat += 30)
      {
         glBegin(GL_QUAD_STRIP);
         for (int lon = 0; lon <= 360; lon += 30)
         {
            double x1 = 0.9 * Cos(lon) * Cos(lat);
            double y1 = 0.8 * Sin(lat);
            double z1 = 0.9 * Sin(lon) * Cos(lat);
            
            double x2 = 0.9 * Cos(lon) * Cos(lat + 30);
            double y2 = 0.8 * Sin(lat + 30);
            double z2 = 0.9 * Sin(lon) * Cos(lat + 30);
            
            glNormal3d(x1, y1, z1);
            glTexCoord2f((float)lon / 360.0, (float)(lat + 90) / 180.0);
            glVertex3d(x1, y1, z1);
            glNormal3d(x2, y2, z2);
            glTexCoord2f((float)lon / 360.0, (float)(lat + 120) / 180.0);
            glVertex3d(x2, y2, z2);
         }
         glEnd();
      }
   }
   else  // Tall rock (cylindrical)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += 30)
      {
         double scale = 1.0 - 0.3 * sin(th * M_PI / 180.0);
         glNormal3d(Cos(th), 0, Sin(th));
         glTexCoord2f((float)th / 360.0, 0.0);
         glVertex3d(scale * 0.5 * Cos(th), 0, scale * 0.5 * Sin(th));
         glTexCoord2f((float)th / 360.0, 1.0);
         glVertex3d(scale * 0.3 * Cos(th), 1.5, scale * 0.3 * Sin(th));
      }
      glEnd();
      
      // Top cap
      glBegin(GL_TRIANGLE_FAN);
      glNormal3f(0, 1, 0);
      glTexCoord2f(0.5, 0.5);
      glVertex3f(0, 1.5, 0);
      for (int th = 0; th <= 360; th += 30)
      {
         double scale = 1.0 - 0.3 * sin(th * M_PI / 180.0);
         glTexCoord2f(0.5 + 0.5 * Cos(th), 0.5 + 0.5 * Sin(th));
         glVertex3d(scale * 0.3 * Cos(th), 1.5, scale * 0.3 * Sin(th));
      }
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 * Setting up lighting
 */
void SetupLighting()
{
   if (!light)
   {
      glDisable(GL_LIGHTING);
      return;
   }
   
   // Getting sun position, animated if enabled
   // double sunAngle = animateSun ? sunTime * 10.0 : 45.0;
   // double sunX = 5.0 * Cos(sunAngle);
   // double sunY = 3.0;
   // double sunZ = 5.0 * Sin(sunAngle);
   
   // Light coming from the giant sun direction
   float lightPos[] = {-1.0, 0.5, -1.0, 0.0};  // Directional from sun
   float ambient[] = {0.2, 0.2, 0.25, 1.0};    // Slight blue ambient from space
   float diffuse[] = {1.0, 0.95, 0.9, 1.0};    // Warm sunlight
   float specular[] = {0.5, 0.5, 0.5, 1.0};
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_NORMALIZE);
   
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

/*
 * Setting up perspective projection
 */
void SetupProjection()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fov, asp, 0.1, 1000.0);
   glMatrixMode(GL_MODELVIEW);
}

/*
 * Setting up camera based on view mode
 */
void SetupCamera()
{
   glLoadIdentity();
   
   if (viewMode)  // First-person
   {
      // Calculate look direction
      double lookX = playerX + sin(playerYaw * M_PI / 180.0) * cos(playerPitch * M_PI / 180.0);
      double lookY = playerY + sin(playerPitch * M_PI / 180.0);
      double lookZ = playerZ + cos(playerYaw * M_PI / 180.0) * cos(playerPitch * M_PI / 180.0);
      
      gluLookAt(playerX, playerY, playerZ,
                lookX, lookY, lookZ,
                0, 1, 0);
   }
   else  // Third-person debug view
   {
      double camDist = 10.0;
      double camX = playerX - camDist * sin(playerYaw * M_PI / 180.0);
      double camY = playerY + 5.0;
      double camZ = playerZ - camDist * cos(playerYaw * M_PI / 180.0);
      
      gluLookAt(camX, camY, camZ,
                playerX, playerY, playerZ,
                0, 1, 0);
   }
}

/*
 * Updating player movement with smooth frame independent physics
 */
void UpdatePlayer(double dt)
{
   if (dt <= 0) return;
   
   // Clamping dt to prevent huge jumps
   if (dt > 0.1) dt = 0.1;
   
   double moveX = 0, moveZ = 0;
   
   // Converting yaw to radians
   double yawRad = playerYaw * M_PI / 180.0;
   
   // Calculating frame independent movement
   double frameSpeed = MOVE_SPEED * dt;
   
   // Forward and backward, standard FPS movement, W is forward, S is backward
   if (moveForward && !moveBackward)  // Making sure only one direction at a time
   {
      moveX += sin(yawRad) * frameSpeed;
      moveZ += cos(yawRad) * frameSpeed;
   }
   else if (moveBackward && !moveForward)  // Making sure only one direction at a time
   {
      moveX -= sin(yawRad) * frameSpeed;
      moveZ -= cos(yawRad) * frameSpeed;
   }
   
   // Left and right strafing, perpendicular to forward direction, A is left, D is right
   if (moveLeft && !moveRight)  // Making sure only one direction at a time
   {
      moveX += cos(yawRad) * frameSpeed;
      moveZ -= sin(yawRad) * frameSpeed;
   }
   else if (moveRight && !moveLeft)  // Making sure only one direction at a time
   {
      moveX -= cos(yawRad) * frameSpeed;
      moveZ += sin(yawRad) * frameSpeed;
   }
   
   // Applying movement
   playerX += moveX;
   playerZ += moveZ;
   
   // Updating height based on terrain
   double terrainHeight = GetTerrainHeight(playerX, playerZ);
   playerY = terrainHeight + playerHeight;
}

/*
 * Drawing simple player model for third person view
 */
void DrawPlayer()
{
   glPushMatrix();
   glTranslated(playerX, playerY - playerHeight * 0.5, playerZ);
   glRotated(playerYaw, 0, 1, 0);
   
   // Body, cyan for visibility
   glColor3f(0, 1, 1);
   float bodyColor[] = {0.0, 1.0, 1.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bodyColor);
   
   // Drawing torso
   glPushMatrix();
   glScaled(0.3, 0.6, 0.2);
   glutSolidCube(1.0);
   glPopMatrix();
   
   // Drawing head
   glPushMatrix();
   glTranslated(0, playerHeight / 2, 0);
   glutSolidSphere(0.15, 12, 12);
   glPopMatrix();
   
   // Drawing direction indicator
   glDisable(GL_LIGHTING);
   glColor3f(1, 0, 0);
   glBegin(GL_LINES);
   glVertex3d(0, playerHeight/2, 0);
   glVertex3d(sin(playerYaw * M_PI/180) * 0.5, playerHeight/2, cos(playerYaw * M_PI/180) * 0.5);
   glEnd();
   
   glPopMatrix();
}

/*
 * Main display loop
 */
void display()
{
   // Clearing with black space background
   glClearColor(0.0, 0.0, 0.0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   
   SetupProjection();

   // SHOWCASE MODE CAMERA
   if (showcaseMode)
   {
      // Fixed camera position looking at origin
      glLoadIdentity();
      double camDist = 8.0;
      double camAngle = glutGet(GLUT_ELAPSED_TIME) / 3000.0;  // Slow rotation
      double camX = camDist * sin(camAngle);
      double camZ = camDist * cos(camAngle);
      gluLookAt(camX, 3.0, camZ,  // Camera position (rotating)
                0, 0, 0,           // Look at origin
                0, 1, 0);          // Up vector
   }
   else
   {
      // Normal game camera
      SetupCamera();
   }
   
   // *** DRAW BASED ON MODE ***
   if (showcaseMode)
   {
      // Showcase mode - only draw current object at origin
      glEnable(GL_LIGHTING);
      SetupLighting();
      
      // Draw axes for reference
      glDisable(GL_LIGHTING);
      glColor3f(1, 1, 1);
      glBegin(GL_LINES);
      glVertex3d(0, 0, 0); glVertex3d(3, 0, 0);
      glVertex3d(0, 0, 0); glVertex3d(0, 3, 0);
      glVertex3d(0, 0, 0); glVertex3d(0, 0, 3);
      glEnd();
      glRasterPos3d(3, 0, 0); Print("X");
      glRasterPos3d(0, 3, 0); Print("Y");
      glRasterPos3d(0, 0, 3); Print("Z");
      
      // Enable lighting for object
      glEnable(GL_LIGHTING);
      
      double time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
      double cameraAngle = atan2(8.0 * cos(time/3.0), 8.0 * sin(time/3.0)) * 180.0 / M_PI;
      
      // Draw the selected showcase object
      if (showcaseObject == 0)
      {
         // Floating Crystal Shard
         FloatingCrystalShard(0, 0, 0, 2.0, time, 42, 1, cameraAngle);
      }
      else if (showcaseObject == 1)
      {
         // Alien Scout Drone
         AlienScoutDrone(0, 0, 0, time, 0, 1.0, 1);
      }
      else if (showcaseObject == 2)
      {
         // Bioluminescent Spore Cloud (if you have it)
         // BioluminescentSporeCloud(0, 0, 0, 1.5, sporeCloud, time);
         
         // TEMPORARY - draw a placeholder until spore cloud is integrated
         glColor3f(0.25, 1.0, 0.22);
         glutSolidSphere(1.0, 20, 20);
         glWindowPos2i(5, 100);
         Print("Bioluminescent Spore Cloud - PLACEHOLDER");
      }
      else if (showcaseObject == 3)
      {
         // Geometric Alien Flora
         
      // GeometricAlienFlora(5.0, 0, 5.0, 1.5, time, 0, 42);

      }
      else if (showcaseObject == 4)
      {
         // Additional objects can be added here
         LunarisVex(0, 0, 0, 0, time, EMOTION_CALM, 0);

      }
      else if (showcaseObject == 5)
      {
         // Additional objects can be added here
         LunarisVex(0, 0, 0, 0, time, EMOTION_ALERT, 0);

      }
      else if (showcaseObject == 6)
      {
         // Additional objects can be added here
         LunarisVex(0, 0, 0, 0, time, EMOTION_CURIOUS, 0);

      }
      else if (showcaseObject == 7)
      {
         // Hovering with tractor beam on
         UFOSpacecraft(0, 15.0, -20.0, 2.5, time, 15.0, 1);


      }
      else if (showcaseObject == 8)
      {

         // Flying by without tractor beam
         UFOSpacecraft(20.0, 10.0, 5.0, 1.8, time, 30.0, 0);   
      }
      else if (showcaseObject == 9)
      {
         // Future object placeholder
         // Create alien garden scene 
         AlienFloraEggCluster(-10.0, 0, 5.0, 1.0, time); 
         AlienFloraAntlerBranch(-5.0, 0, 5.0, 1.2, time); 
         AlienFloraBubbleStack(0, 0, 5.0, 1.0, time); 
         AlienFloraCoralBranch(5.0, 0, 5.0, 1.1, time); 
         AlienFloraBladeFan(10.0, 0, 5.0, 1.0, time); 
         AlienFloraMushroomCaps(15.0, 0, 5.0, 1.0, time);
      }
      else if (showcaseObject == 10)
      {
         // Future object placeholder
         // Create alien garden scene 
         // Parked vehicle
         ArmoredTransportVehicle(10.0, 0.5, -5.0, 1.0, 45, time);

         // Moving vehicle
         ArmoredTransportVehicle(time * 2.0, 0.5, 10.0, 1.2, 90, time);
      }
      else if (showcaseObject == 11)
      {
         // Parked Jeep
         JeepWrangler(10.0, 0, -5.0, 1.0, 45, time);

         // Driving Jeep
         JeepWrangler(time * 3.0, 0, 10.0, 1.2, 90, time);
      }

      


      
      // HUD for showcase mode
      glDisable(GL_LIGHTING);
      glColor3f(1, 1, 0);
      glWindowPos2i(5, windowHeight - 25);
      Print("=== SHOWCASE MODE ===");
      glWindowPos2i(5, windowHeight - 45);
      Print("Press 'P' to cycle through objects");
      glWindowPos2i(5, windowHeight - 65);
      if (showcaseObject == 0)
         Print("Object 1/3: Floating Crystal Shard");
      else if (showcaseObject == 1)
         Print("Object 2/3: Alien Scout Drone");
      else if (showcaseObject == 2)
         Print("Object 3/3: Bioluminescent Spore Cloud");
      else if (showcaseObject == 3)
         Print("Object 4/4: Geometric Alien Flora");
      else if (showcaseObject == 4)
         Print("Object 5/5: Lunaris Vex - Type-1");
      else if (showcaseObject == 5)
         Print("Object 6/6: Lunaris Vex - Type-2");
      else if (showcaseObject == 6)
         Print("Object 7/7: Lunaris Vex - Type-3");
   }
   else
   {
      // NORMAL GAME MODE - draw everything
      
      // Draw starfield (no lighting)
      DrawStarfield();
      
      // Draw asteroids (moving in the sky)
      DrawAsteroids();
      
      // Draw the giant sun
      glDisable(GL_LIGHTING);
      DrawSun(-300, 120, -400, 150);
      
      // Enable lighting for terrain
      SetupLighting();
      
      // Draw terrain with craters
      DrawTerrain();
      
      // Draw rocks
      for (int i = 0; i < NUM_ROCKS; i++)
      {
         DrawRock(rocks[i].x, rocks[i].y, rocks[i].z, rocks[i].size,
                  rocks[i].rotX, rocks[i].rotY, rocks[i].rotZ, rocks[i].type, rocks[i].textureIndex);
      }

      // Draw objects
      double time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
      double cameraAngle = playerYaw;
      
      FloatingCrystalShard(5.0, 2.0, 3.0, 1.0, time, 42, 1, cameraAngle);
   // Draw one demo drone plus the extra swarm
   AlienScoutDrone(10.0, 5.0, 0.0, time, 0, 0.5, 1);
   for(int d=0; d<NUM_EXTRA_DRONES; ++d)
   {
      AlienScoutDrone(extraDrones[d].x, extraDrones[d].y, extraDrones[d].z,
             time, extraDrones[d].seed, extraDrones[d].scale, 1);
   }
      
      // Draw player in third-person mode
      if (!viewMode)
      {
         glEnable(GL_LIGHTING);
         DrawPlayer();
      }
      
      // Draw axes if enabled
      if (axes)
      {
         glDisable(GL_LIGHTING);
         glColor3f(1, 1, 1);
         glBegin(GL_LINES);
         glVertex3d(0, 0, 0); glVertex3d(5, 0, 0);
         glVertex3d(0, 0, 0); glVertex3d(0, 5, 0);
         glVertex3d(0, 0, 0); glVertex3d(0, 0, 5);
         glEnd();
         glRasterPos3d(5, 0, 0); Print("X");
         glRasterPos3d(0, 5, 0); Print("Y");
         glRasterPos3d(0, 0, 5); Print("Z");
      }
      
      // Normal game HUD
      glDisable(GL_LIGHTING);
      glColor3f(1, 1, 1);
      glWindowPos2i(5, 5);
      Print("Press 'P' for Object Showcase Mode");
   }















   SetupCamera();
   
   // Draw starfield (no lighting)
   DrawStarfield();
   
   // Draw asteroids (moving in the sky)
   DrawAsteroids();
   
   // Draw the giant sun (with self-illumination)
   // Positioned far to the left, much larger, outside moon plane
   glDisable(GL_LIGHTING);
   DrawSun(-300, 120, -400, 150);  // Much bigger (150 radius) and farther away
   
   // Enable lighting for terrain
   SetupLighting();
   
   // Draw terrain with craters
   DrawTerrain();
   
   // Draw rocks - 70 rocks scattered across the surface
   for (int i = 0; i < NUM_ROCKS; i++)
   {
      DrawRock(rocks[i].x, rocks[i].y, rocks[i].z, rocks[i].size,
               rocks[i].rotX, rocks[i].rotY, rocks[i].rotZ, rocks[i].type, rocks[i].textureIndex);
   }

      // animation time
   double time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
   
   // Calculate camera angle for volumetric lighting effect
   double cameraAngle = playerYaw;  // Use player's yaw angle
   
   // OPTION A: Draw a single test crystal
   // (Removed single test crystal; replaced by distributed crystal-UFO absorption pairs below)
   // Draw extra scout drones (12) wandering
   for(int d=0; d<NUM_EXTRA_DRONES; ++d)
   {
      AlienScoutDrone(extraDrones[d].x, extraDrones[d].y, extraDrones[d].z,
                      time, extraDrones[d].seed, extraDrones[d].scale, 1);
   }
   // OPTION C: Draw bioluminescent spore cloud
   // BioluminescentSporeCloud(2.0, 1.5, 0, 1.0, sporeCloud1, glutGet(GLUT_ELAPSED_TIME)/1000.0);
   // Single plant - violet variant
   // GeometricAlienFlora(5.0, 0, 5.0, 1.5, time, 0, 42);

   // Single plant - jade variant  
   // GeometricAlienFlora(-8.0, 0, 3.0, 2.0, time, 1, 123);

   // Cluster of 7 plants
   // GeometricAlienFloraCluster(15.0, 0, -10.0, 3.0, 7, time);
   
   // Draw all aliens across the map
   for (int i = 0; i < NUM_ALIENS; i++)
   {
      LunarisVex(aliens[i].x, aliens[i].y, aliens[i].z, 
                 aliens[i].facing, time, aliens[i].emotion, aliens[i].isGliding);
   }

   // Draw additional wandering scout drones
   for (int i = 0; i < NUM_EXTRA_DRONES; i++)
   {
      AlienScoutDrone(extraDrones[i].x, extraDrones[i].y, extraDrones[i].z,
                      time, extraDrones[i].facing, extraDrones[i].speed, 0);
   }

   // Draw all UFO spacecrafts
   // (Original random UFOs disabled for absorption pairs)

   // Draw 5 crystal + UFO absorption pairs dispersed over terrain
   for (int i = 0; i < NUM_CRYSTALS; i++)
   {
      // Draw crystal shard
      FloatingCrystalShard(crystals[i].x, crystals[i].y, crystals[i].z,
                           crystals[i].scale, time, crystals[i].seed, 1, cameraAngle);

      // Position UFO directly above crystal so tractor beam reaches crystal
      double ufoBottomOffset = 0.5; // matches internal UFO beam origin translation
      double desiredBeamLength = 8.0; // tractor beam vertical span
      double ufoY = crystals[i].y + desiredBeamLength + ufoBottomOffset;
      double ufoScale = 1.8; // consistent scale
      double rotationSpeed = 25.0 + (crystals[i].seed % 30); // varied rotation

      // Draw UFO with tractor beam active (absorbing)
      UFOSpacecraft(crystals[i].x, ufoY, crystals[i].z,
                    ufoScale, time, rotationSpeed, 1);

      // Add focused absorption beam visual connecting crystal to UFO bottom
      DrawAbsorptionBeam(crystals[i].x, crystals[i].y, crystals[i].z,
                         ufoY - ufoBottomOffset, time);
   }

   // Draw all Jeep Wranglers
   for (int i = 0; i < NUM_JEEPS; i++)
   {
      JeepWrangler(jeeps[i].x, jeeps[i].y, jeeps[i].z, 
                   jeeps[i].scale, jeeps[i].facing, time);
   }

   // Draw all flora instances dispersed across terrain
   unsigned int floraTextures[] = {floraTexture1, floraTexture2, floraTexture3, mossTexture};
   for (int i = 0; i < NUM_FLORA; i++)
   {
      // Enable texturing and bind the appropriate texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, floraTextures[flora[i].textureIndex]);
      
      // Draw based on type
      if (flora[i].type == 0)
         AlienFloraEggCluster(flora[i].x, flora[i].y, flora[i].z, flora[i].scale, time);
      else if (flora[i].type == 1)
         AlienFloraAntlerBranch(flora[i].x, flora[i].y, flora[i].z, flora[i].scale, time);
      else if (flora[i].type == 2)
         AlienFloraBubbleStack(flora[i].x, flora[i].y, flora[i].z, flora[i].scale, time);
      else if (flora[i].type == 3)
         AlienFloraCoralBranch(flora[i].x, flora[i].y, flora[i].z, flora[i].scale, time);
      else if (flora[i].type == 4)
         AlienFloraBladeFan(flora[i].x, flora[i].y, flora[i].z, flora[i].scale, time);
      else if (flora[i].type == 5)
         AlienFloraMushroomCaps(flora[i].x, flora[i].y, flora[i].z, flora[i].scale, time);
      
      glDisable(GL_TEXTURE_2D);
   }

   // Draw armored transport vehicles in front of player
   for (int i = 0; i < NUM_ARMORED_VEHICLES; i++)
   {
      ArmoredTransportVehicle(armoredVehicles[i].x, armoredVehicles[i].y, armoredVehicles[i].z,
                              armoredVehicles[i].scale, armoredVehicles[i].facing, time);
   }

   // Draw flying armored vehicles patrolling the sky
   for (int i = 0; i < NUM_FLYING_ARMORED; i++)
   {
      ArmoredTransportVehicle(flyingArmored[i].x, flyingArmored[i].y, flyingArmored[i].z,
                              flyingArmored[i].scale, flyingArmored[i].facing, time);
   }






   // Draw player in third-person mode
   if (!viewMode)
   {
      glEnable(GL_LIGHTING);
      DrawPlayer();
   }
   
   // Draw axes for debugging
   if (axes)
   {
      glDisable(GL_LIGHTING);
      glColor3f(1, 1, 1);
      glBegin(GL_LINES);
      glVertex3d(0, 0, 0); glVertex3d(5, 0, 0);
      glVertex3d(0, 0, 0); glVertex3d(0, 5, 0);
      glVertex3d(0, 0, 0); glVertex3d(0, 0, 5);
      glEnd();
      glRasterPos3d(5, 0, 0); Print("X");
      glRasterPos3d(0, 5, 0); Print("Y");
      glRasterPos3d(0, 0, 5); Print("Z");
   }
   
   // HUD
   glDisable(GL_LIGHTING);
   glColor3f(1, 1, 1);
   glWindowPos2i(5, 5);
//    Print("Position: (%.1f, %.1f, %.1f) Yaw: %.1f Pitch: %.1f", 
//          playerX, playerY, playerZ, playerYaw, playerPitch);
//    glWindowPos2i(5, 25);
//    Print("View: %s | Mouse: %s | Light: %s | Sun Anim: %s",
//          viewMode ? "FPP" : "Third-Person", 
//          mouseLook ? "ON" : "OFF", 
//          light ? "ON" : "OFF",
//          animateSun ? "ON" : "OFF");
//    glWindowPos2i(5, 45);
//    Print("WASD:Move | Mouse:Look | V:View | M:Mouse | L:Light | X:Axes | T:Sun");
//    glWindowPos2i(5, 65);
//    Print("Rocks: %d (Rounded & Cylindrical) | Asteroids: %d (Falling)", NUM_ROCKS, NUM_ASTEROIDS);
   
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 * Idle callback
 */
void idle()
{
   static int lastTime = 0;
   int currentTime = glutGet(GLUT_ELAPSED_TIME);
   double dt = (currentTime - lastTime) / 1000.0;
   double time = currentTime / 1000.0;
   lastTime = currentTime;
   
   if (dt > 0.1) dt = 0.1;
   
   UpdatePlayer(dt);
   UpdateAsteroids();  // Update asteroid positions
   UpdateAliens(time);  // Update alien positions and movement
   UpdateJeeps(time);  // Update jeep positions and movement
   UpdateArmoredVehicles(time); // Update armored vehicle movement
   UpdateFlyingArmoredVehicles(time); // Update flying armored vehicles
   UpdateExtraDrones(time); // Update extra scout drones wandering
   
   if (animateSun)
      sunTime = currentTime / 1000.0;
   
   glutPostRedisplay();
}

/*
 * Keyboard input handler
 */
void keyboard(unsigned char key, int x, int y)
{
   if (key == 27)
      exit(0);
   else if (key == 'w' || key == 'W')
      moveForward = true;
   else if (key == 's' || key == 'S')
      moveBackward = true;
   else if (key == 'a' || key == 'A')
      moveLeft = true;
   else if (key == 'd' || key == 'D')
      moveRight = true;
   else if (key == 'v' || key == 'V')
   {
      viewMode = 1 - viewMode;
      // Reset movement when switching views
      moveForward = moveBackward = moveLeft = moveRight = false;
   }
   else if (key == 'm' || key == 'M')
   {
      mouseLook = 1 - mouseLook;
      if (mouseLook)
         glutSetCursor(GLUT_CURSOR_NONE);
      else
         glutSetCursor(GLUT_CURSOR_INHERIT);
   }
   else if (key == 'l' || key == 'L')
      light = 1 - light;
   else if (key == 'x' || key == 'X')
      axes = 1 - axes;
   else if (key == 't' || key == 'T')
      animateSun = 1 - animateSun;

      
   else if (key == 'p' || key == 'P')
   {
      if (!showcaseMode) {
         // Enter showcase mode
         showcaseMode = 1;
         showcaseObject = 0;
         printf("SHOWCASE MODE: Object %d - Floating Crystal Shard\n", showcaseObject);
      } else {
         // Cycle to next object
         showcaseObject = (showcaseObject + 1) % NUM_SHOWCASE_OBJECTS;
         if (showcaseObject == 0) {
            printf("SHOWCASE MODE: Object %d - Floating Crystal Shard\n", showcaseObject);
         } else if (showcaseObject == 1) {
            printf("SHOWCASE MODE: Object %d - Alien Scout Drone\n", showcaseObject);
         } else if (showcaseObject == 2) {
            printf("SHOWCASE MODE: Object %d - Bioluminescent Spore Cloud\n", showcaseObject);
         }
      }
      
      // If we've cycled through all, exit showcase mode
      if (showcaseObject == 0 && showcaseMode) {
         // Second time hitting object 0 means exit
         static int pressCount = 0;
         pressCount++;
         if (pressCount > NUM_SHOWCASE_OBJECTS) {
            showcaseMode = 0;
            pressCount = 0;
            printf("Exited showcase mode - returning to normal game\n");
         }
      }
   }
   
   
   glutPostRedisplay();
}

/*
 * Keyboard release handler
 */
void keyboardUp(unsigned char key, int x, int y)
{
   if (key == 'w' || key == 'W')
      moveForward = false;
   else if (key == 's' || key == 'S')
      moveBackward = false;
   else if (key == 'a' || key == 'A')
      moveLeft = false;
   else if (key == 'd' || key == 'D')
      moveRight = false;
}

/*
 * Mouse motion handler for looking around
 */
void mouseMotion(int x, int y)
{
   if (!mouseLook) return;
   
   if (firstMouse)
   {
      lastMouseX = x;
      lastMouseY = y;
      firstMouse = false;
      return;
   }
   
   int deltaX = x - lastMouseX;
   int deltaY = y - lastMouseY;
   
   // Ignoring huge jumps
   if (abs(deltaX) > 200 || abs(deltaY) > 200)
   {
      lastMouseX = x;
      lastMouseY = y;
      return;
   }
   
   lastMouseX = x;
   lastMouseY = y;
   
   playerYaw += deltaX * mouseSensitivity;
   playerPitch -= deltaY * mouseSensitivity;
   
   // Clamping pitch
   if (playerPitch > 89.0) playerPitch = 89.0;
   if (playerPitch < -89.0) playerPitch = -89.0;
   
   // Wrapping yaw
   while (playerYaw > 360.0) playerYaw -= 360.0;
   while (playerYaw < 0.0) playerYaw += 360.0;
   
   // Re-centering if near edges
   if (x < 100 || x > windowWidth - 100 || y < 100 || y > windowHeight - 100)
   {
      int centerX = windowWidth / 2;
      int centerY = windowHeight / 2;
      glutWarpPointer(centerX, centerY);
      lastMouseX = centerX;
      lastMouseY = centerY;
   }
   
   glutPostRedisplay();
}

void passiveMouseMotion(int x, int y)
{
   mouseMotion(x, y);
}

/*
 * Window reshape handler
 */
void reshape(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
   asp = (height > 0) ? (double)width / height : 1;
   glViewport(0, 0, width, height);
   SetupProjection();
}

/*
 * Main entry point
 */
int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(windowWidth, windowHeight);
   glutCreateWindow("Lunar Outpost - Year 6060");
   
#ifdef USEGLEW
   if (glewInit() != GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   
   // Setting up scene
   InitTerrain();
   InitStarfield();
   InitAsteroids();  // Setting up moving asteroids
   InitEnvironment();  // Setting up only rocks now
   InitCrystals();     // Setting up crystal positions for absorption pairs
   InitAliens();  // Setting up wandering aliens
   InitUFOs();  // Setting up UFO spacecrafts
   InitJeeps();  // Setting up Jeep Wranglers
   InitFlora();  // Setting up flora across terrain
   // NOTE: Armored vehicles need playerYaw for forward placement
   // We're setting them up after playerYaw is set toward the sun
   InitExtraDrones(); // Setting up additional scout drones after base scene pieces
   
   // Loading textures
   moonTexture = LoadTexBMP("moon-textures/moon.bmp");
   sunTexture = LoadTexBMP("moon-textures/sun.bmp");
   rockTexture1 = LoadTexBMP("moon-textures/moon2.bmp");
   rockTexture2 = LoadTexBMP("moon-textures/moon11.bmp");
   rockTexture3 = LoadTexBMP("textures/rock.bmp");
   rockTexture4 = LoadTexBMP("textures/wall.bmp");
   rockTexture5 = LoadTexBMP("textures/rusty_car.bmp");
   rockTexture6 = LoadTexBMP("textures/rusty_plane.bmp");
   
   // Loading floral textures, skipping flora-2.bmp since it's 32-bit, not 24-bit
   floraTexture1 = LoadTexBMP("textures/floral/flora-1.bmp");
   floraTexture2 = LoadTexBMP("textures/floral/flora-3.bmp");  // Using flora-3 instead of flora-2
   floraTexture3 = LoadTexBMP("textures/floral/moss.bmp");
   mossTexture = LoadTexBMP("textures/floral/flora-1.bmp");  // Reusing flora-1 for 4th texture
   
   // Loading alien textures
   alienTexture1 = LoadTexBMP("textures/aliens/alien-type-1.bmp");
   alienTexture2 = LoadTexBMP("textures/aliens/alien-type-2.bmp");
   alienTexture3 = LoadTexBMP("textures/aliens/alien-type-3.bmp");
   
   // Setting player starting position
   playerY = GetTerrainHeight(playerX, playerZ) + playerHeight;
   // Making player face toward the sun at spawn
   playerYaw = FacingTowardSun(playerX, playerZ);
   // Now that playerYaw is known, setting up armored vehicles in front of player
   InitArmoredVehicles(); // Setting up two armored transports ahead of player, correct ordering
   InitFlyingArmoredVehicles(); // Setting up 10 flying patrol vehicles
   InitExtraDrones();     // Setting up 12 additional wandering scout drones

   // sporeCloud1 = CreateSporeCloud(0.6, 40);
   
   // Setting callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutKeyboardUpFunc(keyboardUp);
   glutMotionFunc(mouseMotion);
   glutPassiveMotionFunc(passiveMouseMotion);
   glutIdleFunc(idle);
   
   // Disabling key repeat to prevent sticky keys
   glutIgnoreKeyRepeat(1);
   
   glutSetCursor(GLUT_CURSOR_NONE);
   SetupProjection();
   
   Print("=== Lunar Outpost - Year 6060 ===\n");
   Print("Environment: %d textured rocks (rounded & cylindrical only)\n", NUM_ROCKS);
   Print("%d asteroids falling slowly through the sky\n", NUM_ASTEROIDS);
   Print("Lunar landscape with giant sun and craters loaded\n");
   Print("Controls: WASD=Move, Mouse=Look\n");
   Print("V=Toggle View, M=Mouse, L=Light, T=Animate Sun, X=Axes\n");
   
   ErrCheck("init");
   glutMainLoop();
   
   return 0;
}