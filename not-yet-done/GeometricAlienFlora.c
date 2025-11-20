/*
 *  Geometric Alien Flora - Ultra Complex Version
 *  Multi-layered crystalline plant with organic animation
 */
#include "CSCIx229.h"
#include <math.h>

// Flora structure for managing multiple instances
typedef struct {
   double x, y, z;
   double scale;
   double basePhase;
   int colorVariant;  // 0=violet, 1=jade
   double swaySpeed;
   double rotationSpeed;
} FloraInstance;

/*
 *  Generate vertex jitter for organic effect
 */
static float VertexJitter(int seed, float magnitude)
{
   // Pseudo-random based on seed
   return ((seed * 12345 + 67890) % 1000 / 1000.0 - 0.5) * magnitude;
}

/*
 *  Draw a highly detailed faceted cone base (crystalline anchor)
 */
static void DrawFacetedConeBase(double radius, double height, int facets, int rings)
{
   glPushMatrix();
   
   // Multi-ring faceted cone with beveled edges
   for (int ring = 0; ring < rings; ring++)
   {
      double h0 = (ring / (double)rings) * height;
      double h1 = ((ring + 1) / (double)rings) * height;
      double r0 = radius * (1.0 - h0 / height);
      double r1 = radius * (1.0 - h1 / height);
      
      // Add bevel effect with slight radius variation
      r0 *= (1.0 + 0.05 * sin(ring * 3.14159));
      r1 *= (1.0 + 0.05 * sin((ring + 1) * 3.14159));
      
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= facets; i++)
      {
         double angle = (i * 360.0 / facets);
         // Add slight twist to facets
         double twist = ring * 5.0;
         double a0 = angle + twist;
         double a1 = angle + twist + (360.0 / facets);
         
         // Calculate normals for hard edges (faceted look)
         double midAngle = (a0 + a1) / 2.0;
         double nx = Cos(midAngle);
         double ny = 0.3;  // Slight upward component
         double nz = Sin(midAngle);
         double len = sqrt(nx*nx + ny*ny + nz*nz);
         nx /= len; ny /= len; nz /= len;
         
         glNormal3d(nx, ny, nz);
         glVertex3d(r0 * Cos(a0), h0, r0 * Sin(a0));
         glVertex3d(r1 * Cos(a0), h1, r1 * Sin(a0));
      }
      glEnd();
      
      // Draw beveled edges between facets
      glBegin(GL_LINES);
      for (int i = 0; i <= facets; i++)
      {
         double angle = (i * 360.0 / facets) + ring * 5.0;
         glVertex3d(r0 * Cos(angle), h0, r0 * Sin(angle));
         glVertex3d(r1 * Cos(angle), h1, r1 * Sin(angle));
      }
      glEnd();
   }
   
   // Top cap with radial pattern
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(0, 1, 0);
   glVertex3d(0, height, 0);
   for (int i = 0; i <= facets; i++)
   {
      double angle = (i * 360.0 / facets);
      double r = radius * (1.0 - height / height) * 0.1;
      glVertex3d(r * Cos(angle), height, r * Sin(angle));
   }
   glEnd();
   
   glPopMatrix();
}

/*
 *  Draw pulsating root tendrils emerging from base
 */
static void DrawRootTendrils(double baseRadius, double time, int seed)
{
   int numTendrils = 8;
   for (int i = 0; i < numTendrils; i++)
   {
      double angle = (i * 360.0 / numTendrils) + seed * 10;
      double phase = time * 2.0 + i * 0.5;
      double extend = 0.3 + 0.1 * sin(phase);
      
      glPushMatrix();
      glRotated(angle, 0, 1, 0);
      glRotated(-45 - 15 * sin(phase), 0, 0, 1);
      
      // Organic tendril curve
      glBegin(GL_LINE_STRIP);
      for (int j = 0; j <= 8; j++)
      {
         double t = j / 8.0;
         double x = baseRadius * (1.0 + t * extend);
         double y = -t * 0.4 * sin(t * 3.14159);
         double thickness = 0.02 * (1.0 - t);
         glVertex3d(x, y, 0);
      }
      glEnd();
      
      glPopMatrix();
   }
}

/*
 *  Draw ultra-thin flexible stem with segmentation
 */
