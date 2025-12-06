/*
 *  Floating Crystal Shards
 *  Took Help from AI for advanced shader effects and particle system
 */

#include "CSCIx229.h"
#include "shader_loader.h"
#include <math.h>
#include <stdlib.h>

// Storing shader program 
static unsigned int crystalShaderProgram = 0;
// static unsigned int glowShaderProgram = 0;
static int shadersInitialized = 0;

// Managing particle system for energy emanations
#define MAX_PARTICLES 200
typedef struct {
   float x, y, z;
   float vx, vy, vz;
   float life;
   float size;
   float color[4];
} Particle;

static Particle particles[MAX_PARTICLES];
static int particlesInitialized = 0;

/*
 *  Initializing shader programs
 */
static void InitShaders()
{
   if (shadersInitialized) return;
   
   // Loading crystal shader
   crystalShaderProgram = CreateShaderProgram(
      "shaders/crystal.vert",
      "shaders/crystal.frag"
   );
   
   if (crystalShaderProgram == 0) {
      fprintf(stderr, "Warning: Failed to load crystal shaders, using fixed pipeline\n");
   }
   
   shadersInitialized = 1;
}

/*
 *  Initializing particle system
 */
static void InitParticles()
{
   if (particlesInitialized) return;
   
   for (int i = 0; i < MAX_PARTICLES; i++) {
      particles[i].life = 0.0;
   }
   particlesInitialized = 1;
}

/*
 *  Emitting new particle
 */
static void EmitParticle(float x, float y, float z, float time, int seed)
{
   // Finding dead particle
   for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particles[i].life <= 0.0) {
         particles[i].x = x + (rand() % 100 - 50) / 500.0;
         particles[i].y = y + (rand() % 100 - 50) / 500.0;
         particles[i].z = z + (rand() % 100 - 50) / 500.0;
         
         // Setting radial outward velocity
         float angle = (rand() % 360) * M_PI / 180.0;
         float elevation = (rand() % 60 - 30) * M_PI / 180.0;
         float speed = 0.05 + (rand() % 100) / 1000.0;
         
         particles[i].vx = cos(angle) * cos(elevation) * speed;
         particles[i].vy = sin(elevation) * speed + 0.02; // Adding slight upward drift
         particles[i].vz = sin(angle) * cos(elevation) * speed;
         
         particles[i].life = 2.0 + (rand() % 100) / 50.0;
         particles[i].size = 0.02 + (rand() % 100) / 2000.0;
         
         // Using alien like color palette: cyan, magenta, violet, electric blue
         int colorType = (seed + i) % 4;
         if (colorType == 0) { // Cyan
            particles[i].color[0] = 0.1;
            particles[i].color[1] = 0.9;
            particles[i].color[2] = 1.0;
         } else if (colorType == 1) { // Magenta
            particles[i].color[0] = 1.0;
            particles[i].color[1] = 0.2;
            particles[i].color[2] = 0.9;
         } else if (colorType == 2) { // Violet
            particles[i].color[0] = 0.7;
            particles[i].color[1] = 0.1;
            particles[i].color[2] = 1.0;
         } else { // Electric blue
            particles[i].color[0] = 0.2;
            particles[i].color[1] = 0.5;
            particles[i].color[2] = 1.0;
         }
         particles[i].color[3] = 1.0;
         break;
      }
   }
}

/*
 *  Updating particle system
 * Took help from AI to finish this part
 */
static void UpdateParticles(float dt)
{
   for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particles[i].life > 0.0) {
         particles[i].x += particles[i].vx * dt;
         particles[i].y += particles[i].vy * dt;
         particles[i].z += particles[i].vz * dt;
         particles[i].life -= dt;
         
         // Applying gravity like effect
         particles[i].vy += 0.001 * sin(particles[i].life * 3.0);
      }
   }
}

/*
 *  Drawing particle system with additive blending
 */
static void DrawParticles()
{
   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Using additive blending here
   glDepthMask(GL_FALSE);
   
   for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particles[i].life > 0.0) {
         float alpha = particles[i].life / 3.0;
         if (alpha > 1.0) alpha = 1.0;
         
         glPushMatrix();
         glTranslatef(particles[i].x, particles[i].y, particles[i].z);
         
         // Using billboard effect
         glColor4f(particles[i].color[0], 
                   particles[i].color[1], 
                   particles[i].color[2], 
                   alpha * particles[i].color[3]);
         
         // Drawing as point sprite
         glPointSize(particles[i].size * 100.0);
         glBegin(GL_POINTS);
         glVertex3f(0, 0, 0);
         glEnd();
         
         // Adding glow halo
         glColor4f(particles[i].color[0], 
                   particles[i].color[1], 
                   particles[i].color[2], 
                   alpha * 0.3);
         glPointSize(particles[i].size * 200.0);
         glBegin(GL_POINTS);
         glVertex3f(0, 0, 0);
         glEnd();
         
         glPopMatrix();
      }
   }
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
}

