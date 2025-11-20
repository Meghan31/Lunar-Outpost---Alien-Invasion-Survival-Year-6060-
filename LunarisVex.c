/*
 * Lunaris Vex - moon alien character
 * Rendering bioluminescent alien with shaders and dust particles
 */

#include "CSCIx229.h"
#include "shader_loader.h"
#include <math.h>

// Tracking shader program handle
static GLuint lunarisShaderProgram = 0;

// External alien skin textures (loaded in main program)
extern unsigned int alienTexture1;  // Head and upper body
extern unsigned int alienTexture2;  // Arms and torso details
extern unsigned int alienTexture3;  // Legs and lower body

// Managing lunar dust particle system
#define MAX_DUST_PARTICLES 150
typedef struct {
   float x, y, z;
   float vx, vy, vz;
   float life;
   float size;
   float angle;
} DustParticle;

static DustParticle dustParticles[MAX_DUST_PARTICLES];
static int dustInitialized = 0;

// Defining character state structure fields
typedef struct {
   double x, y, z;
   double facing;       
   double idleTime;      
   int emotion;          
   double glowIntensity; 
   double glidePhase;    
} LunarisState;

/*
 * Initializing shader program
 */
static void InitLunarisShaders(void)
{
   if (lunarisShaderProgram == 0)
   {
      lunarisShaderProgram = CreateShaderProgram(
         "shaders/lunaris.vert",
         "shaders/lunaris.frag"
      );
   }
}

/*
 * Initializing dust particle system
 */
static void InitDustParticles(void)
{
   if (dustInitialized) return;
   
   for (int i = 0; i < MAX_DUST_PARTICLES; i++)
   {
      dustParticles[i].life = 0.0;
   }
   dustInitialized = 1;
}

/*
 * Emitting one dust particle
 */
static void EmitDustParticle(double cx, double cy, double cz, int emotion)
{
   for (int i = 0; i < MAX_DUST_PARTICLES; i++)
   {
      if (dustParticles[i].life <= 0.0)
      {
         // Random position around character
         float angle = (rand() % 360) * M_PI / 180.0;
         float radius = 0.3 + (rand() % 100) / 200.0;
         
         dustParticles[i].x = cx + radius * cos(angle);
         dustParticles[i].y = cy + (rand() % 100) / 100.0 - 0.2;
         dustParticles[i].z = cz + radius * sin(angle);
         
         // Upward spiral motion
         dustParticles[i].vx = -sin(angle) * 0.02;
         dustParticles[i].vy = 0.03 + (rand() % 50) / 1000.0;
         dustParticles[i].vz = cos(angle) * 0.02;
         
         dustParticles[i].life = 2.0 + (rand() % 100) / 50.0;
         dustParticles[i].size = 0.02 + (rand() % 50) / 1000.0;
         dustParticles[i].angle = angle;
         break;
      }
   }
}

/*
 * Updating dust particles
 */
static void UpdateDustParticles(float dt)
{
   for (int i = 0; i < MAX_DUST_PARTICLES; i++)
   {
      if (dustParticles[i].life > 0.0)
      {
         dustParticles[i].x += dustParticles[i].vx * dt;
         dustParticles[i].y += dustParticles[i].vy * dt;
         dustParticles[i].z += dustParticles[i].vz * dt;
         
         // Spiral motion
         dustParticles[i].angle += dt * 2.0;
         dustParticles[i].vx = -sin(dustParticles[i].angle) * 0.02;
         dustParticles[i].vz = cos(dustParticles[i].angle) * 0.02;
         
         dustParticles[i].life -= dt;
      }
   }
}

/*
 * Rendering dust particles with glow
 */
static void DrawDustParticles(int emotion, double glowIntensity)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glDepthMask(GL_FALSE);
   glDisable(GL_LIGHTING);
   
   for (int i = 0; i < MAX_DUST_PARTICLES; i++)
   {
      if (dustParticles[i].life > 0.0)
      {
         float alpha = dustParticles[i].life / 3.0;
         if (alpha > 1.0) alpha = 1.0;
         
         // Emotion-based dust color
         float r, g, b;
         if (emotion == 0) {  // Calm - silvery
            r = 0.9; g = 0.95; b = 1.0;
         } else if (emotion == 1) {  // Curious - cyan
            r = 0.5; g = 0.9; b = 1.0;
         } else if (emotion == 2) {  // Alert - violet
            r = 0.8; g = 0.6; b = 1.0;
         } else {  // Thinking - soft blue
            r = 0.7; g = 0.8; b = 0.95;
         }
         
         glColor4f(r * glowIntensity, g * glowIntensity, b * glowIntensity, alpha * 0.6);
         
         glPushMatrix();
         glTranslated(dustParticles[i].x, dustParticles[i].y, dustParticles[i].z);
         glScaled(dustParticles[i].size, dustParticles[i].size, dustParticles[i].size);
         
         // Billboarded quad
         glBegin(GL_QUADS);
         glVertex3f(-1, -1, 0);
         glVertex3f( 1, -1, 0);
         glVertex3f( 1,  1, 0);
         glVertex3f(-1,  1, 0);
         glEnd();
         
         glPopMatrix();
      }
   }
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
}