static void DrawSegmentedStem(double height, double radius, double swayAngle, double time)
{
   int segments = 20;
   double prevX = 0, prevY = 0, prevZ = 0;
   
   glBegin(GL_QUAD_STRIP);
   for (int seg = 0; seg <= segments; seg++)
   {
      double t = seg / (double)segments;
      double h = t * height;
      
      // Progressive sway - more at top
      double sway = swayAngle * t * t;
      double swayX = sin(sway * M_PI / 180.0) * h * 0.3;
      double swayZ = cos(sway * M_PI / 180.0 + time) * h * 0.1;
      
      // Slight radius variation for organic look
      double r = radius * (1.0 + 0.15 * sin(t * 6.28 * 3));
      
      // Calculate tangent for normal
      double dx = swayX - prevX;
      double dy = h - prevY;
      double dz = swayZ - prevZ;
      double len = sqrt(dx*dx + dy*dy + dz*dz);
      if (len > 0.001) {
         dx /= len; dy /= len; dz /= len;
      }
      
      // Draw cylinder segment with proper normals
      for (int i = 0; i <= 8; i++)
      {
         double angle = i * 45.0;
         double nx = Cos(angle);
         double nz = Sin(angle);
         
         glNormal3d(nx, 0, nz);
         glVertex3d(swayX + r * Cos(angle), h, swayZ + r * Sin(angle));
      }
      
      prevX = swayX; prevY = h; prevZ = swayZ;
   }
   glEnd();
   
   // Segmentation rings for detail
   glDisable(GL_LIGHTING);
   glColor3f(0.1, 0.1, 0.15);
   for (int seg = 0; seg <= segments; seg += 3)
   {
      double t = seg / (double)segments;
      double h = t * height;
      double sway = swayAngle * t * t;
      double swayX = sin(sway * M_PI / 180.0) * h * 0.3;
      double swayZ = cos(sway * M_PI / 180.0 + time) * h * 0.1;
      double r = radius * 1.1;
      
      glBegin(GL_LINE_LOOP);
      for (int i = 0; i <= 16; i++)
      {
         double angle = i * 22.5;
         glVertex3d(swayX + r * Cos(angle), h, swayZ + r * Sin(angle));
      }
      glEnd();
   }
   glEnable(GL_LIGHTING);
}

/*
 *  Draw a single complex geometric leaf triangle with veins and glow
 */
