/*
 *  Lunar Outpost - Alien Invasion Survival (Year 6060)
 *  
 *  A first-person survival game set on the Moon
 *  Featuring: Giant textured sun, starfield, irregular lunar terrain with craters
 *  
 *  Key bindings:
 *  WASD       Move forward/left/backward/right
 *  Mouse      Look around (FPP)
 *  v/V        Toggle FPP/Third-person debug view
 *  l/L        Toggle lighting
 *  m/M        Toggle mouse look
 *  t/T        Toggle sun animation
 *  x/X        Toggle axes
 *  ESC        Exit
 */

#include "CSCIx229.h"
#include <stdbool.h>
#include "Floatingcrystalshards.h"
#include "AlienScoutDrone.h"

// Window dimensions
int windowWidth = 1200;
int windowHeight = 800;

// Camera and view state
int axes = 0;           // Display axes for debugging
int light = 1;          // Lighting enabled
int viewMode = 1;       // 1=FPP, 0=Third-person debug
int mouseLook = 1;      // Mouse look enabled
double asp = 1.0;       // Aspect ratio
double fov = 70;        // Field of view (wider for dramatic effect)

// Player position and movement
double playerX = 0.0;
double playerY = 2.0;   // Height above ground
double playerZ = 30.0;  // Start further back to see scene
double playerHeight = 1.8;

// Player rotation (angles in degrees)
double playerYaw = 0.0;
double playerPitch = 0.0;

// Movement state
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;

// Mouse control
int lastMouseX = 0;
int lastMouseY = 0;
bool firstMouse = true;
double mouseSensitivity = 0.05;  // Smooth mouse control

// Physics constants
const double MOVE_SPEED = 0.2;     // Smooth movement speed
const double MOUSE_SMOOTH = 0.05;  // Mouse smoothing

// Sun/lighting animation
double sunTime = 0.0;
int animateSun = 0;  // Toggle sun animation

// Textures
unsigned int moonTexture;
unsigned int sunTexture;
unsigned int rockTexture1;  // moon2.bmp
unsigned int rockTexture2;  // moon11.bmp
unsigned int rockTexture3;  // rock.bmp
unsigned int rockTexture4;  // wall.bmp
unsigned int rockTexture5;  // rusty_car.bmp
unsigned int rockTexture6;  // rusty_plane.bmp

// Crystal positions
#define NUM_CRYSTALS 5
typedef struct {
   float x, y, z;
   float scale;
   int seed;
} Crystal;
Crystal crystals[NUM_CRYSTALS];

// Terrain data
#define TERRAIN_SIZE 65
#define TERRAIN_SCALE 8.0  // Larger scale for rolling dunes
float terrainZ[TERRAIN_SIZE][TERRAIN_SIZE];
float terrainNormals[TERRAIN_SIZE][TERRAIN_SIZE][3];

// Star positions (random, fixed)
#define NUM_STARS 20
typedef struct {
   float x, y, z;
   float size;
   float brightness;
} Star;
Star stars[NUM_STARS];

// Asteroids - moving objects in the sky
#define NUM_ASTEROIDS 15
typedef struct {
   float x, z;          // Horizontal position
   float y;             // Vertical position (height)
   float size;          // Size of asteroid
   float speed;         // Fall speed
   float rotX, rotY, rotZ;  // Rotation angles
   float rotSpeedX, rotSpeedY, rotSpeedZ;  // Rotation speeds
} Asteroid;
Asteroid asteroids[NUM_ASTEROIDS];

double GetTerrainHeight(double worldX, double worldZ);


/*
 * Initialize crystal positions across the map
 */