/*
 * Drawing smooth sphere for organic form
 */
/*
 * Drawing smooth sphere for organic form (duplicate helper)
 */
static void DrawSmoothSphere(double r, int detail)
{
   for (int i = 0; i < detail; i++)
   {
      double lat0 = M_PI * (-0.5 + (double)(i) / detail);
      double lat1 = M_PI * (-0.5 + (double)(i + 1) / detail);
      double z0 = sin(lat0);
      double zr0 = cos(lat0);
      double z1 = sin(lat1);
      double zr1 = cos(lat1);
      
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= detail; j++)
      {
         double lng = 2 * M_PI * (double)j / detail;
         double x = cos(lng);
         double y = sin(lng);
         
         // Texture coordinates for shader
         glTexCoord2f((float)j / detail, (float)i / detail);
         glNormal3d(x * zr0, z0, y * zr0);
         glVertex3d(r * x * zr0, r * z0, r * y * zr0);
         
         glTexCoord2f((float)j / detail, (float)(i + 1) / detail);
         glNormal3d(x * zr1, z1, y * zr1);
         glVertex3d(r * x * zr1, r * z1, r * y * zr1);
      }
      glEnd();
   }
}

/*
 * Drawing alien head with layered details
 */
static void DrawAlienHead(double time, int emotion, double glowIntensity)
{
   glPushMatrix();
   
   // Enabling skin texturing
   // Selecting texture based on emotion
   glEnable(GL_TEXTURE_2D);
   unsigned int headTexture;
   if (emotion == 0) headTexture = alienTexture1;      // Calm: texture 1
   else if (emotion == 1) headTexture = alienTexture2; // Curious: texture 2
   else if (emotion == 2) headTexture = alienTexture3; // Alert: texture 3
   else headTexture = alienTexture1;                   // Thinking: texture 1
   glBindTexture(GL_TEXTURE_2D, headTexture);
   
   // Setting base color so texture shows clearly
   float skinColor[] = {1.0, 1.0, 1.0, 1.0};
   glColor4fv(skinColor);
   
   // Drawing elongated cranium shape
   glPushMatrix();
   glTranslated(0, 0.1, -0.2);  // Shift back
   glRotated(-15, 1, 0, 0);     // Tilt back
   glScaled(0.8, 0.9, 2.2);     // VERY elongated backward
   DrawSmoothSphere(0.35, 48);
   glPopMatrix();
   
   // Drawing upper cranium ridges
   for (int ridge = 0; ridge < 3; ridge++)
   {
      glPushMatrix();
      glTranslated(0, 0.35 - ridge * 0.08, -0.1 - ridge * 0.15);
      glRotated(-10, 1, 0, 0);
      glScaled(0.55 - ridge * 0.05, 0.08, 1.2 + ridge * 0.2);
      DrawSmoothSphere(1.0, 32);
      glPopMatrix();
   }
   
   // Drawing face and jaw area
   glPushMatrix();
   glTranslated(0, -0.15, 0.42);
   glRotated(10, 1, 0, 0);
   glScaled(0.6, 0.7, 0.8);
   DrawSmoothSphere(0.28, 40);
   glPopMatrix();
   
   // Drawing lower jaw
   glPushMatrix();
   glTranslated(0, -0.42, 0.35);
   glRotated(20, 1, 0, 0);
   glScaled(0.5, 0.35, 0.7);
   DrawSmoothSphere(0.25, 36);
   glPopMatrix();
   
   // Drawing side jaw structures
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.22, -0.3, 0.38);
      glRotated(side * 25, 0, 1, 0);
      glRotated(15, 1, 0, 0);
      glScaled(0.18, 0.4, 0.5);
      DrawSmoothSphere(1.0, 28);
      glPopMatrix();
   }
   
   // Drawing temporal bone structures
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.32, 0.05, 0.1);
      glScaled(0.22, 0.28, 0.35);
      DrawSmoothSphere(1.0, 32);
      glPopMatrix();
   }
   
   // Drawing segmented neck connection
   glPushMatrix();
   glTranslated(0, -0.6, 0.05);
   
   // Iterating neck segments
   for (int seg = 0; seg < 3; seg++)
   {
      glPushMatrix();
      glTranslated(0, -seg * 0.1, 0);
      glRotated(90, 1, 0, 0);
      
      float segRadius = 0.2 - seg * 0.02;
      GLUquadric* quad = gluNewQuadric();
      gluQuadricNormals(quad, GLU_SMOOTH);
      gluQuadricTexture(quad, GL_TRUE);
      gluCylinder(quad, segRadius, segRadius * 0.95, 0.09, 28, 1);
      gluDeleteQuadric(quad);
      glPopMatrix();
      
   // Adding segment ring detail
      glPushMatrix();
      glTranslated(0, -seg * 0.1 - 0.04, 0);
      glScaled(1.1, 0.5, 1.1);
      DrawSmoothSphere(segRadius, 24);
      glPopMatrix();
   }
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 * Drawing eyes (glossy, no pupils)
 */