/*
 *  Calculating advanced spectral color with gradient
 *  Creating realistic alieny effect
 *  Took help from AI
 */
static void AdvancedSpectralColor(float t, float viewAngle, float alpha)
{
   // Calculating Fresnel effect
   float fresnel = pow(1.0 - cos(viewAngle), 3.0);
   
   // Calculating base spectral gradient
   float r, g, b;
   
   if (t < 0.25) {
      // Deep cyan to electric blue
      float local = t * 4.0;
      r = 0.05 + local * 0.15;
      g = 0.85 - local * 0.35;
      b = 1.0;
   } else if (t < 0.5) {
      // Electric blue to violet
      float local = (t - 0.25) * 4.0;
      r = 0.2 + local * 0.6;
      g = 0.5 - local * 0.4;
      b = 1.0;
   } else if (t < 0.75) {
      // Violet to magenta
      float local = (t - 0.5) * 4.0;
      r = 0.8 + local * 0.2;
      g = 0.1 + local * 0.1;
      b = 1.0 - local * 0.1;
   } else {
      // Magenta to hot pink
      float local = (t - 0.75) * 4.0;
      r = 1.0;
      g = 0.2 + local * 0.3;
      b = 0.9 - local * 0.4;
   }
   
   // Applying Fresnel enhancement
   r = r * (1.0 - fresnel) + fresnel;
   g = g * (1.0 - fresnel) + fresnel * 0.9;
   b = b * (1.0 - fresnel) + fresnel;
   
   glColor4f(r, g, b, alpha);
}

/*
 *  Drawing caustics pattern on surfaces near crystal
 *  Simulating light refraction patterns
 */
static void DrawCaustics(float intensity, float time)
{
   if (intensity <= 0) return;
   
   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Using additive blend
   glDepthMask(GL_FALSE);
   
   // Drawing animated caustic pattern as ground projection
   glPushMatrix();
   glTranslatef(0, -0.5, 0); // Slightly below crystal
   glRotatef(90, 1, 0, 0);
   
   int rings = 8;
   int segments = 24;
   
   for (int r = 0; r < rings; r++) {
      float innerR = r * 0.3;
      float outerR = (r + 1) * 0.3;
      
      glBegin(GL_QUAD_STRIP);
      for (int s = 0; s <= segments; s++) {
         float angle = s * 2.0 * M_PI / segments;
         
         // Animating caustic distortion
         float distort1 = sin(time * 2.0 + r * 0.5 + angle * 3.0) * 0.1;
         float distort2 = sin(time * 2.0 + (r+1) * 0.5 + angle * 3.0) * 0.1;
         
         float x1 = (innerR + distort1) * cos(angle);
         float z1 = (innerR + distort1) * sin(angle);
         float x2 = (outerR + distort2) * cos(angle);
         float z2 = (outerR + distort2) * sin(angle);
         
         // Shifting color with time and radius
         float colorPhase = fmod(time * 0.3 + r * 0.2 + s * 0.05, 1.0);
         float alpha = intensity * (1.0 - r / (float)rings) * 0.4;
         
         AdvancedSpectralColor(colorPhase, 0.5, alpha);
         glVertex3f(x1, z1, 0);
         
         AdvancedSpectralColor(colorPhase + 0.1, 0.5, alpha * 0.7);
         glVertex3f(x2, z2, 0);
      }
      glEnd();
   }
   
   glPopMatrix();
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
}

/*
 *  Drawing energy field distortion sphere
 *  Creating volumetric effect showing gravitational manipulation
 */
