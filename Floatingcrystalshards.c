/*
 *  Floating Crystal Shards
 *
 *  Ancient lunar relic fragments suspended by unknown gravitational technology.
 *  Features multi-layered animation, spectral gradients, and refractive effects.
 *
 *  Created for: Lunar Outpost - Alien Invasion Survival Year 6060
 */

#include "CSCIx229.h"
#include <math.h>

/*
 *  Spectral color interpolation
 *  t ranges from 0.0 (cyan) to 1.0 (violet)
 */
static void SpectralColor(float t, float alpha)
{
   // Cyan (0.1, 0.9, 1.0) → Violet (0.9, 0.15, 1.0)
   float r = 0.1 + t * 0.8;
   float g = 0.9 - t * 0.75;
   float b = 1.0;
   glColor4f(r, g, b, alpha);
}

/*
 *  Draw a micro-faceted surface
 *  Creates hyper-detailed octahedron face with 24 subdivisions
 */
static void DrawMicroFacet(float x1, float y1, float z1,
                          float x2, float y2, float z2,
                          float x3, float y3, float z3,
                          float colorPhase, float alpha)
{
   // Calculate face normal
   float dx1 = x2 - x1, dy1 = y2 - y1, dz1 = z2 - z1;
   float dx2 = x3 - x1, dy2 = y3 - y1, dz2 = z3 - z1;
   float nx = dy1*dz2 - dz1*dy2;
   float ny = dz1*dx2 - dx1*dz2;
   float nz = dx1*dy2 - dy1*dx2;
   float len = sqrt(nx*nx + ny*ny + nz*nz);
   if (len > 0) {
      nx /= len; ny /= len; nz /= len;
   }
   
   // Subdivide into micro-facets (6x4 grid = 24 facets)
   int subdivX = 6, subdivY = 4;
   for (int i = 0; i < subdivX; i++) {
      for (int j = 0; j < subdivY; j++) {
         float u1 = i / (float)subdivX;
         float u2 = (i+1) / (float)subdivX;
         float v1 = j / (float)subdivY;
         float v2 = (j+1) / (float)subdivY;
         
         // Interpolate positions with slight perturbation for sparkle
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
         
         // Color varies slightly per micro-facet
         float facetPhase = colorPhase + (i*subdivY + j) * 0.02;
         facetPhase = fmod(facetPhase, 1.0);
         SpectralColor(facetPhase, alpha);
         
         // Edge highlighting with magenta
         if (i == 0 || j == 0 || i == subdivX-1 || j == subdivY-1) {
            glColor4f(1.0, 0.2, 0.8, alpha * 1.2); // Electric magenta
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
 *  Draw elongated octahedron core
 *  Central crystal structure with 8 faces
 */
static void DrawOctahedronCore(float length, float width, float height, 
                              float colorPhase, float alpha, int emissive)
{
   // Set material properties
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
   
   // Draw 8 triangular faces with micro-facets
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
 *  Draw inner refractive core
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
   
   // Draw small octahedron core
   DrawOctahedronCore(size * 0.3, size * 0.15, size * 0.15, 
                      colorPhase + 0.5, 0.2, 1);
   
   glPopMatrix();
}

/*
 *  Draw satellite micro-shard
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
 *  Draw volumetric light cone
 *  Soft glow emanating from the crystal
 */
static void DrawVolumeLightCone(float intensity)
{
   if (intensity <= 0) return;
   
   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for glow
   glDepthMask(GL_FALSE);
   
   int segments = 16;
   float coneHeight = 1.5;
   float coneRadius = 0.8;
   
   glBegin(GL_TRIANGLE_FAN);
   glColor4f(0.1, 0.4, 1.0, intensity * 0.3);
   glVertex3f(0, 0, 0); // Apex
   
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
 *
 *  Parameters:
 *    x, y, z       - Position in world space
 *    scale         - Overall size multiplier (0.5 to 2.0 recommended)
 *    time          - Animation time in seconds
 *    seed          - Random seed for variation (0-1000)
 *    drawTrails    - Whether to draw motion trails (0 or 1)
 *    facingCamera  - Camera angle in degrees for volumetric effect
 */
void FloatingCrystalShard(double x, double y, double z,
                          double scale, double time,
                          int seed, int drawTrails,
                          double facingCamera)
{
   // Save transformation
   glPushMatrix();
   
   // Position
   glTranslatef(x, y, z);
   
   // Variation based on seed
   float seedPhase = (seed % 100) / 100.0;
   float sizeVariation = 0.8 + seedPhase * 1.6; // 0.8 to 2.4 range
   float widthVariation = 0.15 + seedPhase * 0.2; // 0.15 to 0.35
   
   // ANIMATION LAYER 1: Vertical oscillating float
   // 1.5 second period sine wave
   float floatOffset = sin(time * 2.0 * M_PI / 1.5) * 0.3;
   glTranslatef(0, floatOffset, 0);
   
   // ANIMATION LAYER 2: Global rotation
   // 12 degrees per second = 30 second full rotation
   float rotationAngle = fmod(time * 12.0, 360.0);
   glRotatef(rotationAngle, 0.3, 1, 0.2); // Slight off-axis rotation
   
   // Scale
   glScalef(scale, scale, scale);
   
   // Volumetric light cone when facing camera
   float coneIntensity = cos(facingCamera * M_PI / 180.0);
   if (coneIntensity > 0) {
      DrawVolumeLightCone(coneIntensity * 0.5);
   }
   
   // Enable transparency and blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_LIGHTING);
   glEnable(GL_NORMALIZE);
   glDepthMask(GL_FALSE); // Don't write to depth buffer for transparency
   
   // Color phase animation
   float colorPhase = fmod(time * 0.1 + seedPhase, 1.0);
   
   // MAIN CRYSTAL SHELL (semi-transparent, alpha = 0.55)
   DrawOctahedronCore(sizeVariation, widthVariation, widthVariation,
                      colorPhase, 0.55, 1);
   
   // INNER REFRACTIVE CORE (very transparent, alpha = 0.2)
   DrawInnerCore(sizeVariation * 0.5, colorPhase, time);
   
   // ANIMATION LAYER 3: Orbiting satellite shards
   int numSatellites = 4 + (seed % 3); // 4-6 satellites
   float orbitSpeed = 8.0; // 8 degrees per second
   
   for (int i = 0; i < numSatellites; i++) {
      glPushMatrix();
      
      // Orbit radius varies from 0.3 to 0.6 units
      float orbitRadius = 0.3 + (((seed + i * 37) % 100) / 100.0) * 0.3;
      
      // Each satellite has different orbital phase
      float orbitAngle = fmod(time * orbitSpeed + i * (360.0 / numSatellites), 360.0);
      
      // Orbit in tilted plane
      glRotatef(orbitAngle, 0, 1, 0);
      glRotatef(30 + i * 15, 1, 0, 0); // Varied orbital planes
      glTranslatef(orbitRadius, 0, 0);
      
      // Draw satellite with trail effect
      if (drawTrails) {
         // Draw faint prism trail
         glDisable(GL_LIGHTING);
         glBegin(GL_LINES);
         for (int t = 1; t <= 8; t++) {
            float trailPhase = t / 8.0;
            float prevAngle = orbitAngle - t * 2.0;
            float px = orbitRadius * cos(prevAngle * M_PI / 180.0);
            float pz = orbitRadius * sin(prevAngle * M_PI / 180.0);
            
            SpectralColor(colorPhase + trailPhase * 0.3, (1.0 - trailPhase) * 0.3);
            glVertex3f(px, 0, pz);
         }
         glEnd();
         glEnable(GL_LIGHTING);
      }
      
      // Satellite shard
      float satellitePhase = colorPhase + i * 0.15;
      DrawSatelliteShard(sizeVariation, satellitePhase);
      
      glPopMatrix();
   }
   
   // Restore states
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   
   // Reset emission
   float black[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   
   // Restore transformation
   glPopMatrix();
}

/*
 *  Convenience function: Draw multiple crystals in a cluster
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
      
      float scale = 0.5 + (i % 3) * 0.3; // Varied sizes
      
      FloatingCrystalShard(x, y, z, scale, time + i * 0.5,
                          i * 123, drawTrails, facingCamera);
   }
}

/*
 *  Demo display function for testing
 *  (Remove or comment out when integrating into main game)
 */
#ifdef CRYSTAL_DEMO
void display()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   
   // Camera setup
   double Ex = 5 * sin(glutGet(GLUT_ELAPSED_TIME) / 2000.0);
   double Ez = 5 * cos(glutGet(GLUT_ELAPSED_TIME) / 2000.0);
   gluLookAt(Ex, 2, Ez, 0, 0, 0, 0, 1, 0);
   
   // Lighting
   float Ambient[]  = {0.2, 0.2, 0.3, 1.0};
   float Diffuse[]  = {0.6, 0.6, 0.8, 1.0};
   float Specular[] = {1.0, 1.0, 1.0, 1.0};
   float Position[] = {3, 5, 3, 1};
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glLightfv(GL_LIGHT0, GL_AMBIENT,  Ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  Diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
   glLightfv(GL_LIGHT0, GL_POSITION, Position);
   
   // Get time
   double time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
   
   // Calculate facing angle
   double facingAngle = atan2(Ez, Ex) * 180.0 / M_PI;
   
   // Draw single crystal
   FloatingCrystalShard(0, 0, 0, 1.0, time, 42, 1, facingAngle);
   
   // Draw cluster
   FloatingCrystalCluster(3, 1, 2, 1.5, 5, time, 0, facingAngle);
   
   glFlush();
   glutSwapBuffers();
}

void idle()
{
   glutPostRedisplay();
}

void reshape(int width, int height)
{
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45, (double)width/height, 0.1, 100);
   glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(800, 600);
   glutCreateWindow("Floating Crystal Shards - Demo");
   
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutIdleFunc(idle);
   
   glutMainLoop();
   return 0;
}
#endif