static void DrawIridescentEyes(double time, int emotion, double glowIntensity)
{
   // Enabling eye texturing based on emotion
   glEnable(GL_TEXTURE_2D);
   unsigned int eyeTexture;
   if (emotion == 0) eyeTexture = alienTexture1;      // Calm: texture 1
   else if (emotion == 1) eyeTexture = alienTexture2; // Curious: texture 2
   else if (emotion == 2) eyeTexture = alienTexture3; // Alert: texture 3
   else eyeTexture = alienTexture1;                   // Thinking: texture 1
   glBindTexture(GL_TEXTURE_2D, eyeTexture);
   
   // Positioning recessed eyes
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.2, -0.05, 0.46);
      glRotated(side * 20, 0, 1, 0);
      glRotated(10, 1, 0, 0);
      
   // Drawing eye socket recess
      glPushMatrix();
      glScaled(1.15, 1.25, 0.6);
      float socketColor[] = {0.8, 0.8, 0.8, 1.0};
      glColor4fv(socketColor);
      DrawSmoothSphere(0.12, 28);
      glPopMatrix();
      
   // Drawing eye globe
      glPushMatrix();
      glTranslated(0, 0, 0.04);
      glScaled(1.0, 1.15, 0.85);
      
      float eyeColor[] = {0.9, 0.9, 0.9, 1.0};
      glColor4fv(eyeColor);
      
      DrawSmoothSphere(0.11, 36);
      glPopMatrix();
      
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glDepthMask(GL_FALSE);
      
      glPushMatrix();
      glTranslated(0.03, 0.05, 0.1);
      glColor4f(0.5, 0.55, 0.6, 0.4);
      DrawSmoothSphere(0.03, 16);
      glPopMatrix();
      
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      glEnable(GL_LIGHTING);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, eyeTexture);
      
      glPopMatrix();
   }
   
   glDisable(GL_TEXTURE_2D);
}

/*
 * Drawing torso with ribbed exoskeleton
 */
static void DrawTorso(double glowIntensity, double time, int emotion)
{
   // Enabling torso texturing and selecting emotion texture
   glEnable(GL_TEXTURE_2D);
   unsigned int torsoTexture;
   if (emotion == 0) torsoTexture = alienTexture2;      // Calm: texture 2
   else if (emotion == 1) torsoTexture = alienTexture3; // Curious: texture 3
   else if (emotion == 2) torsoTexture = alienTexture1; // Alert: texture 1
   else torsoTexture = alienTexture2;                   // Thinking: texture 2
   glBindTexture(GL_TEXTURE_2D, torsoTexture);
   
   float skinColor[] = {1.0, 1.0, 1.0, 1.0};
   glColor4fv(skinColor);
   
   // Drawing upper chest
   glPushMatrix();
   glScaled(0.52, 0.75, 0.44);
   DrawSmoothSphere(1.0, 40);
   glPopMatrix();
   
   // Drawing spine ridge
   glPushMatrix();
   glTranslated(0, 0.2, -0.38);
   glScaled(0.1, 0.68, 0.2);
   DrawSmoothSphere(1.0, 28);
   glPopMatrix();
   
   // Drawing ribcage segments
   for (int rib = 0; rib < 8; rib++)
   {
      double ribY = 0.32 - rib * 0.11;
      double ribScale = 1.0 - rib * 0.06;
      
      for (int side = -1; side <= 1; side += 2)
      {
         glPushMatrix();
         glTranslated(side * 0.14, ribY, 0.12);
         glRotated(side * 45, 0, 1, 0);
         glRotated(12, 1, 0, 0);
         glScaled(0.35 * ribScale, 0.05, 0.22 * ribScale);
         DrawSmoothSphere(1.0, 24);
         glPopMatrix();
         
         // Rib tip protrusion
         glPushMatrix();
         glTranslated(side * 0.42 * ribScale, ribY - 0.02, 0.15);
         glScaled(0.07, 0.08, 0.07);
         DrawSmoothSphere(1.0, 16);
         glPopMatrix();
      }
   }
   
   // Drawing mid torso segments
   for (int seg = 0; seg < 3; seg++)
   {
      glPushMatrix();
      glTranslated(0, -0.5 - seg * 0.18, 0);
      float segScale = 1.0 - seg * 0.05;
      glScaled(0.48 * segScale, 0.18, 0.38 * segScale);
      DrawSmoothSphere(1.0, 36);
      glPopMatrix();
      
   // Adding segment connection ridges
      if (seg < 2)
      {
         glPushMatrix();
         glTranslated(0, -0.58 - seg * 0.18, 0);
         glScaled(0.44 * segScale, 0.05, 0.35 * segScale);
         DrawSmoothSphere(1.0, 28);
         glPopMatrix();
      }
   }
   
   // Drawing lower abdomen
   glPushMatrix();
   glTranslated(0, -1.1, 0);
   glScaled(0.44, 0.28, 0.36);
   DrawSmoothSphere(1.0, 32);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
}