static void DrawEnergyField(float radius, float intensity, float time)
{
   if (intensity <= 0) return;
   
   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);
   
   // Drawing distorted sphere
   int lats = 12;
   int longs = 16;
   
   for (int i = 0; i < lats; i++) {
      float lat1 = M_PI * (-0.5 + (float)i / lats);
      float lat2 = M_PI * (-0.5 + (float)(i + 1) / lats);
      
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= longs; j++) {
         float lng = 2 * M_PI * j / longs;
         
         // Calculating distortion based on time and position
         float distort1 = 1.0 + 0.1 * sin(time * 3.0 + lat1 * 4.0 + lng * 3.0);
         float distort2 = 1.0 + 0.1 * sin(time * 3.0 + lat2 * 4.0 + lng * 3.0);
         
         float x1 = cos(lng) * cos(lat1) * radius * distort1;
         float y1 = sin(lat1) * radius * distort1;
         float z1 = sin(lng) * cos(lat1) * radius * distort1;
         
         float x2 = cos(lng) * cos(lat2) * radius * distort2;
         float y2 = sin(lat2) * radius * distort2;
         float z2 = sin(lng) * cos(lat2) * radius * distort2;
         
         // Setting color based on position
         float colorPhase = fmod((lat1 + M_PI/2) / M_PI + time * 0.1, 1.0);
         float alpha = intensity * 0.15 * (1.0 - fabs(sin(lat1)));
         
         AdvancedSpectralColor(colorPhase, 0.5, alpha);
         glVertex3f(x1, y1, z1);
         
         AdvancedSpectralColor(colorPhase + 0.05, 0.5, alpha);
         glVertex3f(x2, y2, z2);
      }
      glEnd();
   }
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
}
// Took help from AI to finish this part
static void DrawMicroFacet(float x1, float y1, float z1,
                          float x2, float y2, float z2,
                          float x3, float y3, float z3,
                          float colorPhase, float alpha)
{
   // Calculating face normal
   float dx1 = x2 - x1, dy1 = y2 - y1, dz1 = z2 - z1;
   float dx2 = x3 - x1, dy2 = y3 - y1, dz2 = z3 - z1;
   float nx = dy1*dz2 - dz1*dy2;
   float ny = dz1*dx2 - dx1*dz2;
   float nz = dx1*dy2 - dy1*dx2;
   float len = sqrt(nx*nx + ny*ny + nz*nz);
   if (len > 0) {
      nx /= len; ny /= len; nz /= len;
   }
   
   // Subdividing into micro-facets
   int subdivX = 6, subdivY = 4;
   for (int i = 0; i < subdivX; i++) {
      for (int j = 0; j < subdivY; j++) {
         float u1 = i / (float)subdivX;
         float u2 = (i+1) / (float)subdivX;
         float v1 = j / (float)subdivY;
         float v2 = (j+1) / (float)subdivY;
         
         // Interpolating positions with slight perturbation for sparkle
         float p1x = x1 + u1*(x2-x1) + v1*(x3-x1);
         float p1y = y1 + u1*(y2-y1) + v1*(y3-y1);
         float p1z = z1 + u1*(z2-z1) + v1*(z3-z1);
         
         float p2x = x1 + u2*(x2-x1) + v1*(x3-x1);
         float p2y = y1 + u2*(y2-y1) + v1*(y3-y1);
         float p2z = z1 + u2*(z2-z1) + v1*(y3-z1);
         
         float p3x = x1 + u2*(x2-x1) + v2*(x3-x1);
         float p3y = y1 + u2*(y2-y1) + v2*(y3-y1);
         float p3z = z1 + u2*(z2-z1) + v2*(z3-z1);
         
         float p4x = x1 + u1*(x2-x1) + v2*(x3-x1);
         float p4y = y1 + u1*(y2-y1) + v2*(y3-y1);
         float p4z = z1 + u1*(z2-z1) + v2*(z3-z1);
         
         // Varying color slightly per micro-facet with view angle
         float facetPhase = colorPhase + (i*subdivY + j) * 0.02;
         facetPhase = fmod(facetPhase, 1.0);
         
         // Calculating view angle for Fresnel effect
         float viewAngle = acos(fabs(nz)) / M_PI;
         AdvancedSpectralColor(facetPhase, viewAngle, alpha);
         
         // Highlighting edges with electric glow
         if (i == 0 || j == 0 || i == subdivX-1 || j == subdivY-1) {
            glColor4f(1.0, 0.3, 1.0, alpha * 1.5);
         }
         
         glBegin(GL_QUADS);
         glNormal3f(nx, ny, nz);
         glVertex3f(p1x, p1y, p1z);
         glVertex3f(p2x, p2y, p2z);
         glVertex3f(p3x, p3y, p3z);
         glVertex3f(p4x, p4y, p4z);
         glEnd();
      }
   }
}

/*
 *  Drawing elongated octahedron core
 *  Central crystal structure with 8 faces
 */
