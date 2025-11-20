/*
 *  Classic UFO Spacecraft
 */
#include "CSCIx229.h"

/*
 *  Drawing a torus (donut shape)
 */
// static void DrawTorus(double outerRadius, double innerRadius, int sides, int rings)
// {
//    for (int i = 0; i < sides; i++)
//    {
//       glBegin(GL_QUAD_STRIP);
//       for (int j = 0; j <= rings; j++)
//       {
//          for (int k = 0; k <= 1; k++)
//          {
//             double s = (i + k) % sides + 0.5;
//             double t = j % rings;
            
//             double phi = 2.0 * M_PI * s / sides;
//             double theta = 2.0 * M_PI * t / rings;
            
//             double x = (outerRadius + innerRadius * cos(theta)) * cos(phi);
//             double y = innerRadius * sin(theta);
//             double z = (outerRadius + innerRadius * cos(theta)) * sin(phi);
            
//             // Normal calculation
//             double nx = cos(theta) * cos(phi);
//             double ny = sin(theta);
//             double nz = cos(theta) * sin(phi);
            
//             glNormal3d(nx, ny, nz);
//             glVertex3d(x, y, z);
//          }
//       }
//       glEnd();
//    }
// }

/*
 *  Drawing highly detailed central dome with segments
 */
static void DrawCentralDome(double radius, double height, int segments)
{
   // Multi-layered dome with metallic segments
   int layers = 8;
   
   for (int layer = 0; layer < layers; layer++)
   {
      double h0 = (layer / (double)layers) * height;
      double h1 = ((layer + 1) / (double)layers) * height;
      double r0 = radius * (1.0 - h0 / height);
      double r1 = radius * (1.0 - h1 / height);
      
      // Adding panel detail with slight radius variation
      r0 *= (1.0 + 0.02 * sin(layer * 2.0));
      r1 *= (1.0 + 0.02 * sin((layer + 1) * 2.0));
      
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= segments; i++)
      {
         double angle = (i * 360.0 / segments);
         
         // Calculating normals for smooth shading
         double nx0 = Cos(angle) * 0.7;
         double ny0 = 0.7;
         double nz0 = Sin(angle) * 0.7;
         double len0 = sqrt(nx0*nx0 + ny0*ny0 + nz0*nz0);
         
         double nx1 = Cos(angle) * 0.7;
         double ny1 = 0.7;
         double nz1 = Sin(angle) * 0.7;
         double len1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1);
         
         glNormal3d(nx0/len0, ny0/len0, nz0/len0);
         glVertex3d(r0 * Cos(angle), h0, r0 * Sin(angle));
         
         glNormal3d(nx1/len1, ny1/len1, nz1/nz1);
         glVertex3d(r1 * Cos(angle), h1, r1 * Sin(angle));
      }
      glEnd();
      
      // Panel separation lines
      glDisable(GL_LIGHTING);
      glColor3f(0.1, 0.15, 0.2);
      glLineWidth(1.5);
      glBegin(GL_LINE_LOOP);
      for (int i = 0; i <= segments; i++)
      {
         double angle = (i * 360.0 / segments);
         glVertex3d(r0 * Cos(angle), h0 + 0.001, r0 * Sin(angle));
      }
      glEnd();
      glEnable(GL_LIGHTING);
   }
   
   // Top antenna/sensor
   glPushMatrix();
   glTranslated(0, height, 0);
   
   float antennaColor[] = {0.3, 0.35, 0.4, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, antennaColor);
   
   // Antenna base
   GLUquadric* quad = gluNewQuadric();
   gluCylinder(quad, 0.08, 0.06, 0.15, 16, 1);
   
   // Antenna tip
   glTranslated(0, 0, 0.15);
   glutSolidSphere(0.05, 12, 12);
   gluDeleteQuadric(quad);
   
   glPopMatrix();
}

/*
 *  Drawing main saucer with layered construction
 */