/*
 * Drawing arm with segmented exoskeleton
 */
static void DrawArm(int side, double shoulderRotX, double elbowRotX, double time, double glowIntensity, int emotion)
{
   glPushMatrix();
   
   // Enabling arm texturing based on emotion
   glEnable(GL_TEXTURE_2D);
   unsigned int armTexture;
   if (emotion == 0) armTexture = alienTexture2;      // Calm: texture 2 (same as torso)
   else if (emotion == 1) armTexture = alienTexture3; // Curious: texture 3 (same as torso)
   else if (emotion == 2) armTexture = alienTexture1; // Alert: texture 1 (same as torso)
   else armTexture = alienTexture2;                   // Thinking: texture 2
   glBindTexture(GL_TEXTURE_2D, armTexture);
   
   // Positioning shoulder
   glTranslated(side * 0.55, 0.35, -0.05);
   glRotated(shoulderRotX + side * 8, 1, 0, 0);
   glRotated(side * 15, 0, 0, 1);
   glRotated(side * 10, 0, 1, 0);
   
   float skinColor[] = {1.0, 1.0, 1.0, 1.0};
   glColor4fv(skinColor);
   
   // Drawing shoulder joint sphere
   glPushMatrix();
   glScaled(1.0, 1.0, 0.95);
   DrawSmoothSphere(0.13, 24);
   glPopMatrix();
   
   // Drawing upper arm segments
   for (int seg = 0; seg < 3; seg++)
   {
      glPushMatrix();
      glTranslated(0, -seg * 0.14, 0);
      glRotated(90, 0, 1, 0);
      
      GLUquadric* quad = gluNewQuadric();
      gluQuadricNormals(quad, GLU_SMOOTH);
      gluQuadricTexture(quad, GL_TRUE);
      double radius = 0.095 - seg * 0.005;
      gluCylinder(quad, radius, radius * 0.92, 0.13, 24, 1);
      gluDeleteQuadric(quad);
      glPopMatrix();
      
   // Adding segment joint rings
      if (seg < 2)
      {
         glPushMatrix();
         glTranslated(0, -(seg + 1) * 0.14 + 0.01, 0);
         glScaled(1.15, 0.4, 1.15);
         DrawSmoothSphere(0.095, 20);
         glPopMatrix();
      }
   }
   
   // Drawing elbow joint
   glTranslated(0, -0.42, 0);
   glPushMatrix();
   glScaled(1.1, 0.85, 1.0);
   DrawSmoothSphere(0.11, 24);
   glPopMatrix();
   
   glRotated(elbowRotX, 1, 0, 0);
   
   // Drawing forearm segments
   for (int seg = 0; seg < 4; seg++)
   {
      glPushMatrix();
      glTranslated(0, -seg * 0.13, 0);
      glRotated(90, 0, 1, 0);
      
      GLUquadric* quad = gluNewQuadric();
      gluQuadricNormals(quad, GLU_SMOOTH);
      gluQuadricTexture(quad, GL_TRUE);
      double radius = 0.085 - seg * 0.005;
      gluCylinder(quad, radius, radius * 0.94, 0.12, 22, 1);
      gluDeleteQuadric(quad);
      glPopMatrix();
      
   // Adding segment joints
      if (seg < 3)
      {
         glPushMatrix();
         glTranslated(0, -(seg + 1) * 0.13 + 0.01, 0);
         glScaled(1.12, 0.38, 1.12);
         DrawSmoothSphere(0.085, 18);
         glPopMatrix();
      }
   }
   
   // Drawing wrist joint
   glTranslated(0, -0.52, 0);
   DrawSmoothSphere(0.052, 20);
   
   // Drawing hand base
   glPushMatrix();
   glTranslated(0, -0.08, 0);
   glScaled(0.6, 0.9, 0.45);
   DrawSmoothSphere(0.09, 24);
   glPopMatrix();
   
   // Drawing clawed fingers
   for (int finger = 0; finger < 3; finger++)
   {
      glPushMatrix();
      double angleSpread = -25 + finger * 25;
      glRotated(angleSpread, 0, 0, 1);
      glRotated(-12, 1, 0, 0);
      glTranslated(0, -0.14, 0);
      
      // Four finger segments (very long, claw-like)
      for (int segment = 0; segment < 4; segment++)
      {
         glTranslated(0, -0.1, 0);
         
         glPushMatrix();
         glRotated(90, 0, 1, 0);
         GLUquadric* quad = gluNewQuadric();
         gluQuadricNormals(quad, GLU_SMOOTH);
         gluQuadricTexture(quad, GL_TRUE);
         double segRadius = 0.022 - segment * 0.004;
         gluCylinder(quad, segRadius, segRadius * 0.85, 0.09, 16, 1);
         gluDeleteQuadric(quad);
         glPopMatrix();
         
         // Joint knuckle
         if (segment < 3) {
            DrawSmoothSphere(segRadius, 14);
         }
      }
      
   // Drawing claw tip
      glTranslated(0, -0.1, 0);
      glPushMatrix();
      glRotated(90, 0, 1, 0);
      glRotated(180, 1, 0, 0);
      GLUquadric* quad = gluNewQuadric();
      gluQuadricNormals(quad, GLU_SMOOTH);
      gluCylinder(quad, 0.015, 0.0, 0.08, 12, 1);
      gluDeleteQuadric(quad);
      glPopMatrix();
      
      glPopMatrix();
   }
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 * Drawing translucent wing membrane
 */
static void DrawWingMembrane(int side, double time, double glidePhase)
{
   glPushMatrix();
   glTranslated(side * 0.4, 0.35, -0.12);
   glRotated(side * (18 + 18 * sin(glidePhase)), 0, 0, 1);
   glRotated(25 + 10 * sin(glidePhase * 0.5), 1, 0, 0);
   glRotated(side * 10, 0, 1, 0);
   
   // Enabling wing texturing and transparency
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, alienTexture3);
   
   // Enable transparency
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);
   glDisable(GL_LIGHTING);
   
   // Drawing membrane layer stack
   for (int layer = 0; layer < 3; layer++)
   {
      float alpha = 0.35 - layer * 0.08;
      float scale = 1.0 + layer * 0.08;
      
      // Use white/light color to show texture
      glColor4f(1.0, 1.0, 1.0, alpha);
      
      glPushMatrix();
      glScaled(scale, scale, scale);
      
      // Wing membrane - organic shape
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.5, 0.5);
      glVertex3d(0, 0, 0);  // Base
      
      // Create moth-wing shape with curves
      for (int i = 0; i <= 24; i++)
      {
         double t = i / 24.0;
         double angle = t * 130.0 - 10.0;
         
         // Wing profile - wider at middle
         double radius;
         if (t < 0.5) {
            radius = 0.6 * (t * 2.0);  // Expand
         } else {
            radius = 0.6 * (2.0 - t * 2.0);  // Contract
         }
         radius = radius * (0.8 + 0.4 * sin(t * 3.14));  // Organic curve
         
         double x = radius * Cos(angle);
         double y = -radius * Sin(angle) - t * 0.7;
         
         // Map texture coordinates
         double u = 0.5 + x * 0.8;
         double v = 0.5 + y * 0.7;
         glTexCoord2f(u, v);
         glVertex3d(x, y, 0.01 * sin(t * 6.28 * 2));
      }
      glEnd();
      
      glPopMatrix();
   }
   
   // Drawing vein network
   glColor4f(0.8, 0.92, 1.0, 0.5);
   glLineWidth(2.0);
   
   // Drawing main veins
   glBegin(GL_LINES);
   // Central vein
   for (int i = 0; i < 12; i++)
   {
      double t1 = i / 12.0;
      double t2 = (i + 1) / 12.0;
      double y1 = -t1 * 0.7;
      double y2 = -t2 * 0.7;
      glVertex3d(0, y1, 0.002);
      glVertex3d(0, y2, 0.002);
   }
   
   // Drawing branching veins
   for (int branch = 0; branch < 8; branch++)
   {
      double t = (branch + 1) / 9.0;
      double startY = -t * 0.7;
      double angle = -10 + t * 130.0;
      double radius = 0.5 * t * (1.0 - t * 0.5);
      
      double endX = radius * Cos(angle);
      double endY = startY - radius * Sin(angle) * 0.5;
      
      glVertex3d(0, startY, 0.002);
      glVertex3d(endX, endY, 0.002);
      
      // Sub-branches
      for (int sub = 1; sub <= 2; sub++)
      {
         double subT = sub / 3.0;
         double subX = endX * subT;
         double subY = startY + (endY - startY) * subT;
         double subEndX = subX + 0.1 * Cos(angle - 30);
         double subEndY = subY - 0.1 * Sin(angle - 30);
         
         glVertex3d(subX, subY, 0.002);
         glVertex3d(subEndX, subEndY, 0.002);
      }
   }
   glEnd();
   
   glLineWidth(1.0);
   
   // Drawing edge shimmer particles
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glPointSize(3.0);
   glColor4f(0.7, 0.9, 1.0, 0.6);
   
   glBegin(GL_POINTS);
   for (int i = 0; i < 20; i++)
   {
      double t = i / 20.0;
      double angle = -10 + t * 130.0;
      double radius = 0.55 * t * (1.0 - t * 0.5);
      double shimmer = sin(time * 4.0 + i * 0.5) * 0.5 + 0.5;
      
      if (shimmer > 0.6) {
         double x = radius * Cos(angle);
         double y = -t * 0.7 - radius * Sin(angle) * 0.5;
         glVertex3d(x, y, 0.003);
      }
   }
   glEnd();
   glPointSize(1.0);
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_LIGHTING);
   
   glPopMatrix();
}