static void DrawOctahedronCore(float length, float width, float height, 
                              float colorPhase, float alpha, int emissive)
{
   // Setting material properties
   float white[] = {1.0, 1.0, 1.0, 1.0};
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float emission[] = {0.15, 0.25, 0.8, 1.0}; // Iridescent glow
   
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f); // Diamond-like
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   
   if (emissive) {
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   } else {
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   }
   
   // Octahedron vertices (elongated along Z-axis)
   float hl = length / 2.0;
   float hw = width / 2.0;
   float hh = height / 2.0;
   
   // Six vertices of elongated octahedron
   float verts[6][3] = {
      { 0,    0,    hl},  // Top
      { hw,   0,    0 },  // Front
      { 0,    hh,   0 },  // Right
      {-hw,   0,    0 },  // Back
      { 0,   -hh,   0 },  // Left
      { 0,    0,   -hl}   // Bottom
   };
   
   // Drawing 8 triangular faces with micro-facets
   // Top pyramid (4 faces)
   DrawMicroFacet(verts[0][0], verts[0][1], verts[0][2],
                  verts[1][0], verts[1][1], verts[1][2],
                  verts[2][0], verts[2][1], verts[2][2],
                  colorPhase, alpha);
   
   DrawMicroFacet(verts[0][0], verts[0][1], verts[0][2],
                  verts[2][0], verts[2][1], verts[2][2],
                  verts[3][0], verts[3][1], verts[3][2],
                  colorPhase + 0.125, alpha);
   
   DrawMicroFacet(verts[0][0], verts[0][1], verts[0][2],
                  verts[3][0], verts[3][1], verts[3][2],
                  verts[4][0], verts[4][1], verts[4][2],
                  colorPhase + 0.25, alpha);
   
   DrawMicroFacet(verts[0][0], verts[0][1], verts[0][2],
                  verts[4][0], verts[4][1], verts[4][2],
                  verts[1][0], verts[1][1], verts[1][2],
                  colorPhase + 0.375, alpha);
   
   // Bottom pyramid (4 faces)
   DrawMicroFacet(verts[5][0], verts[5][1], verts[5][2],
                  verts[2][0], verts[2][1], verts[2][2],
                  verts[1][0], verts[1][1], verts[1][2],
                  colorPhase + 0.5, alpha);
   
   DrawMicroFacet(verts[5][0], verts[5][1], verts[5][2],
                  verts[3][0], verts[3][1], verts[3][2],
                  verts[2][0], verts[2][1], verts[2][2],
                  colorPhase + 0.625, alpha);
   
   DrawMicroFacet(verts[5][0], verts[5][1], verts[5][2],
                  verts[4][0], verts[4][1], verts[4][2],
                  verts[3][0], verts[3][1], verts[3][2],
                  colorPhase + 0.75, alpha);
   
   DrawMicroFacet(verts[5][0], verts[5][1], verts[5][2],
                  verts[1][0], verts[1][1], verts[1][2],
                  verts[4][0], verts[4][1], verts[4][2],
                  colorPhase + 0.875, alpha);
}

/*
 *  Drawing inner refractive core
 *  Floating kernel inside the main crystal
 */