void InitCrystals()
{
   srand(99999);  // Different seed from asteroids/stars
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
 * Initialize starfield
 * Random positions in the sky, varying sizes
 */
void InitStarfield()
{
   srand(42);  // Fixed seed for consistent stars
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
 * Initialize asteroids - moving objects in the sky
 */
void InitAsteroids()
{
   srand(12345);  // Different seed from stars
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
 * Update asteroid positions (call in idle function)
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
 * Draw a single asteroid (irregular rocky shape)
 */
void DrawAsteroid(float x, float y, float z, float size, float rotX, float rotY, float rotZ)
{
   // Asteroid material - dark grey/brown
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
   
   // Draw irregular asteroid shape (icosphere-like)
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
 * Draw all asteroids
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
 * Draw starfield
 * Reference: ex19.c starfield rendering
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
 * Draw vertex in polar coordinates (for sphere)
 * Reference: ex19.c planet rendering
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
 * Draw the giant sun with texture - positioned far away
 * Reference: ex19.c planet sphere rendering
 */
void DrawSun(double x, double y, double z, double radius)
{
   // Sun emits its own light
   float emission[] = {1.0, 0.5, 0.2, 1.0};  // Orange glow
   float ambient[] = {1.0, 0.5, 0.2, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambient);
   
   // Enable texture
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, sunTexture);
   glColor3f(1, 1, 1);
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(radius, radius, radius);
   
   // Draw textured sphere
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
   
   // Reset emission
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 * Initialize lunar terrain with irregular surface and craters
 * Reference: ex18.c terrain generation
 */
void InitTerrain()
{
   srand(12345);
   
   // Generate base irregular terrain
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
   
   // Add craters (depressions in the surface)
   // Crater positions and sizes
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
   
   // Compute normals (reference: ex18.c normal calculation)
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
 * Get terrain height at world position
 */
double GetTerrainHeight(double worldX, double worldZ)
{
   // Convert world coordinates to terrain grid coordinates
   double i = (worldX / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
   double j = (worldZ / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
   
   // Check bounds
   if (i < 0 || i >= TERRAIN_SIZE - 1 || j < 0 || j >= TERRAIN_SIZE - 1)
      return 0.0;
   
   // Get integer parts
   int i0 = (int)i;
   int j0 = (int)j;
   
   // Get fractional parts
   double fi = i - i0;
   double fj = j - j0;
   
   // Bilinear interpolation
   double h00 = terrainZ[i0][j0];
   double h10 = terrainZ[i0+1][j0];
   double h01 = terrainZ[i0][j0+1];
   double h11 = terrainZ[i0+1][j0+1];
   
   double h0 = h00 * (1 - fi) + h10 * fi;
   double h1 = h01 * (1 - fi) + h11 * fi;
   
   return h0 * (1 - fj) + h1 * fj;
}

/*
 * Draw terrain with textured surface
 * Reference: ex18.c terrain rendering with normals
 */
void DrawTerrain()
{
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, moonTexture);
   glColor3f(0.8, 0.8, 0.8);  // Slight grey tint
   
   for (int i = 0; i < TERRAIN_SIZE - 1; i++)
   {
      glBegin(GL_TRIANGLE_STRIP);
      for (int j = 0; j < TERRAIN_SIZE; j++)
      {
         double x0 = (i - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         double x1 = (i + 1 - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         double z = (j - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         
         // Apply normals for proper lighting
         glNormal3fv(terrainNormals[i][j]);
         glTexCoord2f((float)i / TERRAIN_SIZE, (float)j / TERRAIN_SIZE);
         glVertex3d(x0, terrainZ[i][j], z);
         
         glNormal3fv(terrainNormals[i+1][j]);
         glTexCoord2f((float)(i+1) / TERRAIN_SIZE, (float)j / TERRAIN_SIZE);
         glVertex3d(x1, terrainZ[i+1][j], z);
      }
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
}

// Environmental objects - ROCKS ONLY
#define NUM_ROCKS 70  // Increased from 50 to 70

typedef struct {
   double x, y, z;
   double size;
   double rotX, rotY, rotZ;
   int type;  // 0=angular, 1=rounded, 2=tall
   int textureIndex;  // Which texture to use
} Rock;

Rock rocks[NUM_ROCKS];

/*
 * Random float between min and max
 */
float RandFloat(float min, float max)
{
   return min + (float)rand() / RAND_MAX * (max - min);
}

/*
 * Initialize environmental objects
 * Now only rocks - more dispersed around spawn area
 */
void InitEnvironment()
{
   srand(54321);
   
   // ROCKS - 70 rocks with varying sizes from very big to big
   // More concentrated around spawn area (0,0)
   for (int i = 0; i < NUM_ROCKS; i++)
   {
      // Generate positions with bias toward spawn area
      float angle = RandFloat(0, 360);
      float distance;
      
      // 40% within 20 units of spawn (close)
      // 35% between 20-50 units (medium)
      // 25% between 50-100 units (far)
      float rand_val = RandFloat(0, 1);
      if (rand_val < 0.4)
         distance = RandFloat(5, 20);
      else if (rand_val < 0.75)
         distance = RandFloat(20, 50);
      else
         distance = RandFloat(50, 100);
      
      rocks[i].x = distance * cos(angle * M_PI / 180.0);
      rocks[i].z = distance * sin(angle * M_PI / 180.0);
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
 * Draw a rock with irregular shape and texture
 * Reference: ex16.c for basic geometry
 */
void DrawRock(double x, double y, double z, double size, 
              double rotX, double rotY, double rotZ, int type, int textureIndex)
{
   // Rock material - white to let texture show through
   float white[] = {1.0, 1.0, 1.0, 1.0};
   float spec[] = {0.1, 0.1, 0.1, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0);
   
   // Select and bind appropriate texture
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
   
   // Angular rock type commented out - no sharp rocks
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
 * Setup lighting
 * Reference: ex17.c lighting setup
 */
void SetupLighting()
{
   if (!light)
   {
      glDisable(GL_LIGHTING);
      return;
   }
   
   // Calculate sun position (animated if enabled)
   double sunAngle = animateSun ? sunTime * 10.0 : 45.0;
   double sunX = 5.0 * Cos(sunAngle);
   double sunY = 3.0;
   double sunZ = 5.0 * Sin(sunAngle);
   
   // Light coming from the giant sun direction
   float lightPos[] = {-1.0, 0.5, -1.0, 0.0};  // Directional from sun
   float ambient[] = {0.2, 0.2, 0.25, 1.0};    // Slight blue ambient (space)
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
 * Setup perspective projection
 */
void SetupProjection()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fov, asp, 0.1, 1000.0);
   glMatrixMode(GL_MODELVIEW);
}

/*
 * Setup camera based on view mode
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
 * Update player movement with smooth physics
 */
void UpdatePlayer(double dt)
{
   if (dt <= 0) return;
   
   double moveX = 0, moveZ = 0;
   
   // Forward/backward
   if (moveForward)
   {
      moveX += sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
      moveZ += cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
   }
   if (moveBackward)
   {
      moveX -= sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
      moveZ -= cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
   }
   
   // Left/right strafing
   if (moveLeft)
   {
      moveX -= cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
      moveZ += sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
   }
   if (moveRight)
   {
      moveX += cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
      moveZ -= sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
   }
   
   // Apply movement
   playerX += moveX;
   playerZ += moveZ;
   
   // Update height based on terrain
   double terrainHeight = GetTerrainHeight(playerX, playerZ);
   playerY = terrainHeight + playerHeight;
}

/*
 * Draw simple player model (for third-person view)
 */
void DrawPlayer()
{
   glPushMatrix();
   glTranslated(playerX, playerY - playerHeight * 0.5, playerZ);
   glRotated(playerYaw, 0, 1, 0);
   
   // Body (cyan for visibility)
   glColor3f(0, 1, 1);
   float bodyColor[] = {0.0, 1.0, 1.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bodyColor);
   
   // Torso
   glPushMatrix();
   glScaled(0.3, 0.6, 0.2);
   glutSolidCube(1.0);
   glPopMatrix();
   
   // Head
   glPushMatrix();
   glTranslated(0, playerHeight / 2, 0);
   glutSolidSphere(0.15, 12, 12);
   glPopMatrix();
   
   // Direction indicator
   glDisable(GL_LIGHTING);
   glColor3f(1, 0, 0);
   glBegin(GL_LINES);
   glVertex3d(0, playerHeight/2, 0);
   glVertex3d(sin(playerYaw * M_PI/180) * 0.5, playerHeight/2, cos(playerYaw * M_PI/180) * 0.5);
   glEnd();
   
   glPopMatrix();
}

/*
 * Main display function
 */
void display()
{
   // Clear with black (space)
   glClearColor(0.0, 0.0, 0.0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   
   SetupProjection();
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
   FloatingCrystalShard(5.0, 2.0, 3.0, 1.0, time, 42, 1, cameraAngle);
   // OPTION B: Draw a scouting alien drone
   AlienScoutDrone(10.0, 5.0, 0.0, time, 0, 0.5, 1);





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
 * Idle function
 */
void idle()
{
   static int lastTime = 0;
   int currentTime = glutGet(GLUT_ELAPSED_TIME);
   double dt = (currentTime - lastTime) / 1000.0;
   lastTime = currentTime;
   
   if (dt > 0.1) dt = 0.1;
   
   UpdatePlayer(dt);
   UpdateAsteroids();  // Update asteroid positions
   
   if (animateSun)
      sunTime = currentTime / 1000.0;
   
   glutPostRedisplay();
}

/*
 * Keyboard input
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
      viewMode = 1 - viewMode;
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
   
   glutPostRedisplay();
}

/*
 * Keyboard release
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
 * Mouse motion for looking
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
   
   // Ignore huge jumps
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
   
   // Clamp pitch
   if (playerPitch > 89.0) playerPitch = 89.0;
   if (playerPitch < -89.0) playerPitch = -89.0;
   
   // Wrap yaw
   while (playerYaw > 360.0) playerYaw -= 360.0;
   while (playerYaw < 0.0) playerYaw += 360.0;
   
   // Re-center if near edges
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
 * Window reshape
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
 * Main
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
   
   // Initialize scene
   InitTerrain();
   InitStarfield();
   InitAsteroids();  // Initialize moving asteroids
   InitEnvironment();  // Initialize only rocks now
   
   // Load textures
   moonTexture = LoadTexBMP("moon-textures/moon.bmp");
   sunTexture = LoadTexBMP("moon-textures/sun.bmp");
   rockTexture1 = LoadTexBMP("moon-textures/moon2.bmp");
   rockTexture2 = LoadTexBMP("moon-textures/moon11.bmp");
   rockTexture3 = LoadTexBMP("textures/rock.bmp");
   rockTexture4 = LoadTexBMP("textures/wall.bmp");
   rockTexture5 = LoadTexBMP("textures/rusty_car.bmp");
   rockTexture6 = LoadTexBMP("textures/rusty_plane.bmp");
   
   // Set player starting position
   playerY = GetTerrainHeight(playerX, playerZ) + playerHeight;
   
   // Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutKeyboardUpFunc(keyboardUp);
   glutMotionFunc(mouseMotion);
   glutPassiveMotionFunc(passiveMouseMotion);
   glutIdleFunc(idle);
   
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