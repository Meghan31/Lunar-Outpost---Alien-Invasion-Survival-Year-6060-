/*
 *  Bioluminescent Spore Cloud
 *  Realistic particle-based rendering with organic drift and pulsating glow
 */
#include "CSCIx229.h"

// Spore structure
typedef struct {
   float x, y, z;           // Position
   float vx, vy, vz;        // Drift velocity
   float size;              // Base size
   float phase;             // Animation phase offset
   float pulseSpeed;        // Individual pulse timing
} Spore;

// Cloud structure
typedef struct {
   Spore spores[50];        // Individual spores
   int count;               // Number of active spores
   float cloudRadius;       // Overall cloud size
   float globalPhase;       // Shared animation time
} SporeCloud;

/*
 *  Initialize a spore cloud
 */
static void InitSporeCloud(SporeCloud* cloud, float radius, int sporeCount)
{
   cloud->count = sporeCount;
   cloud->cloudRadius = radius;
   cloud->globalPhase = 0;
   
   for (int i = 0; i < sporeCount; i++)
   {
      // Random spherical distribution
      float theta = (rand() / (float)RAND_MAX) * 360.0;
      float phi = ((rand() / (float)RAND_MAX) - 0.5) * 180.0;
      float r = (rand() / (float)RAND_MAX) * radius;
      
      cloud->spores[i].x = r * Sin(theta) * Cos(phi);
      cloud->spores[i].y = r * Sin(phi);
      cloud->spores[i].z = r * Cos(theta) * Cos(phi);
      
      // Random drift velocity (very slow)
      cloud->spores[i].vx = ((rand() / (float)RAND_MAX) - 0.5) * 0.04;
      cloud->spores[i].vy = ((rand() / (float)RAND_MAX) - 0.5) * 0.04;
      cloud->spores[i].vz = ((rand() / (float)RAND_MAX) - 0.5) * 0.04;
      
      // Random size between 0.02 and 0.05
      cloud->spores[i].size = 0.02 + (rand() / (float)RAND_MAX) * 0.03;
      
      // Random phase offset for asynchronous pulsation
      cloud->spores[i].phase = (rand() / (float)RAND_MAX) * 6.28;
      
      // Random pulse speed variation
      cloud->spores[i].pulseSpeed = 0.8 + (rand() / (float)RAND_MAX) * 0.4;
   }
}

/*
 *  Update spore positions and keep them bounded
 */
static void UpdateSporeCloud(SporeCloud* cloud, float dt)
{
   cloud->globalPhase += dt * 3.49; // ~1.8 second cycle (2*PI/1.8)
   
   for (int i = 0; i < cloud->count; i++)
   {
      // Update position with drift
      cloud->spores[i].x += cloud->spores[i].vx * dt;
      cloud->spores[i].y += cloud->spores[i].vy * dt;
      cloud->spores[i].z += cloud->spores[i].vz * dt;
      
      // Contain within cloud radius (soft boundary)
      float dist = sqrt(cloud->spores[i].x * cloud->spores[i].x + 
                       cloud->spores[i].y * cloud->spores[i].y + 
                       cloud->spores[i].z * cloud->spores[i].z);
      
      if (dist > cloud->cloudRadius)
      {
         // Gentle bounce back toward center
         float factor = 0.02 * dt;
         cloud->spores[i].vx -= cloud->spores[i].x / dist * factor;
         cloud->spores[i].vy -= cloud->spores[i].y / dist * factor;
         cloud->spores[i].vz -= cloud->spores[i].z / dist * factor;
      }
      
      // Add slight random turbulence
      if (rand() % 100 < 3)
      {
         cloud->spores[i].vx += ((rand() / (float)RAND_MAX) - 0.5) * 0.01;
         cloud->spores[i].vy += ((rand() / (float)RAND_MAX) - 0.5) * 0.01;
         cloud->spores[i].vz += ((rand() / (float)RAND_MAX) - 0.5) * 0.01;
      }
      
      // Velocity damping to prevent runaway
      cloud->spores[i].vx *= 0.98;
      cloud->spores[i].vy *= 0.98;
      cloud->spores[i].vz *= 0.98;
   }
}

/*
 *  Draw a single billboarded spore
 */