static void DrawInnerCore(float size, float colorPhase, float time)
{
   glPushMatrix();
   
   // Subtle internal rotation
   glRotatef(time * 20.0, 1, 1, 1);
   
   // Very transparent and glowy
   float emission[] = {0.3, 0.4, 1.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   
   // Drawing small octahedron core
   DrawOctahedronCore(size * 0.3, size * 0.15, size * 0.15, 
                      colorPhase + 0.5, 0.2, 1);
   
   glPopMatrix();
}

/*
 *  Drawing satellite micro-shard
 *  Tiny crystal fragments orbiting the main crystal
 */
static void DrawSatelliteShard(float size, float colorPhase)
{
   float emission[] = {0.2, 0.3, 0.9, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
   
   DrawOctahedronCore(size * 0.15, size * 0.05, size * 0.05,
                      colorPhase, 0.7, 1);
}

/*
 *  Drawing volumetric light cone
 *  Soft glowing emanating from the crystal
 */
static void DrawVolumeLightCone(float intensity)
{
   if (intensity <= 0) return;
   
   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Using additive blending for glow here
   glDepthMask(GL_FALSE);
   
   int segments = 16;
   float coneHeight = 1.5;
   float coneRadius = 0.8;
   
   glBegin(GL_TRIANGLE_FAN);
   glColor4f(0.1, 0.4, 1.0, intensity * 0.3);
   glVertex3f(0, 0, 0); 
   
   for (int i = 0; i <= segments; i++) {
      float angle = i * 2.0 * M_PI / segments;
      float x = coneRadius * cos(angle);
      float y = coneRadius * sin(angle);
      glColor4f(0.05, 0.2, 0.6, 0.0); // Fade to transparent
      glVertex3f(x, y, -coneHeight);
   }
   glEnd();
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
}

/*
 *  Main drawing function: FloatingCrystalShard
 */
void FloatingCrystalShard(double x, double y, double z,
                          double scale, double time,
                          int seed, int drawTrails,
                          double facingCamera)
{
   // Initializing systems if needed
   InitParticles();
   InitShaders();
   
   // Saving transformation
   glPushMatrix();
   
   // Position
   glTranslatef(x, y, z);
   
   // Variation based on seed
   float seedPhase = (seed % 100) / 100.0;
   float sizeVariation = 0.8 + seedPhase * 1.6; // 0.8 to 2.4 range
   float widthVariation = 0.15 + seedPhase * 0.2; // 0.15 to 0.35
   
   // Adding more natural floating
   float floatOffset = sin(time * 2.0 * M_PI / 1.5) * 0.3;
   floatOffset += sin(time * 2.0 * M_PI / 2.3) * 0.1; // Second harmonic
   floatOffset += sin(time * 2.0 * M_PI / 0.8) * 0.05; // Third harmonic
   glTranslatef(0, floatOffset, 0);
   
   // Multi-axis rotation for alieny effect
   float rotationAngle = fmod(time * 12.0, 360.0);
   glRotatef(rotationAngle, 0.3, 1, 0.2); // Primary rotation
   
   // Secondary wobble rotation
   float wobbleAngle = sin(time * 1.5) * 8.0;
   glRotatef(wobbleAngle, 1, 0, 0.5);
   
   // Scaling with pulsing effect
   float pulse = 1.0 + sin(time * 4.0) * 0.03; // Subtle pulse
   glScalef(scale * pulse, scale * pulse, scale * pulse);
   
   // Outermost layer
   float fieldIntensity = 0.7 + sin(time * 2.0) * 0.3;
   DrawEnergyField(2.5 * sizeVariation, fieldIntensity, time);
   
   // Caustics on ground
   float causticsIntensity = 0.5 + sin(time * 1.5) * 0.3;
   DrawCaustics(causticsIntensity, time);
   
   // Volumetric light cone when facing camera
   float coneIntensity = cos(facingCamera * M_PI / 180.0);
   if (coneIntensity > 0) {
      DrawVolumeLightCone(coneIntensity * 0.7);
   }
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_LIGHTING);
   glEnable(GL_NORMALIZE);
   glDepthMask(GL_FALSE); 
   
   
   float colorPhase = fmod(time * 0.08 + seedPhase, 1.0);
   
   // Outer shell layer (most transparent)
   glPushMatrix();
   glScalef(1.3, 1.3, 1.3); // Larger outer shell
   DrawOctahedronCore(sizeVariation * 1.2, widthVariation * 1.2, widthVariation * 1.2,
                      colorPhase, 0.15, 1);
   glPopMatrix();
   
   // Main crystal shell
   // Enable shader for main crystal
   if (crystalShaderProgram != 0) {
      glUseProgram(crystalShaderProgram);
      
      // Set uniform variables
      int timeLoc = glGetUniformLocation(crystalShaderProgram, "time");
      int cameraLoc = glGetUniformLocation(crystalShaderProgram, "cameraPos");
      int transparencyLoc = glGetUniformLocation(crystalShaderProgram, "transparency");
      int iridescenceLoc = glGetUniformLocation(crystalShaderProgram, "iridescence");
      int chromaticLoc = glGetUniformLocation(crystalShaderProgram, "enableChromatic");
      
      if (timeLoc != -1) glUniform1f(timeLoc, time);
      if (transparencyLoc != -1) glUniform1f(transparencyLoc, 0.65);
      if (iridescenceLoc != -1) glUniform1f(iridescenceLoc, 0.8);
      if (chromaticLoc != -1) glUniform1i(chromaticLoc, 1);
      
      // Get camera position from OpenGL modelview matrix
      float modelview[16];
      glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
      // Camera is at inverse of translation in view space
      if (cameraLoc != -1) {
         glUniform3f(cameraLoc, -modelview[12], -modelview[13], -modelview[14]);
      }
   }
   
   DrawOctahedronCore(sizeVariation, widthVariation, widthVariation,
                      colorPhase, 0.65, 1);
   
   if (crystalShaderProgram != 0) {
      glUseProgram(0);
   }
   
   // Inner refractive core
   DrawInnerCore(sizeVariation * 0.6, colorPhase, time);
   
   // Central energy core (bright, pulsing)
   glPushMatrix();
   float corePulse = 0.5 + 0.5 * sin(time * 6.0);
   glScalef(corePulse, corePulse, corePulse);
   
   // Ultra-bright core
   float coreEmission[] = {1.0, 0.7, 1.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, coreEmission);
   glColor4f(1.0, 0.9, 1.0, 0.9);
   glutSolidSphere(sizeVariation * 0.08, 12, 12);
   
   // Reset emission
   float black[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glPopMatrix();
   
   // Drawing orbiting satellite shards
   int numSatellites = 6 + (seed % 4); // 6-9 satellites
   float orbitSpeed = 10.0; // Faster orbit
   
   for (int i = 0; i < numSatellites; i++) {
      glPushMatrix();
      
      // Multi-level orbit radii
      float orbitLevel = i / 3; // Creates rings
      float orbitRadius = 0.4 + orbitLevel * 0.3 + (((seed + i * 37) % 100) / 100.0) * 0.2;
      
      // Each satellite has different orbital phase and speed
      float speedMult = 1.0 + (i % 3) * 0.3;
      float orbitAngle = fmod(time * orbitSpeed * speedMult + i * (360.0 / numSatellites), 360.0);
      
      // Multi-plane orbits (creates complex patterns)
      glRotatef(orbitAngle, 0, 1, 0);
      glRotatef(25 + i * 18, 1, 0, 0); // Varied orbital planes
      glRotatef(i * 30, 0, 0, 1); // Additional tilt
      glTranslatef(orbitRadius, 0, 0);
      
      // Satellite self-rotation
      glRotatef(time * 50.0 + i * 60, 1, 1, 0);
      
      // Draw satellite with trail effect
      if (drawTrails) {
         glDisable(GL_LIGHTING);
         glBegin(GL_LINE_STRIP);
         for (int t = 0; t <= 12; t++) {
            float trailPhase = t / 12.0;
            float trailIntensity = (1.0 - trailPhase);
            
            AdvancedSpectralColor(colorPhase + trailPhase * 0.5, 0.5, trailIntensity * 0.5);
            
            // Trail curve
            float tx = -trailPhase * 0.15;
            float ty = sin(trailPhase * M_PI) * 0.05;
            glVertex3f(tx, ty, 0);
         }
         glEnd();
         glEnable(GL_LIGHTING);
      }
      
      // Satellite shard with pulsing
      float satellitePulse = 0.8 + 0.2 * sin(time * 4.0 + i);
      glScalef(satellitePulse, satellitePulse, satellitePulse);
      
      float satellitePhase = colorPhase + i * 0.12;
      DrawSatelliteShard(sizeVariation * 0.8, satellitePhase);
      
      glPopMatrix();
   }
   
   // Restore states
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   
   // Reset emission
   float black2[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black2);
   
   
   // Emit 2-4 particles per frame for energy effect
   int emitCount = 2 + (int)(time * 10) % 3;
   for (int p = 0; p < emitCount; p++) {
      EmitParticle(0, 0, 0, time, seed);
   }
   
   // Restore transformation
   glPopMatrix();
   
   
   // (Particles are in world space now)
   UpdateParticles(0.016); // ~60fps
   
   glPushMatrix();
   glTranslatef(x, y, z);
   DrawParticles();
   glPopMatrix();
}

/*
 * Drawing multiple crystals in a cluster
 */
void FloatingCrystalCluster(double centerX, double centerY, double centerZ,
                           double clusterRadius, int numCrystals,
                           double time, int drawTrails, double facingCamera)
{
   for (int i = 0; i < numCrystals; i++) {
      // Distribute crystals in a sphere
      float theta = i * 2.0 * M_PI / numCrystals;
      float phi = acos(2.0 * (i / (float)numCrystals) - 1.0);
      
      float x = centerX + clusterRadius * sin(phi) * cos(theta);
      float y = centerY + clusterRadius * sin(phi) * sin(theta);
      float z = centerZ + clusterRadius * cos(phi);
      
      float scale = 0.5 + (i % 3) * 0.3; 
      
      FloatingCrystalShard(x, y, z, scale, time + i * 0.5,
                          i * 123, drawTrails, facingCamera);
   }
}