/*
 * Drawing crystalline belt with floating orbs
 */
static void DrawQuartzBelt(double time, double glowIntensity)
{
   glPushMatrix();
   glTranslated(0, -0.65, 0);
   
   float quartzColor[] = {0.88, 0.94, 1.0, 1.0};
   glColor4fv(quartzColor);
   
   // Drawing belt segments
   for (int seg = 0; seg < 16; seg++)
   {
      double angle = seg * 22.5;
      glPushMatrix();
      glRotated(angle, 0, 1, 0);
      glTranslated(0, 0, 0.36);
      
      // Belt segment
      glScaled(0.06, 0.08, 0.08);
      DrawSmoothSphere(1.0, 16);
      glPopMatrix();
   }
   
   // Placing crystal shards
   for (int i = 0; i < 12; i++)
   {
      double angle = i * 30.0 + time * 5.0;
      glPushMatrix();
      glRotated(angle, 0, 1, 0);
      glTranslated(0.38, 0, 0);
      glRotated(45 + sin(time + i) * 15, 0, 1, 0);
      glRotated(20, 1, 0, 0);
      
      // Crystal prism
      glScaled(0.04, 0.1, 0.04);
      
      glBegin(GL_TRIANGLE_FAN);
      glNormal3d(0, 1, 0);
      glVertex3d(0, 1, 0);  // Top point
      for (int v = 0; v <= 6; v++)
      {
         double a = v * 60.0;
         glNormal3d(Cos(a), 0, Sin(a));
         glVertex3d(Cos(a), 0, Sin(a));
      }
      glEnd();
      
      glBegin(GL_TRIANGLE_FAN);
      glNormal3d(0, -1, 0);
      glVertex3d(0, -1, 0);  // Bottom point
      for (int v = 6; v >= 0; v--)
      {
         double a = v * 60.0;
         glNormal3d(Cos(a), 0, Sin(a));
         glVertex3d(Cos(a), 0, Sin(a));
      }
      glEnd();
      
      glPopMatrix();
   }
   
   // Drawing floating orbs and effects
   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   
   for (int orb = 0; orb < 3; orb++)
   {
      double orbAngle = orb * 120.0 + time * 35.0;
      double orbRadius = 0.52 + 0.08 * sin(time * 1.5 + orb * 2.1);
      double orbHeight = 0.18 * sin(time * 2.3 + orb * 2.0);
      double orbPhase = time * 3.0 + orb * 2.1;
      
      glPushMatrix();
      glRotated(orbAngle, 0, 1, 0);
      glTranslated(orbRadius, orbHeight, 0);
      
      // Core orb
      float orbR = 0.6 + 0.4 * sin(orbPhase);
      float orbG = 0.9;
      float orbB = 1.0;
      glColor4f(orbR * glowIntensity, orbG * glowIntensity, orbB * glowIntensity, 0.9);
      DrawSmoothSphere(0.045, 16);
      
      // Energy field around orb
      glDepthMask(GL_FALSE);
      glColor4f(orbR * glowIntensity, orbG * glowIntensity, orbB * glowIntensity, 0.3);
      DrawSmoothSphere(0.07, 12);
      
      // Orbital rings
      for (int ring = 0; ring < 2; ring++)
      {
         glPushMatrix();
         glRotated(time * 80.0 + ring * 90, 0, 1, 0);
         glRotated(60, 1, 0, 0);
         
         glColor4f(orbR * glowIntensity, orbG * glowIntensity, orbB * glowIntensity, 0.5);
         glBegin(GL_LINE_LOOP);
         for (int i = 0; i <= 20; i++)
         {
            double a = i * 18.0;
            glVertex3d(0.06 * Cos(a), 0.06 * Sin(a), 0);
         }
         glEnd();
         glPopMatrix();
      }
      
      glDepthMask(GL_TRUE);
      glPopMatrix();
   }
   
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
   
   glPopMatrix();
}