static void DrawMainHull(double radius, double thickness)
{
   int segments = 64;
   
   // Top surface - slightly domed
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(0, 1, 0);
   glVertex3d(0, thickness * 0.3, 0);
   for (int i = 0; i <= segments; i++)
   {
      double angle = (i * 360.0 / segments);
      double r = radius * 0.85;
      double domeHeight = thickness * 0.3 - 0.05 * (r / radius);
      
      glNormal3d(Cos(angle) * 0.2, 0.98, Sin(angle) * 0.2);
      glVertex3d(r * Cos(angle), domeHeight, r * Sin(angle));
   }
   glEnd();
   
   // Upper rim section with panel detail
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = (i * 360.0 / segments);
      double r0 = radius * 0.85;
      double r1 = radius;
      
      // Normal pointing outward and slightly up
      double nx = Cos(angle);
      double ny = 0.3;
      double nz = Sin(angle);
      double len = sqrt(nx*nx + ny*ny + nz*nz);
      
      glNormal3d(nx/len, ny/len, nz/len);
      glVertex3d(r0 * Cos(angle), thickness * 0.2, r0 * Sin(angle));
      glVertex3d(r1 * Cos(angle), 0, r1 * Sin(angle));
   }
   glEnd();
   
   // Central band (red accent stripe)
   float redStripe[] = {0.8, 0.15, 0.1, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, redStripe);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
   
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = (i * 360.0 / segments);
      
      glNormal3d(Cos(angle), 0, Sin(angle));
      glVertex3d(radius * Cos(angle), 0, radius * Sin(angle));
      glVertex3d(radius * Cos(angle), -0.08, radius * Sin(angle));
   }
   glEnd();
   
   
   float hullColor[] = {0.25, 0.3, 0.35, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hullColor);
   
   // Lower rim section
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = (i * 360.0 / segments);
      double r0 = radius;
      double r1 = radius * 0.85;
      
      double nx = Cos(angle);
      double ny = -0.3;
      double nz = Sin(angle);
      double len = sqrt(nx*nx + ny*ny + nz*nz);
      
      glNormal3d(nx/len, ny/len, nz/len);
      glVertex3d(r0 * Cos(angle), -0.08, r0 * Sin(angle));
      glVertex3d(r1 * Cos(angle), -thickness * 0.2, r1 * Sin(angle));
   }
   glEnd();
   
   // Bottom surface - flat with central indent
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(0, -1, 0);
   glVertex3d(0, -thickness * 0.35, 0);
   for (int i = segments; i >= 0; i--)
   {
      double angle = (i * 360.0 / segments);
      double r = radius * 0.85;
      glVertex3d(r * Cos(angle), -thickness * 0.2, r * Sin(angle));
   }
   glEnd();
   
   // Panel lines on top surface
   glDisable(GL_LIGHTING);
   glColor3f(0.15, 0.18, 0.22);
   glLineWidth(1.0);
   
   // Radial panel lines
   for (int i = 0; i < 16; i++)
   {
      double angle = i * 22.5;
      glBegin(GL_LINES);
      glVertex3d(0, thickness * 0.3 + 0.001, 0);
      glVertex3d(radius * 0.85 * Cos(angle), thickness * 0.2 + 0.001, 
                 radius * 0.85 * Sin(angle));
      glEnd();
   }
   
   // Concentric circles
   for (int ring = 1; ring <= 3; ring++)
   {
      double r = radius * 0.85 * ring / 3.5;
      glBegin(GL_LINE_LOOP);
      for (int i = 0; i <= segments; i++)
      {
         double angle = (i * 360.0 / segments);
         glVertex3d(r * Cos(angle), thickness * 0.25 + 0.001, r * Sin(angle));
      }
      glEnd();
   }
   
   glEnable(GL_LIGHTING);
}

/*
 *  Drawing rotating light ports with glow
 */