static void DrawComplexLeafTriangle(double size, double rotation, double glowIntensity, 
                                   int seed, double time)
{
   glPushMatrix();
   glRotated(rotation, 0, 1, 0);
   
   // Leaf triangle vertices with jitter
   double v0x = 0.0 + VertexJitter(seed, 0.01);
   double v0y = 0.0 + VertexJitter(seed + 1, 0.01);
   double v1x = size * cos(0) + VertexJitter(seed + 2, 0.02);
   double v1y = size * sin(0) + VertexJitter(seed + 3, 0.02);
   double v2x = size * cos(2.094) + VertexJitter(seed + 4, 0.02);
   double v2y = size * sin(2.094) + VertexJitter(seed + 5, 0.02);
   double v3x = size * cos(4.189) + VertexJitter(seed + 6, 0.02);
   double v3y = size * sin(4.189) + VertexJitter(seed + 7, 0.02);
   
   // Calculate normal
   double edge1x = v1x - v0x, edge1y = v1y - v0y;
   double edge2x = v2x - v0x, edge2y = v2y - v0y;
   // Cross product for normal (in XY plane, normal points in Z)
   double nz = edge1x * edge2y - edge1y * edge2x;
   if (nz < 0) nz = 1; else nz = -1;
   
   // Main leaf surface (front and back)
   for (int side = 0; side < 2; side++)
   {
      glBegin(GL_TRIANGLES);
      glNormal3d(0, 0, side ? -nz : nz);
      glVertex3d(v0x, v0y, 0);
      glVertex3d(v1x, v1y, 0);
      glVertex3d(v2x, v2y, 0);
      glEnd();
   }
   
   // Glowing edge highlights
   float edgeGlow[] = {0.4 * glowIntensity, 0.9 * glowIntensity, 0.7 * glowIntensity, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, edgeGlow);
   
   glLineWidth(2.0);
   glBegin(GL_LINE_LOOP);
   glVertex3d(v0x, v0y, 0.001);
   glVertex3d(v1x, v1y, 0.001);
   glVertex3d(v2x, v2y, 0.001);
   glEnd();
   glLineWidth(1.0);
   
   // Intricate vein pattern
   glDisable(GL_LIGHTING);
   glColor4f(0.4 * glowIntensity, 0.9 * glowIntensity, 0.7 * glowIntensity, 0.6);
   glLineWidth(1.0);
   
   // Central veins radiating from center
   int numVeins = 12;
   for (int i = 0; i < numVeins; i++)
   {
      double veinAngle = (i * 360.0 / numVeins) + time * 10.0;  // Scrolling effect
      double veinLength = size * (0.3 + 0.4 * (i % 3) / 3.0);
      
      glBegin(GL_LINE_STRIP);
      for (int j = 0; j <= 5; j++)
      {
         double t = j / 5.0;
         double r = veinLength * t;
         // Add waviness to veins
         r += 0.02 * sin(t * 6.28 * 3 + time * 5);
         glVertex3d(r * Cos(veinAngle), r * Sin(veinAngle), 0.002);
      }
      glEnd();
   }
   
   // Secondary branching veins
   for (int i = 0; i < numVeins; i += 3)
   {
      double veinAngle = (i * 360.0 / numVeins);
      double branchStart = size * 0.4;
      
      for (int branch = -1; branch <= 1; branch += 2)
      {
         glBegin(GL_LINE_STRIP);
         glVertex3d(branchStart * Cos(veinAngle), branchStart * Sin(veinAngle), 0.002);
         double branchAngle = veinAngle + branch * 30;
         glVertex3d((branchStart + size * 0.2) * Cos(branchAngle), 
                    (branchStart + size * 0.2) * Sin(branchAngle), 0.002);
         glEnd();
      }
   }
   
   // Tip glow point
   float tipGlow[] = {0.22 * glowIntensity, 0.42 * glowIntensity, 0.32 * glowIntensity, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, tipGlow);
   glEnable(GL_LIGHTING);
   
   glPushMatrix();
   glTranslated(v1x, v1y, 0);
   glutSolidSphere(0.015 * glowIntensity, 8, 8);
   glPopMatrix();
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 *  Draw complete leaf ring layer with rotation
 */
static void DrawLeafRingLayer(int numLeaves, double radius, double leafSize, 
                             double layerHeight, double rotation, double time, 
                             double glowIntensity, int seed)
{
   for (int i = 0; i < numLeaves; i++)
   {
      double angle = (i * 360.0 / numLeaves) + rotation;
      
      glPushMatrix();
      glTranslated(0, layerHeight, 0);
      glRotated(angle, 0, 1, 0);
      glTranslated(radius, 0, 0);
      
      // Slight tilt for each leaf
      glRotated(15 + 10 * sin(time + i), 0, 0, 1);
      
      DrawComplexLeafTriangle(leafSize, 0, glowIntensity, seed + i * 100, time);
      
      glPopMatrix();
   }
}

/*
 *  Main function: Draw complete Geometric Alien Flora
 */
void GeometricAlienFlora(double x, double y, double z, double scale, 
                        double time, int colorVariant, int seed)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   
   // Set color variant
   float primaryColor[4], accentColor[4];
   if (colorVariant == 0) {
      // Amethyst violet
      primaryColor[0] = 0.82; primaryColor[1] = 0.28; 
      primaryColor[2] = 0.82; primaryColor[3] = 1.0;
   } else {
      // Aquatic jade-green
      primaryColor[0] = 0.22; primaryColor[1] = 0.82; 
      primaryColor[2] = 0.62; primaryColor[3] = 1.0;
   }
   accentColor[0] = 0.4; accentColor[1] = 0.9; 
   accentColor[2] = 0.7; accentColor[3] = 1.0;
   
   // Material properties
   float shininess = 8.0;
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, primaryColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, accentColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
   
   // Base contraction/expansion (breathing effect)
   double basePulse = 1.0 + 0.02 * sin(time * 2.0);
   
   glPushMatrix();
   glScaled(basePulse, basePulse, basePulse);
   
   // Draw crystalline cone base
   DrawFacetedConeBase(0.25, 0.35, 16, 8);
   
   glPopMatrix();
   
   // Draw root tendrils
   glColor3f(0.15, 0.1, 0.15);
   DrawRootTendrils(0.25, time, seed);
   
   // Stem sway calculation
   double swayAngle = 8.0 * sin(time * 0.6 + seed);
   
   // Draw flexible stem
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, primaryColor);
   DrawSegmentedStem(1.2, 0.03, swayAngle, time);
   
   // Leaf layers with counter-rotation
   int numLayers = 5;
   for (int layer = 0; layer < numLayers; layer++)
   {
      double layerHeight = 0.4 + layer * 0.18;
      int leavesInLayer = 6 + (layer % 2) * 3;
      double leafSize = 0.30 + layer * 0.05;
      double radius = 0.15 + layer * 0.03;
      
      // Each layer rotates at different speed and direction
      double layerRotation = time * (5.0 + layer * 2.0) * ((layer % 2) ? 1 : -1);
      layerRotation += seed * 10;
      
      // Glow intensity varies with sway
      double glowIntensity = 0.7 + 0.3 * sin(time * 1.5 + layer * 0.5);
      
      // Set leaf color
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, primaryColor);
      
      DrawLeafRingLayer(leavesInLayer, radius, leafSize, layerHeight, 
                       layerRotation, time, glowIntensity, seed + layer * 1000);
   }
   
   // Top crown of smaller leaves
   glPushMatrix();
   glTranslated(0, 1.3, 0);
   DrawLeafRingLayer(8, 0.1, 0.15, 0, time * 15.0, time, 1.0, seed + 5000);
   glPopMatrix();
   
   // Reset emission
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 *  Draw a cluster of flora
 */
void GeometricAlienFloraCluster(double centerX, double centerY, double centerZ,
                               double clusterRadius, int count, double time)
{
   for (int i = 0; i < count; i++)
   {
      double angle = (i * 360.0 / count) + time * 2.0;
      double dist = clusterRadius * (0.5 + 0.5 * (i % 3) / 3.0);
      
      double x = centerX + dist * Cos(angle);
      double z = centerZ + dist * Sin(angle);
      double scale = 0.6 + 0.6 * ((i % 4) / 4.0);
      int colorVariant = i % 2;
      
      GeometricAlienFlora(x, centerY, z, scale, time + i * 0.3, colorVariant, i * 42);
   }
}