static void DrawBillboardedSpore(float x, float y, float z, float size, 
                                 float brightness, float colorMix)
{
   // Get modelview matrix to extract camera right and up vectors
   float mv[16];
   glGetFloatv(GL_MODELVIEW_MATRIX, mv);
   
   // Camera right vector (billboard X axis)
   float rx = mv[0], ry = mv[4], rz = mv[8];
   // Camera up vector (billboard Y axis)  
   float ux = mv[1], uy = mv[5], uz = mv[9];
   
   // Color interpolation: green-yellow to gold based on colorMix
   float r = 0.25 + colorMix * 0.65;  // 0.25 -> 0.90
   float g = 1.0 - colorMix * 0.05;   // 1.0 -> 0.95
   float b = 0.22 + colorMix * 0.18;  // 0.22 -> 0.40
   
   // Set emission for glow
   float emission[] = {r * brightness * 0.85, g * brightness * 1.0, b * brightness * 0.45, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   
   // Set color with transparency
   float alpha = 0.35 + brightness * 0.25; // 0.35 to 0.60
   glColor4f(r, g, b, alpha);
   
   // Draw billboarded quad
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1); // Billboard always faces camera
   
   float s = size * (0.5 + brightness * 0.3); // Size varies with brightness
   
   glVertex3f(x - rx*s - ux*s, y - ry*s - uy*s, z - rz*s - uz*s);
   glVertex3f(x + rx*s - ux*s, y + ry*s - uy*s, z + rz*s - uz*s);
   glVertex3f(x + rx*s + ux*s, y + ry*s + uy*s, z + rz*s + uz*s);
   glVertex3f(x - rx*s + ux*s, y - ry*s + uy*s, z - rz*s + uz*s);
   
   glEnd();
}

/*
 *  Draw the entire spore cloud with volumetric halo
 */
void BioluminescentSporeCloud(double x, double y, double z, 
                              double size, 
                              SporeCloud* cloud,
                              double time)
{
   if (!cloud) return;
   
   // Update animation
   UpdateSporeCloud(cloud, 0.016); // Assuming ~60fps
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(size, size, size);
   
   // Enable transparency and additive blending for glow effect
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for glow
   glDepthMask(GL_FALSE); // Don't write to depth buffer for transparent particles
   
   // Disable lighting for emission-based rendering
   int lightWasOn = glIsEnabled(GL_LIGHTING);
   glDisable(GL_LIGHTING);
   
   // First pass: Draw volumetric halo (outer mist)
   glColor4f(0.25, 1.0, 0.22, 0.08);
   for (int ring = 0; ring < 3; ring++)
   {
      float haloRadius = cloud->cloudRadius * (0.9 + ring * 0.15);
      float haloAlpha = 0.08 / (ring + 1);
      glColor4f(0.25, 1.0, 0.22, haloAlpha);
      
      glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0, 0, 0);
      for (int i = 0; i <= 16; i++)
      {
         float angle = i * 22.5;
         glVertex3f(haloRadius * Cos(angle), haloRadius * Sin(angle), 0);
      }
      glEnd();
   }
   
   // Re-enable lighting for spores
   if (lightWasOn) glEnable(GL_LIGHTING);
   
   // Second pass: Draw individual spores with pulsation
   for (int i = 0; i < cloud->count; i++)
   {
      Spore* s = &cloud->spores[i];
      
      // Calculate pulsation: 0.8x to 1.25x size
      float pulsePhase = cloud->globalPhase * s->pulseSpeed + s->phase;
      float pulseFactor = 0.8 + 0.225 * (1.0 + sin(pulsePhase)); // 0.8 to 1.25
      
      // Brightness pulsation (for emission)
      float brightness = 0.6 + 0.4 * (1.0 + sin(pulsePhase)) / 2.0; // 0.6 to 1.0
      
      // Color shift toward gold at pulse peaks
      float colorMix = (brightness > 0.85) ? (brightness - 0.85) / 0.15 : 0.0;
      
      // Draw billboarded spore
      DrawBillboardedSpore(s->x, s->y, s->z, s->size * pulseFactor, 
                          brightness, colorMix);
   }
   
   // Draw motion streaks for fast-moving spores (optional enhancement)
   glDisable(GL_LIGHTING);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   for (int i = 0; i < cloud->count; i++)
   {
      Spore* s = &cloud->spores[i];
      float speed = sqrt(s->vx*s->vx + s->vy*s->vy + s->vz*s->vz);
      
      if (speed > 0.03) // Only draw streaks for faster spores
      {
         float streakLen = speed * 2.0;
         glColor4f(0.25, 1.0, 0.22, 0.15);
         glBegin(GL_LINES);
         glVertex3f(s->x, s->y, s->z);
         glVertex3f(s->x - s->vx * streakLen, 
                   s->y - s->vy * streakLen, 
                   s->z - s->vz * streakLen);
         glEnd();
      }
   }
   
   // Reset emission
   float black[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   
   // Restore render state
   glDepthMask(GL_TRUE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_BLEND);
   if (lightWasOn) glEnable(GL_LIGHTING);
   
   glPopMatrix();
}

/*
 *  Create and initialize a new spore cloud
 *  Call this once during initialization
 */
SporeCloud* CreateSporeCloud(float radius, int sporeCount)
{
   SporeCloud* cloud = (SporeCloud*)malloc(sizeof(SporeCloud));
   if (!cloud) return NULL;
   
   if (sporeCount < 20) sporeCount = 20;
   if (sporeCount > 50) sporeCount = 50;
   
   InitSporeCloud(cloud, radius, sporeCount);
   return cloud;
}

/*
 *  Free spore cloud memory
 */
void FreeSporeCloud(SporeCloud* cloud)
{
   if (cloud) free(cloud);
}