/*
 * Drawing full Lunaris Vex character
 */
void LunarisVex(double x, double y, double z, double facing, 
                double time, int emotion, int isGliding)
{
   // Initializing required systems
   InitLunarisShaders();
   InitDustParticles();
   
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(facing, 0, 1, 0);
   
   // Applying floating drift motion
   double floatOffset = 0.12 * sin(time * 1.2) + 0.06 * sin(time * 2.3);
   glTranslated(0, floatOffset, 0);
   
   // Applying gentle body sway
   glRotated(4.0 * sin(time * 0.7), 0, 0, 1);
   glRotated(2.0 * sin(time * 0.9), 1, 0, 0);
   
   // Computing glow intensity
   double glowIntensity = 0.5 + 0.4 * sin(time * 2.5);
   if (emotion == 1) glowIntensity *= 1.2;      // Curious = brighter
   else if (emotion == 2) glowIntensity = 1.0;  // Alert = full glow
   else if (emotion == 3) glowIntensity *= 0.8; // Thinking = dimmer
   
   // Selecting fissure color based on emotion
   float fissureR, fissureG, fissureB;
   if (emotion == 0) {  // Calm - blue glow
      fissureR = 0.3; fissureG = 0.5; fissureB = 0.8;
   } else if (emotion == 1) {  // Curious - cyan
      fissureR = 0.2; fissureG = 0.7; fissureB = 0.9;
   } else if (emotion == 2) {  // Alert - orange-red
      fissureR = 0.9; fissureG = 0.4; fissureB = 0.2;
   } else {  // Thinking - purple
      fissureR = 0.6; fissureG = 0.3; fissureB = 0.8;
   }
   
   // Disabling shader for textured body parts
   int shaderEnabled = 0;
   
   // Drawing torso
   DrawTorso(glowIntensity, time, emotion);
   
   // Drawing head and eyes
   glPushMatrix();
   glTranslated(0, 0.65, 0);
   glRotated(3.0 * sin(time * 1.1), 0, 1, 0);  // Slight head movement
   DrawAlienHead(time, emotion, glowIntensity);
   DrawIridescentEyes(time, emotion, glowIntensity);
   glPopMatrix();
   
   // Drawing arms
   double armSwing = 12.0 * sin(time * 0.4);
   double leftElbow = 45 + 15 * sin(time * 0.7);
   double rightElbow = 45 - 15 * sin(time * 0.7 + 0.5);
   
   DrawArm(-1, armSwing, leftElbow, time, glowIntensity, emotion);
   DrawArm(1, -armSwing, rightElbow, time, glowIntensity, emotion);
   
   // Skipping wing membranes (design choice)
   
   // Drawing standing legs with texture
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.28, -1.25, -0.08);
      
      double legSwing = 3 * sin(time * 0.5 + side);
      glRotated(legSwing, 1, 0, 0);  // Minimal sway for standing
      glRotated(side * 3, 0, 0, 1);
      
      // Enable texturing for legs - different texture per emotion
      glEnable(GL_TEXTURE_2D);
      unsigned int legTexture;
      if (emotion == 0) legTexture = alienTexture3;      // Calm: texture 3
      else if (emotion == 1) legTexture = alienTexture1; // Curious: texture 1
      else if (emotion == 2) legTexture = alienTexture2; // Alert: texture 2
      else legTexture = alienTexture3;                   // Thinking: texture 3
      glBindTexture(GL_TEXTURE_2D, legTexture);
      
      float skinColor[] = {1.0, 1.0, 1.0, 1.0};
      glColor4fv(skinColor);
      
      // Upper leg (thigh) - segmented - MUCH WIDER
      for (int seg = 0; seg < 3; seg++)
      {
         glPushMatrix();
         glTranslated(0, -seg * 0.24, 0);
         glRotated(90, 1, 0, 0);
         
         GLUquadric* quad = gluNewQuadric();
         gluQuadricNormals(quad, GLU_SMOOTH);
         gluQuadricTexture(quad, GL_TRUE);
         double radius = 0.12 - seg * 0.006;
         gluCylinder(quad, radius, radius * 0.94, 0.23, 24, 1);
         gluDeleteQuadric(quad);
         glPopMatrix();
         
         // Segment joint rings
         if (seg < 2)
         {
            glPushMatrix();
            glTranslated(0, -(seg + 1) * 0.24 + 0.02, 0);
            glScaled(1.15, 0.45, 1.15);
            DrawSmoothSphere(0.12, 22);
            glPopMatrix();
         }
      }
      
      // Knee joint - prominent - standing straight
      glTranslated(0, -0.78, 0);
      glPushMatrix();
      glScaled(1.25, 0.85, 1.25);
      DrawSmoothSphere(0.14, 24);
      glPopMatrix();
      
      glRotated(-10, 1, 0, 0);  // Almost straight, natural standing bend
      
      // Lower leg - longer, very segmented - WIDER
      for (int seg = 0; seg < 4; seg++)
      {
         glPushMatrix();
         glTranslated(0, -seg * 0.22, 0);
         glRotated(90, 1, 0, 0);
         
         GLUquadric* quad = gluNewQuadric();
         gluQuadricNormals(quad, GLU_SMOOTH);
         gluQuadricTexture(quad, GL_TRUE);
         double radius = 0.105 - seg * 0.005;
         gluCylinder(quad, radius, radius * 0.95, 0.21, 22, 1);
         gluDeleteQuadric(quad);
         glPopMatrix();
         
         // Segment joints
         if (seg < 3)
         {
            glPushMatrix();
            glTranslated(0, -(seg + 1) * 0.22 + 0.02, 0);
            glScaled(1.12, 0.42, 1.12);
            DrawSmoothSphere(0.105, 20);
            glPopMatrix();
         }
      }
      
      // Ankle - thicker
      glTranslated(0, -0.92, 0);
      DrawSmoothSphere(0.1, 20);
      
      glRotated(10, 1, 0, 0);  // Slight forward angle for standing
      
      // Foot - wider, more stable for standing
      glPushMatrix();
      glTranslated(0, -0.08, 0);
      glRotated(90, 1, 0, 0);
      
      // Foot base - WIDER
      glScaled(0.24, 0.32, 0.16);
      DrawSmoothSphere(1.0, 20);
      glPopMatrix();
      
      // Three elongated toes with claws - THICKER
      for (int toe = 0; toe < 3; toe++)
      {
         glPushMatrix();
         double toeAngle = -20 + toe * 20;
         glRotated(toeAngle, 0, 0, 1);
         glRotated(-8, 1, 0, 0);
         glTranslated(0, -0.08, -0.08);
         
         // Toe segments - WIDER
         for (int seg = 0; seg < 3; seg++)
         {
            glTranslated(0, -0.1, 0);
            
            glPushMatrix();
            glRotated(90, 1, 0, 0);
            GLUquadric* quad = gluNewQuadric();
            gluQuadricNormals(quad, GLU_SMOOTH);
            double toeRadius = 0.042 - seg * 0.008;
            gluCylinder(quad, toeRadius, toeRadius * 0.88, 0.09, 14, 1);
            gluDeleteQuadric(quad);
            glPopMatrix();
            
            if (seg < 2) {
               DrawSmoothSphere(0.042 - seg * 0.008, 12);
            }
         }
         
         // Claw tip
         glTranslated(0, -0.1, 0);
         glPushMatrix();
         glRotated(90, 1, 0, 0);
         glRotated(180, 1, 0, 0);
         GLUquadric* quad = gluNewQuadric();
         gluQuadricNormals(quad, GLU_SMOOTH);
         gluCylinder(quad, 0.028, 0.0, 0.08, 10, 1);
         gluDeleteQuadric(quad);
         glPopMatrix();
         
         glPopMatrix();
      }
      
      glDisable(GL_TEXTURE_2D);
      glPopMatrix();
   }
   
   // Omitting belt and orbs
   
   // Emitting dust particles occasionally
   if ((int)(time * 60.0) % 8 == 0) {
      EmitDustParticle(x, y - 0.8, z, emotion);
   }
   
   UpdateDustParticles(1.0 / 60.0);
   DrawDustParticles(emotion, glowIntensity);
   
   glPopMatrix();
}