static void DrawLightPort(double x, double y, double z, double time, 
                         float* color, int index, double intensity)
{
   glPushMatrix();
   glTranslated(x, y, z);
   
   // Outer rim (dark)
   float rimColor[] = {0.15, 0.15, 0.2, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rimColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
   
   glPushMatrix();
   glScaled(1.0, 0.3, 1.0);
   glutSolidSphere(0.12, 16, 16);
   glPopMatrix();
   
   // Inner glowing lens
   float glowColor[] = {color[0] * intensity, color[1] * intensity, 
                        color[2] * intensity, 1.0};
   float emission[] = {color[0] * intensity * 0.8, color[1] * intensity * 0.8, 
                       color[2] * intensity * 0.8, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glowColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
   
   glPushMatrix();
   glScaled(1.0, 0.4, 1.0);
   glutSolidSphere(0.09, 16, 16);
   glPopMatrix();
   
   // Glow halo effect
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glDepthMask(GL_FALSE);
   
   glColor4f(color[0], color[1], color[2], 0.3 * intensity);
   glPushMatrix();
   glScaled(1.0, 0.2, 1.0);
   glutSolidSphere(0.18, 12, 12);
   glPopMatrix();
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 *  Drawing bottom tractor beam with volumetric effect
 */
static void DrawTractorBeam(double time, double intensity)
{
   if (intensity < 0.1) return;
   
   glPushMatrix();
   glTranslated(0, -0.5, 0);
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glDepthMask(GL_FALSE);
   glDisable(GL_LIGHTING);
   
   // Beam colors - gradient from yellow to cyan
   int segments = 32;
   int layers = 15;
   
   for (int layer = 0; layer < layers; layer++)
   {
      double t = layer / (double)layers;
      double h = -t * 8.0;  // Beam extends 8 units down
      double r0 = 0.3 + t * 2.5;  // Expands as it goes down
      double r1 = 0.3 + (t + 1.0/layers) * 2.5;
      
      // Color gradient and pulsation
      double pulse = 0.7 + 0.3 * sin(time * 3.0 - t * 6.28);
      float alpha = (1.0 - t) * 0.15 * intensity * pulse;
      
      // Yellow at top, cyan at bottom
      float r = 1.0 - t * 0.7;
      float g = 0.9 + t * 0.1;
      float b = 0.3 + t * 0.7;
      
      glColor4f(r, g, b, alpha);
      
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= segments; i++)
      {
         double angle = (i * 360.0 / segments) + time * 30.0;
         glVertex3d(r0 * Cos(angle), h, r0 * Sin(angle));
         glVertex3d(r1 * Cos(angle), h - 8.0/layers, r1 * Sin(angle));
      }
      glEnd();
   }
   
   // Particle effects in beam
   glPointSize(3.0);
   glBegin(GL_POINTS);
   for (int i = 0; i < 50; i++)
   {
      double angle = (i * 37.0) + time * 50.0;
      double r = 0.5 + (i % 7) * 0.3;
      double h = -((time * 2.0 + i * 0.1) - (int)(time * 2.0 + i * 0.1)) * 8.0;
      
      glColor4f(1.0, 1.0, 0.7, 0.6 * intensity);
      glVertex3d(r * Cos(angle), h, r * Sin(angle));
   }
   glEnd();
   glPointSize(1.0);
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
   
   glPopMatrix();
}

/*
 *  Drawing bottom central hub with vents
 */
static void DrawBottomHub(double time)
{
   glPushMatrix();
   glTranslated(0, -0.45, 0);
   
   float hubColor[] = {0.2, 0.25, 0.3, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hubColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);
   
   // Central cylinder
   glPushMatrix();
   glRotated(-90, 1, 0, 0);
   GLUquadric* quad = gluNewQuadric();
   gluCylinder(quad, 0.35, 0.25, 0.2, 32, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();
   
   // Bottom cap with vent pattern
   glPushMatrix();
   glTranslated(0, -0.2, 0);
   
   // Concentric vent rings
   glDisable(GL_LIGHTING);
   glColor3f(0.1, 0.1, 0.15);
   glLineWidth(2.0);
   
   for (int ring = 1; ring <= 4; ring++)
   {
      double r = 0.08 * ring;
      glBegin(GL_LINE_LOOP);
      for (int i = 0; i <= 32; i++)
      {
         double angle = i * 11.25;
         glVertex3d(r * Cos(angle), -0.01, r * Sin(angle));
      }
      glEnd();
   }
   
   // Radial vents
   for (int i = 0; i < 12; i++)
   {
      double angle = i * 30.0;
      glBegin(GL_LINES);
      glVertex3d(0, -0.01, 0);
      glVertex3d(0.3 * Cos(angle), -0.01, 0.3 * Sin(angle));
      glEnd();
   }
   
   glEnable(GL_LIGHTING);
   glPopMatrix();
   
   glPopMatrix();
}

/*
 *  Main function: To draw complete UFO spacecraft
 */
void UFOSpacecraft(double x, double y, double z, double scale, 
                   double time, double rotationSpeed, int tractorBeamOn)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   
   // Gentle hovering motion
   double hover = 0.15 * sin(time * 1.2);
   glTranslated(0, hover, 0);
   
   // Rotation
   glRotated(time * rotationSpeed, 0, 1, 0);
   
   // Main hull material - metallic dark teal
   float hullColor[] = {0.25, 0.3, 0.35, 1.0};
   float hullSpec[] = {0.7, 0.75, 0.8, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hullColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hullSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 90.0);
   
   // Drawing main saucer hull
   DrawMainHull(2.0, 0.5);
   
   // Drawing central dome
   glPushMatrix();
   glTranslated(0, 0.15, 0);
   
   float domeColor[] = {0.3, 0.35, 0.4, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, domeColor);
   DrawCentralDome(0.8, 0.6, 32);
   glPopMatrix();
   
   // Drawing rotating light ports (6 around perimeter)
   int numLights = 6;
   for (int i = 0; i < numLights; i++)
   {
      double angle = (i * 360.0 / numLights) + time * 40.0;  // Counter-rotate
      double radius = 1.85;
      
      // Alternate colors: magenta, cyan, magenta, cyan...
      float lightColor[3];
      double intensity = 0.7 + 0.3 * sin(time * 4.0 + i * 1.0);
      
      if (i % 2 == 0) {
         // Magenta/pink
         lightColor[0] = 0.9; lightColor[1] = 0.3; lightColor[2] = 0.7;
      } else {
         // Cyan
         lightColor[0] = 0.3; lightColor[1] = 0.8; lightColor[2] = 0.9;
      }
      
      DrawLightPort(radius * Cos(angle), -0.05, radius * Sin(angle), 
                   time, lightColor, i, intensity);
   }
   
   // Drawing bottom hub
   DrawBottomHub(time);
   
   // Drawing tractor beam if active
   if (tractorBeamOn)
   {
      double beamIntensity = 0.8 + 0.2 * sin(time * 2.5);
      DrawTractorBeam(time, beamIntensity);
   }
   
   glPopMatrix();
}
