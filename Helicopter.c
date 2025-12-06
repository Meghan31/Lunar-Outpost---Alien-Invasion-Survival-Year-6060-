/*
 *  Helicopter.c
 *  Renders an EC135-style rescue helicopter with smoothly connected parts
 */
#include "CSCIx229.h"
#include <math.h>

/* Stores texture handles */
static unsigned int heliBodyTexture = 0;
static unsigned int heliBackTexture = 0;
static unsigned int metalPlateTexture = 0;
static unsigned int blueMetalTexture = 0;
static int texturesLoaded = 0;

/* Defines helicopter dimensions shared across parts */
#define CABIN_LENGTH 2.2
#define CABIN_WIDTH 0.95
#define CABIN_HEIGHT 0.85
#define TAIL_LENGTH 2.4
#define TAIL_START_RADIUS 0.38
#define TAIL_END_RADIUS 0.12
#define SKID_WIDTH 1.1
#define SKID_LENGTH 2.0
#define MAIN_ROTOR_RADIUS 2.2
#define TAIL_ROTOR_RADIUS 0.32

/*
 * Initializes helicopter textures
 */
void InitHelicopterTextures(void)
{
   if (texturesLoaded) return;
   heliBodyTexture = LoadTexBMP("textures/helibody.bmp");
   heliBackTexture = LoadTexBMP("textures/heliback.bmp");
   metalPlateTexture = LoadTexBMP("textures/metals/metal_plate.bmp");
   blueMetalTexture = LoadTexBMP("textures/metals/blue_metal.bmp");
   texturesLoaded = 1;
}

/*
 * Calculates smooth interpolation
 */
static double SmoothStep(double t)
{
   return t * t * (3.0 - 2.0 * t);
}

/*
 * Returns the fuselage cross-section at a position along the length
 * This keeps a smooth shape from nose to tail
 */
static void GetFuselageProfile(double t, double* width, double* height, double* yOffset)
{
   /* t ranges from 0 at the nose to 1 at the tail connection */
   
   if (t < 0.15) {
      /* Nose cone tapers to a point */
      double s = t / 0.15;
      s = SmoothStep(s);
      *width = 0.25 + s * (CABIN_WIDTH * 0.5 - 0.25);
      *height = 0.3 + s * (CABIN_HEIGHT * 0.5 - 0.3);
      *yOffset = -0.1 * (1.0 - s);
   }
   else if (t < 0.65) {
      /* Main cabin stays full width */
      *width = CABIN_WIDTH * 0.5;
      *height = CABIN_HEIGHT * 0.5;
      *yOffset = 0;
   }
   else {
      /* Tail transition eases into the boom */
      double s = (t - 0.65) / 0.35;
      s = SmoothStep(s);
      *width = CABIN_WIDTH * 0.5 * (1.0 - s) + TAIL_START_RADIUS * s;
      *height = CABIN_HEIGHT * 0.5 * (1.0 - s) + TAIL_START_RADIUS * s;
      *yOffset = 0.15 * s;
   }
}

/*
 * Draws the fuselage body with connected quad strips
 */
static void DrawFuselageBody(void)
{
   int lengthSegments = 32;
   int circumSegments = 24;
   
   float bodyColor[] = {0.95, 0.78, 0.12, 1.0};
   float bodySpec[] = {0.5, 0.45, 0.3, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bodyColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bodySpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, heliBodyTexture);
   
   double zStart = CABIN_LENGTH * 0.5;
   
   for (int i = 0; i < lengthSegments; i++)
   {
      double t0 = (double)i / lengthSegments;
      double t1 = (double)(i + 1) / lengthSegments;
      
      double z0 = zStart - t0 * CABIN_LENGTH;
      double z1 = zStart - t1 * CABIN_LENGTH;
      
      double w0, h0, yOff0;
      double w1, h1, yOff1;
      GetFuselageProfile(t0, &w0, &h0, &yOff0);
      GetFuselageProfile(t1, &w1, &h1, &yOff1);
      
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= circumSegments; j++)
      {
         double angle = (double)j / circumSegments * 2.0 * M_PI;
         
         /* Slightly flattens bottom for a more natural shape */
         double flattenFactor = (sin(angle) < -0.3) ? 0.85 : 1.0;
         
         double x0 = w0 * cos(angle);
         double y0 = h0 * sin(angle) * flattenFactor + yOff0;
         double x1 = w1 * cos(angle);
         double y1 = h1 * sin(angle) * flattenFactor + yOff1;
         
         /* Calculates the local normals */
         double nx = cos(angle) / w0;
         double ny = sin(angle) * flattenFactor / h0;
         double len = sqrt(nx*nx + ny*ny);
         nx /= len; ny /= len;
         
         glNormal3d(nx, ny, 0);
         glTexCoord2f((float)j / circumSegments, t0);
         glVertex3d(x0, y0, z0);
         
         glTexCoord2f((float)j / circumSegments, t1);
         glVertex3d(x1, y1, z1);
      }
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
}

/*
 * Draws the tail boom as a smooth tapered tube
 */
static void DrawTailBoom(void)
{
   int lengthSegments = 20;
   int circumSegments = 16;
   
   float boomColor[] = {0.93, 0.76, 0.10, 1.0};
   float boomSpec[] = {0.45, 0.4, 0.28, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, boomColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, boomSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 55.0);
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, heliBackTexture);
   
   double zStart = -CABIN_LENGTH * 0.5;
   double yStart = 0.15;
   
   for (int i = 0; i < lengthSegments; i++)
   {
      double t0 = (double)i / lengthSegments;
      double t1 = (double)(i + 1) / lengthSegments;
      
      /* Smoothly tapers the radius */
      double r0 = TAIL_START_RADIUS * (1.0 - t0) + TAIL_END_RADIUS * t0;
      double r1 = TAIL_START_RADIUS * (1.0 - t1) + TAIL_END_RADIUS * t1;
      
      /* Adds a slight upward curve for the tail */
      double curve0 = 0.08 * t0 * t0;
      double curve1 = 0.08 * t1 * t1;
      
      double z0 = zStart - t0 * TAIL_LENGTH;
      double z1 = zStart - t1 * TAIL_LENGTH;
      double y0 = yStart + curve0;
      double y1 = yStart + curve1;
      
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= circumSegments; j++)
      {
         double angle = (double)j / circumSegments * 2.0 * M_PI;
         
         double x0 = r0 * cos(angle);
         double yp0 = r0 * sin(angle) + y0;
         double x1 = r1 * cos(angle);
         double yp1 = r1 * sin(angle) + y1;
         
         double nx = cos(angle);
         double ny = sin(angle);
         
         glNormal3d(nx, ny, 0.05);
         glTexCoord2f((float)j / circumSegments, t0);
         glVertex3d(x0, yp0, z0);
         
         glTexCoord2f((float)j / circumSegments, t1);
         glVertex3d(x1, yp1, z1);
      }
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
}

/*
 * Draws the engine cowling on top and keeps it blended into the body
 */
static void DrawEngineCowling(void)
{
   float cowlColor[] = {0.90, 0.72, 0.08, 1.0};
   float cowlSpec[] = {0.55, 0.5, 0.35, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cowlColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cowlSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, heliBodyTexture);
   
   int segments = 20;
   double cowlLength = 0.9;
   double cowlWidth = 0.32;
   double cowlHeight = 0.22;
   double yBase = CABIN_HEIGHT * 0.42;
   double zCenter = -0.15;
   
   /* Shapes the cowling as an elongated dome */
   for (int i = 0; i < segments; i++)
   {
      double t0 = (double)i / segments;
      double t1 = (double)(i + 1) / segments;
      
      double z0 = zCenter + cowlLength * 0.5 - t0 * cowlLength;
      double z1 = zCenter + cowlLength * 0.5 - t1 * cowlLength;
      
      /* Keeps an elliptical profile along the length */
      double profileT0 = 2.0 * t0 - 1.0;
      double profileT1 = 2.0 * t1 - 1.0;
      double h0 = cowlHeight * sqrt(fmax(0, 1.0 - profileT0 * profileT0));
      double h1 = cowlHeight * sqrt(fmax(0, 1.0 - profileT1 * profileT1));
      double w0 = cowlWidth * sqrt(fmax(0, 1.0 - profileT0 * profileT0 * 0.3));
      double w1 = cowlWidth * sqrt(fmax(0, 1.0 - profileT1 * profileT1 * 0.3));
      
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= 12; j++)
      {
         double angle = (double)j / 12 * M_PI;  /* This loop covers only the top half */
         
         double x0 = w0 * cos(angle);
         double y0 = yBase + h0 * sin(angle);
         double x1 = w1 * cos(angle);
         double y1 = yBase + h1 * sin(angle);
         
         double nx = cos(angle) * cowlHeight;
         double ny = sin(angle) * cowlWidth;
         double len = sqrt(nx*nx + ny*ny);
         
         glNormal3d(nx/len, ny/len, 0);
         glTexCoord2f(t0, (float)j / 12);
         glVertex3d(x0, y0, z0);
         glTexCoord2f(t1, (float)j / 12);
         glVertex3d(x1, y1, z1);
      }
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
   
   /* Adds exhaust outlets */
   float exhaustColor[] = {0.12, 0.12, 0.14, 1.0};
   float exhaustSpec[] = {0.6, 0.6, 0.65, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, exhaustColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, exhaustSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80.0);
   
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.22, yBase + cowlHeight * 0.6, zCenter - cowlLength * 0.35);
      glRotated(90 - side * 20, 1, 0, 0);
      glRotated(side * 15, 0, 0, 1);

      /* Draws the exhaust pipe */
      gluCylinder(quad, 0.055, 0.048, 0.18, 14, 2);
      
      /* Shades the exhaust interior */
      float innerColor[] = {0.03, 0.03, 0.04, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, innerColor);
      glTranslated(0, 0, 0.18);
      gluDisk(quad, 0.02, 0.048, 14, 1);
      
      glPopMatrix();
   }
   
   gluDeleteQuadric(quad);
}

/*
 * Draws the cockpit with its glass canopy
 */
static void DrawCockpit(void)
{
   /* Cockpit frame uses a dark finish */
   float frameColor[] = {0.06, 0.06, 0.08, 1.0};
   float frameSpec[] = {0.3, 0.3, 0.35, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, frameColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, frameSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);
   
   /* Sets up the window frame pillars */
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   
   /* Draws the center pillar */
   glPushMatrix();
   glTranslated(0, -0.15, CABIN_LENGTH * 0.38);
   glRotated(-65, 1, 0, 0);
   gluCylinder(quad, 0.022, 0.018, 0.55, 10, 2);
   glPopMatrix();
   
   /* Draws the side pillars */
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.42, -0.18, CABIN_LENGTH * 0.32);
      glRotated(-70, 1, 0, 0);
      glRotated(side * 12, 0, 0, 1);
      gluCylinder(quad, 0.02, 0.016, 0.58, 10, 2);
      glPopMatrix();
   }
   
   gluDeleteQuadric(quad);
   
   /* Configures the glass panels */
   float glassColor[] = {0.15, 0.22, 0.32, 0.72};
   float glassSpec[] = {0.98, 0.98, 1.0, 1.0};
   float glassEmit[] = {0.015, 0.025, 0.04, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glassColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpec);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glassEmit);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);
   
   /* Forms the main curved windscreen */
   int uSegs = 12;
   int vSegs = 8;
   
   for (int side = -1; side <= 1; side += 2)
   {
      glBegin(GL_QUADS);
      for (int i = 0; i < uSegs; i++)
      {
         for (int j = 0; j < vSegs; j++)
         {
            double u0 = (double)i / uSegs;
            double u1 = (double)(i + 1) / uSegs;
            double v0 = (double)j / vSegs;
            double v1 = (double)(j + 1) / vSegs;
            
            /* Uses curved windscreen coordinates */
            double xBase = side * 0.02;
            double xEnd = side * 0.44;
            double zFront = CABIN_LENGTH * 0.48;
            double zBack = CABIN_LENGTH * 0.15;
            double yBottom = -0.22;
            double yTop = 0.32;
            
            double x0 = xBase + u0 * (xEnd - xBase);
            double x1 = xBase + u1 * (xEnd - xBase);
            double z0 = zFront - v0 * (zFront - zBack);
            double z1 = zFront - v1 * (zFront - zBack);
            
            /* Applies a curved profile */
            double curve0 = 0.12 * sin(u0 * M_PI) * (1.0 - v0 * 0.5);
            double curve1 = 0.12 * sin(u1 * M_PI) * (1.0 - v1 * 0.5);
            
            double y0 = yBottom + v0 * (yTop - yBottom);
            double y1 = yBottom + v1 * (yTop - yBottom);
            
            /* Calculates the normals */
            double nx = side * 0.4;
            double ny = 0.3;
            double nz = 0.85 - v0 * 0.3;
            double len = sqrt(nx*nx + ny*ny + nz*nz);
            glNormal3d(nx/len, ny/len, nz/len);
            
            glVertex3d(x0, y0, z0 + curve0);
            glVertex3d(x1, y0, z0 + curve1);
            glVertex3d(x1, y1, z1 + curve1 * 0.7);
            glVertex3d(x0, y1, z1 + curve0 * 0.7);
         }
      }
      glEnd();
   }
   
   /* Adds the chin bubble windows */
   glBegin(GL_QUADS);
   glNormal3d(0, -0.65, 0.75);
   glVertex3d(-0.38, -0.38, CABIN_LENGTH * 0.35);
   glVertex3d(0.38, -0.38, CABIN_LENGTH * 0.35);
   glVertex3d(0.44, -0.22, CABIN_LENGTH * 0.48);
   glVertex3d(-0.44, -0.22, CABIN_LENGTH * 0.48);
   glEnd();
   
   /* Adds the side cabin windows */
   for (int side = -1; side <= 1; side += 2)
   {
      glBegin(GL_QUADS);
      glNormal3d(side, 0.15, 0);
      glVertex3d(side * 0.475, -0.2, CABIN_LENGTH * 0.25);
      glVertex3d(side * 0.475, -0.2, -CABIN_LENGTH * 0.15);
      glVertex3d(side * 0.46, 0.28, -CABIN_LENGTH * 0.1);
      glVertex3d(side * 0.46, 0.28, CABIN_LENGTH * 0.2);
      glEnd();
   }
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   
   float noEmit[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmit);
}

/*
 * Draws the vertical and horizontal stabilizers on the tail
 */
static void DrawStabilizers(void)
{
   float finColor[] = {0.92, 0.74, 0.08, 1.0};
   float finSpec[] = {0.45, 0.4, 0.3, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, finColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, finSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, heliBackTexture);
   
   double tailEnd = -CABIN_LENGTH * 0.5 - TAIL_LENGTH;
   double tailY = 0.15 + 0.08;  /* Matches the tail boom curve */
   
   /* Vertical stabilizer keeps an airfoil shape */
   double finThickness = 0.035;
   double finHeight = 0.55;
   double finChordRoot = 0.45;
   double finChordTip = 0.25;
   
   /* Draws the right side of the vertical fin */
   glBegin(GL_QUADS);
   glNormal3d(1, 0, 0);
   glTexCoord2f(0, 0);
   glVertex3d(finThickness, tailY + TAIL_END_RADIUS, tailEnd + 0.1);
   glTexCoord2f(1, 0);
   glVertex3d(finThickness, tailY + TAIL_END_RADIUS, tailEnd + 0.1 - finChordRoot);
   glTexCoord2f(1, 1);
   glVertex3d(finThickness * 0.6, tailY + TAIL_END_RADIUS + finHeight, tailEnd + 0.05 - finChordTip);
   glTexCoord2f(0, 1);
   glVertex3d(finThickness * 0.6, tailY + TAIL_END_RADIUS + finHeight, tailEnd + 0.05);
   glEnd();
   
   /* Draws the left side of the vertical fin */
   glBegin(GL_QUADS);
   glNormal3d(-1, 0, 0);
   glTexCoord2f(0, 0);
   glVertex3d(-finThickness, tailY + TAIL_END_RADIUS, tailEnd + 0.1);
   glTexCoord2f(0, 1);
   glVertex3d(-finThickness * 0.6, tailY + TAIL_END_RADIUS + finHeight, tailEnd + 0.05);
   glTexCoord2f(1, 1);
   glVertex3d(-finThickness * 0.6, tailY + TAIL_END_RADIUS + finHeight, tailEnd + 0.05 - finChordTip);
   glTexCoord2f(1, 0);
   glVertex3d(-finThickness, tailY + TAIL_END_RADIUS, tailEnd + 0.1 - finChordRoot);
   glEnd();
   
   /* Outlines the leading edge */
   glBegin(GL_QUADS);
   glNormal3d(0, 0.3, 1);
   glVertex3d(-finThickness, tailY + TAIL_END_RADIUS, tailEnd + 0.1);
   glVertex3d(finThickness, tailY + TAIL_END_RADIUS, tailEnd + 0.1);
   glVertex3d(finThickness * 0.6, tailY + TAIL_END_RADIUS + finHeight, tailEnd + 0.05);
   glVertex3d(-finThickness * 0.6, tailY + TAIL_END_RADIUS + finHeight, tailEnd + 0.05);
   glEnd();
   
   /* Outlines the trailing edge */
   glBegin(GL_QUADS);
   glNormal3d(0, 0.2, -1);
   glVertex3d(-finThickness, tailY + TAIL_END_RADIUS, tailEnd + 0.1 - finChordRoot);
   glVertex3d(-finThickness * 0.6, tailY + TAIL_END_RADIUS + finHeight, tailEnd + 0.05 - finChordTip);
   glVertex3d(finThickness * 0.6, tailY + TAIL_END_RADIUS + finHeight, tailEnd + 0.05 - finChordTip);
   glVertex3d(finThickness, tailY + TAIL_END_RADIUS, tailEnd + 0.1 - finChordRoot);
   glEnd();
   
   /* Handles the horizontal stabilizers */
   double hstabSpan = 0.48;
   double hstabChordRoot = 0.28;
   double hstabChordTip = 0.18;
   double hstabThickness = 0.025;
   double hstabY = tailY + TAIL_END_RADIUS * 0.8;
   double hstabZ = tailEnd + 0.02;
   
   for (int side = -1; side <= 1; side += 2)
   {
      /* Draws the top surface */
      glBegin(GL_QUADS);
      glNormal3d(0, 1, 0);
      glTexCoord2f(0, 0);
      glVertex3d(0, hstabY + hstabThickness, hstabZ);
      glTexCoord2f(1, 0);
      glVertex3d(side * hstabSpan, hstabY + hstabThickness * 0.7, hstabZ + 0.03);
      glTexCoord2f(1, 1);
      glVertex3d(side * hstabSpan * 0.85, hstabY + hstabThickness * 0.7, hstabZ - hstabChordTip);
      glTexCoord2f(0, 1);
      glVertex3d(0, hstabY + hstabThickness, hstabZ - hstabChordRoot);
      glEnd();
      
      /* Draws the bottom surface */
      glBegin(GL_QUADS);
      glNormal3d(0, -1, 0);
      glTexCoord2f(0, 0);
      glVertex3d(0, hstabY - hstabThickness, hstabZ);
      glTexCoord2f(0, 1);
      glVertex3d(0, hstabY - hstabThickness, hstabZ - hstabChordRoot);
      glTexCoord2f(1, 1);
      glVertex3d(side * hstabSpan * 0.85, hstabY - hstabThickness * 0.7, hstabZ - hstabChordTip);
      glTexCoord2f(1, 0);
      glVertex3d(side * hstabSpan, hstabY - hstabThickness * 0.7, hstabZ + 0.03);
      glEnd();
      
      /* Closes the tip */
      glBegin(GL_QUADS);
      glNormal3d(side, 0, 0);
      glVertex3d(side * hstabSpan, hstabY + hstabThickness * 0.7, hstabZ + 0.03);
      glVertex3d(side * hstabSpan, hstabY - hstabThickness * 0.7, hstabZ + 0.03);
      glVertex3d(side * hstabSpan * 0.85, hstabY - hstabThickness * 0.7, hstabZ - hstabChordTip);
      glVertex3d(side * hstabSpan * 0.85, hstabY + hstabThickness * 0.7, hstabZ - hstabChordTip);
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
}

/*
 * Draws the fenestron (enclosed tail rotor) housing
 */
static void DrawFenestron(double tailRotorAngle)
{
   double tailEnd = -CABIN_LENGTH * 0.5 - TAIL_LENGTH;
   double tailY = 0.15 + 0.08 + TAIL_END_RADIUS + 0.15;
   double tailZ = tailEnd + 0.15;
   
      /* Configures the fenestron shroud */
   float shroudColor[] = {0.88, 0.70, 0.06, 1.0};
   float shroudSpec[] = {0.5, 0.45, 0.35, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shroudColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, shroudSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 55.0);
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, heliBackTexture);
   
   /* Builds the outer shroud ring */
   int segments = 32;
   double outerRadius = TAIL_ROTOR_RADIUS + 0.06;
   double innerRadius = TAIL_ROTOR_RADIUS + 0.02;
   double shroudDepth = 0.1;
   
   glPushMatrix();
   glTranslated(0, tailY, tailZ);
   glRotated(90, 0, 1, 0);
   
   /* Renders the outer surface */
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = (double)i / segments * 2.0 * M_PI;
      double x = outerRadius * cos(angle);
      double y = outerRadius * sin(angle);
      
      glNormal3d(cos(angle), sin(angle), 0);
      glTexCoord2f((float)i / segments, 0);
      glVertex3d(-shroudDepth * 0.5, y, x);
      glTexCoord2f((float)i / segments, 1);
      glVertex3d(shroudDepth * 0.5, y, x);
   }
   glEnd();
   
   /* Renders the inner surface */
   float innerColor[] = {0.08, 0.08, 0.1, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, innerColor);
   
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = (double)i / segments * 2.0 * M_PI;
      double x = innerRadius * cos(angle);
      double y = innerRadius * sin(angle);
      
      glNormal3d(-cos(angle), -sin(angle), 0);
      glVertex3d(-shroudDepth * 0.5, y, x);
      glVertex3d(shroudDepth * 0.5, y, x);
   }
   glEnd();
   
   /* Seals the front ring face */
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shroudColor);
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = (double)i / segments * 2.0 * M_PI;
      glNormal3d(-1, 0, 0);
      glVertex3d(-shroudDepth * 0.5, innerRadius * sin(angle), innerRadius * cos(angle));
      glVertex3d(-shroudDepth * 0.5, outerRadius * sin(angle), outerRadius * cos(angle));
   }
   glEnd();
   
   /* Seals the rear ring face */
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = (double)i / segments * 2.0 * M_PI;
      glNormal3d(1, 0, 0);
      glVertex3d(shroudDepth * 0.5, outerRadius * sin(angle), outerRadius * cos(angle));
      glVertex3d(shroudDepth * 0.5, innerRadius * sin(angle), innerRadius * cos(angle));
   }
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   /* Draws the tail rotor hub */
   float hubColor[] = {0.12, 0.12, 0.14, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hubColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);
   
   glPushMatrix();
   glRotated(tailRotorAngle, 1, 0, 0);
   
   glutSolidSphere(0.035, 12, 8);
   
   /* Draws the tail rotor blades */
   float bladeColor[] = {0.08, 0.08, 0.1, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bladeColor);
   
   int numBlades = 8;  /* Fenestron uses many blades */
   for (int i = 0; i < numBlades; i++)
   {
      glPushMatrix();
      glRotated(i * (360.0 / numBlades), 1, 0, 0);
      glTranslated(0, 0.04, 0);
      
      /* Draws each blade */
      glBegin(GL_QUADS);
      double bladeLen = TAIL_ROTOR_RADIUS - 0.03;
      glNormal3d(1, 0, 0);
      glVertex3d(0.008, 0, -0.012);
      glVertex3d(0.008, bladeLen, -0.008);
      glVertex3d(0.008, bladeLen, 0.008);
      glVertex3d(0.008, 0, 0.012);
      
      glNormal3d(-1, 0, 0);
      glVertex3d(-0.005, 0, -0.012);
      glVertex3d(-0.005, 0, 0.012);
      glVertex3d(-0.005, bladeLen, 0.008);
      glVertex3d(-0.005, bladeLen, -0.008);
      glEnd();
      
      glPopMatrix();
   }
   
   glPopMatrix();
   glPopMatrix();
}

/*
 * Draws the main rotor hub with the mast connection
 */
static void DrawMainRotorHub(void)
{
   float hubColor[] = {0.1, 0.1, 0.12, 1.0};
   float hubSpec[] = {0.5, 0.5, 0.55, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hubColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hubSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);
   
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   
   /* Extrudes the rotor mast */
   glPushMatrix();
   glRotated(-90, 1, 0, 0);
   gluCylinder(quad, 0.055, 0.045, 0.22, 16, 2);
   glPopMatrix();
   
   /* Smooths the mast base */
   glPushMatrix();
   glTranslated(0, 0.02, 0);
   glScaled(1.0, 0.35, 1.0);
   glutSolidSphere(0.1, 14, 10);
   glPopMatrix();
   
   /* Shapes the swashplate */
   glPushMatrix();
   glTranslated(0, 0.14, 0);
   glScaled(1.0, 0.22, 1.0);
   glutSolidSphere(0.09, 14, 8);
   glPopMatrix();
   
   /* Caps the hub */
   glPushMatrix();
   glTranslated(0, 0.22, 0);
   glScaled(1.0, 0.5, 1.0);
   glutSolidSphere(0.065, 12, 8);
   glPopMatrix();
   
   /* Adds the pitch control rods */
   float rodColor[] = {0.25, 0.25, 0.28, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rodColor);
   
   for (int i = 0; i < 4; i++)
   {
      glPushMatrix();
      double angle = i * 90.0 + 22.5;
      glRotated(angle, 0, 1, 0);
      glTranslated(0.07, 0.12, 0);
      glRotated(15, 0, 0, 1);
      glRotated(-90, 1, 0, 0);
      gluCylinder(quad, 0.008, 0.006, 0.12, 8, 1);
      glPopMatrix();
   }
   
   gluDeleteQuadric(quad);
}

/*
 * Draws a main rotor blade with an airfoil profile
 */
static void DrawMainRotorBlade(double length, double chordRoot, double chordTip)
{
   int segments = 16;
   double thickness = 0.022;
   
   for (int i = 0; i < segments; i++)
   {
      double t0 = (double)i / segments;
      double t1 = (double)(i + 1) / segments;
      
      double r0 = t0 * length;
      double r1 = t1 * length;
      
      double c0 = chordRoot * (1.0 - t0) + chordTip * t0;
      double c1 = chordRoot * (1.0 - t1) + chordTip * t1;
      double th0 = thickness * (1.0 - t0 * 0.4);
      double th1 = thickness * (1.0 - t1 * 0.4);
      
      /* Draws the top surface */
      glBegin(GL_QUADS);
      glNormal3d(0, 1, 0);
      glVertex3d(r0, th0, -c0 * 0.3);
      glVertex3d(r1, th1, -c1 * 0.3);
      glVertex3d(r1, th1 * 0.6, c1 * 0.7);
      glVertex3d(r0, th0 * 0.6, c0 * 0.7);
      glEnd();
      
      /* Draws the bottom surface */
      glBegin(GL_QUADS);
      glNormal3d(0, -1, 0);
      glVertex3d(r0, -th0 * 0.5, -c0 * 0.3);
      glVertex3d(r0, -th0 * 0.3, c0 * 0.7);
      glVertex3d(r1, -th1 * 0.3, c1 * 0.7);
      glVertex3d(r1, -th1 * 0.5, -c1 * 0.3);
      glEnd();
      
      /* Shapes the leading edge */
      glBegin(GL_QUADS);
      glNormal3d(0, 0.3, -1);
      glVertex3d(r0, th0, -c0 * 0.3);
      glVertex3d(r0, -th0 * 0.5, -c0 * 0.3);
      glVertex3d(r1, -th1 * 0.5, -c1 * 0.3);
      glVertex3d(r1, th1, -c1 * 0.3);
      glEnd();
      
      /* Shapes the trailing edge */
      glBegin(GL_QUADS);
      glNormal3d(0, 0.1, 1);
      glVertex3d(r0, th0 * 0.6, c0 * 0.7);
      glVertex3d(r1, th1 * 0.6, c1 * 0.7);
      glVertex3d(r1, -th1 * 0.3, c1 * 0.7);
      glVertex3d(r0, -th0 * 0.3, c0 * 0.7);
      glEnd();
   }
   
   /* Closes the blade tip */
   glBegin(GL_QUADS);
   double c = chordTip;
   double th = thickness * 0.6;
   glNormal3d(1, 0, 0);
   glVertex3d(length, th, -c * 0.3);
   glVertex3d(length, th * 0.6, c * 0.7);
   glVertex3d(length, -th * 0.3, c * 0.7);
   glVertex3d(length, -th * 0.5, -c * 0.3);
   glEnd();
}

/*
 * Draws the complete main rotor assembly
 */
static void DrawMainRotor(double rotation)
{
   glPushMatrix();
   
   /* Positions the hub at the top of the fuselage */
   double rotorY = CABIN_HEIGHT * 0.5 + 0.08;
   double rotorZ = -0.08;
   glTranslated(0, rotorY, rotorZ);
   
   /* Draws the hub that ties into the mast */
   DrawMainRotorHub();
   
   /* Rotates the blades */
   glTranslated(0, 0.24, 0);
   glRotated(rotation, 0, 1, 0);
   
   /* Sets blade material */
   float bladeColor[] = {0.07, 0.07, 0.09, 1.0};
   float bladeSpec[] = {0.25, 0.25, 0.3, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bladeColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bladeSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 35.0);
   
   /* Creates four blades */
   int numBlades = 4;
   for (int i = 0; i < numBlades; i++)
   {
      glPushMatrix();
      glRotated(i * (360.0 / numBlades), 0, 1, 0);
      glTranslated(0.08, 0, 0);
      glRotated(6, 0, 0, 1);  /* Blade pitch */
      DrawMainRotorBlade(MAIN_ROTOR_RADIUS, 0.14, 0.09);
      glPopMatrix();
   }
   
   glPopMatrix();
}

/*
 * Draws the landing skids with their attachment struts
 */
static void DrawLandingSkids(void)
{
   float skidColor[] = {0.08, 0.08, 0.1, 1.0};
   float skidSpec[] = {0.35, 0.35, 0.4, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skidColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, skidSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, metalPlateTexture);
   
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   gluQuadricTexture(quad, GL_TRUE);
   
   double skidY = -CABIN_HEIGHT * 0.5 - 0.22;
   double skidTubeRadius = 0.028;
   double strutRadius = 0.018;
   
   for (int side = -1; side <= 1; side += 2)
   {
      double skidX = side * SKID_WIDTH * 0.5;
      
      /* Draws the main horizontal skid tube */
      glPushMatrix();
      glTranslated(skidX, skidY, SKID_LENGTH * 0.5);
      glRotated(180, 1, 0, 0);
      glRotated(180, 0, 1, 0);
      gluCylinder(quad, skidTubeRadius, skidTubeRadius, SKID_LENGTH, 14, 4);
      glPopMatrix();
      
      /* Forms the front curved upturn */
      glPushMatrix();
      glTranslated(skidX, skidY + 0.08, SKID_LENGTH * 0.5);
      for (int i = 0; i < 8; i++)
      {
         double a0 = i * 11.25 * M_PI / 180.0;
         double a1 = (i + 1) * 11.25 * M_PI / 180.0;
         double r = 0.12;
         
         glBegin(GL_QUAD_STRIP);
         for (int j = 0; j <= 10; j++)
         {
            double angle = j * 36.0;
            double nx = cos(angle * M_PI / 180.0);
            double ny = sin(angle * M_PI / 180.0);
            
            glNormal3d(nx, ny * cos(a0), ny * sin(a0));
            glVertex3d(skidTubeRadius * nx, 
                      r * sin(a0) + skidTubeRadius * ny * cos(a0),
                      r * cos(a0) + skidTubeRadius * ny * sin(a0));
            glVertex3d(skidTubeRadius * nx,
                      r * sin(a1) + skidTubeRadius * ny * cos(a1),
                      r * cos(a1) + skidTubeRadius * ny * sin(a1));
         }
         glEnd();
      }
      glPopMatrix();
      
      /* Forms the rear curved upturn */
      glPushMatrix();
      glTranslated(skidX, skidY + 0.08, -SKID_LENGTH * 0.5);
      glRotated(180, 0, 1, 0);
      for (int i = 0; i < 8; i++)
      {
         double a0 = i * 11.25 * M_PI / 180.0;
         double a1 = (i + 1) * 11.25 * M_PI / 180.0;
         double r = 0.12;
         
         glBegin(GL_QUAD_STRIP);
         for (int j = 0; j <= 10; j++)
         {
            double angle = j * 36.0;
            double nx = cos(angle * M_PI / 180.0);
            double ny = sin(angle * M_PI / 180.0);
            
            glNormal3d(nx, ny * cos(a0), ny * sin(a0));
            glVertex3d(skidTubeRadius * nx,
                      r * sin(a0) + skidTubeRadius * ny * cos(a0),
                      r * cos(a0) + skidTubeRadius * ny * sin(a0));
            glVertex3d(skidTubeRadius * nx,
                      r * sin(a1) + skidTubeRadius * ny * cos(a1),
                      r * cos(a1) + skidTubeRadius * ny * sin(a1));
         }
         glEnd();
      }
      glPopMatrix();
      
      /* Adds the front cross tube */
      glPushMatrix();
      glTranslated(skidX, skidY + 0.02, CABIN_LENGTH * 0.28);
      glRotated(90, 1, 0, 0);
      gluCylinder(quad, strutRadius, strutRadius, 0.24, 10, 2);
      glPopMatrix();
      
      /* Adds the rear cross tube */
      glPushMatrix();
      glTranslated(skidX, skidY + 0.02, -CABIN_LENGTH * 0.25);
      glRotated(90, 1, 0, 0);
      gluCylinder(quad, strutRadius, strutRadius, 0.24, 10, 2);
      glPopMatrix();
      
      /* Adds the front diagonal struts */
      glPushMatrix();
      glTranslated(skidX * 0.92, -CABIN_HEIGHT * 0.35, CABIN_LENGTH * 0.22);
      
      double strutLen = 0.32;
      double strutAngleV = -68.0;
      double strutAngleH = side * 18.0;
      
      glRotated(strutAngleH, 0, 1, 0);
      glRotated(strutAngleV, 1, 0, 0);
      gluCylinder(quad, strutRadius, strutRadius * 0.9, strutLen, 10, 2);
      glPopMatrix();
      
      /* Adds the rear diagonal struts */
      glPushMatrix();
      glTranslated(skidX * 0.92, -CABIN_HEIGHT * 0.35, -CABIN_LENGTH * 0.18);
      glRotated(strutAngleH, 0, 1, 0);
      glRotated(strutAngleV, 1, 0, 0);
      gluCylinder(quad, strutRadius, strutRadius * 0.9, strutLen, 10, 2);
      glPopMatrix();
   }
   
   gluDeleteQuadric(quad);
   glDisable(GL_TEXTURE_2D);
}

/*
 * Draws the navigation and anti-collision lights
 */
static void DrawLights(double time)
{
   float noSpec[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, noSpec);
   
   /* Anti-collision beacon pulses red */
   double beaconPulse = 0.5 + 0.5 * sin(time * 7.0);
   float beaconColor[] = {(float)beaconPulse, 0.02f, 0.02f, 1.0f};
   float beaconEmit[] = {(float)(beaconPulse * 0.85), 0.01f, 0.01f, 1.0f};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, beaconColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, beaconEmit);
   
   /* Draws the top beacon */
   glPushMatrix();
   glTranslated(0, CABIN_HEIGHT * 0.5 + 0.35, -0.08);
   glutSolidSphere(0.032, 12, 8);
   glPopMatrix();
   
   /* Draws the belly beacon */
   glPushMatrix();
   glTranslated(0, -CABIN_HEIGHT * 0.48, 0.1);
   glutSolidSphere(0.028, 10, 6);
   glPopMatrix();
   
   /* Draws the tail beacon */
   double tailEnd = -CABIN_LENGTH * 0.5 - TAIL_LENGTH;
   double tailY = 0.15 + 0.08 + TAIL_END_RADIUS + 0.55;
   glPushMatrix();
   glTranslated(0, tailY, tailEnd + 0.1);
   glutSolidSphere(0.022, 10, 6);
   glPopMatrix();
   
   /* Port navigation light glows red */
   float redNav[] = {0.92f, 0.08f, 0.08f, 1.0f};
   float redEmit[] = {0.75f, 0.04f, 0.04f, 1.0f};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, redNav);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, redEmit);
   
   glPushMatrix();
   glTranslated(-CABIN_WIDTH * 0.52, 0.02, CABIN_LENGTH * 0.25);
   glutSolidSphere(0.022, 10, 6);
   glPopMatrix();
   
   /* Starboard navigation light glows green */
   float greenNav[] = {0.08f, 0.92f, 0.08f, 1.0f};
   float greenEmit[] = {0.04f, 0.75f, 0.04f, 1.0f};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, greenNav);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, greenEmit);
   
   glPushMatrix();
   glTranslated(CABIN_WIDTH * 0.52, 0.02, CABIN_LENGTH * 0.25);
   glutSolidSphere(0.022, 10, 6);
   glPopMatrix();
   
   /* Positions the landing and search light */
   float whiteLight[] = {1.0f, 0.98f, 0.92f, 1.0f};
   float whiteEmit[] = {0.65f, 0.62f, 0.55f, 1.0f};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, whiteLight);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, whiteEmit);
   
   glPushMatrix();
   glTranslated(0, -CABIN_HEIGHT * 0.42, CABIN_LENGTH * 0.4);
   glScaled(1.0, 0.55, 1.0);
   glutSolidSphere(0.038, 12, 8);
   glPopMatrix();
   
   /* Resets the emission */
   float noEmit[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmit);
}

/*
 * Draws panel lines and small exterior details
 */
static void DrawPanelDetails(void)
{
   glDisable(GL_LIGHTING);
   glColor3f(0.18, 0.15, 0.05);
   glLineWidth(1.2);
   
   /* Traces the cabin door outlines */
   for (int side = -1; side <= 1; side += 2)
   {
      glBegin(GL_LINE_LOOP);
      glVertex3d(side * CABIN_WIDTH * 0.505, -0.3, CABIN_LENGTH * 0.22);
      glVertex3d(side * CABIN_WIDTH * 0.505, -0.3, -CABIN_LENGTH * 0.18);
      glVertex3d(side * CABIN_WIDTH * 0.49, 0.22, -CABIN_LENGTH * 0.15);
      glVertex3d(side * CABIN_WIDTH * 0.49, 0.22, CABIN_LENGTH * 0.18);
      glEnd();
   }
   
   /* Traces the engine cowl access panels */
   glBegin(GL_LINE_LOOP);
   glVertex3d(-0.28, CABIN_HEIGHT * 0.44, -0.05);
   glVertex3d(-0.28, CABIN_HEIGHT * 0.44, -0.55);
   glVertex3d(-0.18, CABIN_HEIGHT * 0.52, -0.55);
   glVertex3d(-0.18, CABIN_HEIGHT * 0.52, -0.05);
   glEnd();
   
   glBegin(GL_LINE_LOOP);
   glVertex3d(0.28, CABIN_HEIGHT * 0.44, -0.05);
   glVertex3d(0.18, CABIN_HEIGHT * 0.52, -0.05);
   glVertex3d(0.18, CABIN_HEIGHT * 0.52, -0.55);
   glVertex3d(0.28, CABIN_HEIGHT * 0.44, -0.55);
   glEnd();
   
   glLineWidth(1.0);
   glEnable(GL_LIGHTING);
   
   /* Adds the door handles */
   float handleColor[] = {0.12, 0.12, 0.14, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, handleColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 65.0);
   
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * CABIN_WIDTH * 0.51, -0.02, CABIN_LENGTH * 0.02);
      glScaled(0.015, 0.035, 0.07);
      glutSolidCube(1.0);
      glPopMatrix();
   }
}

/*
 * Draws antennas and sensors
 */
static void DrawAntennas(void)
{
   float antennaColor[] = {0.08, 0.08, 0.1, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, antennaColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 45.0);
   
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   
   /* Mounts the VHF antenna */
   glPushMatrix();
   glTranslated(0, CABIN_HEIGHT * 0.5 + 0.06, CABIN_LENGTH * 0.12);
   glRotated(-90, 1, 0, 0);
   gluCylinder(quad, 0.007, 0.004, 0.16, 8, 2);
   glPopMatrix();
   
   /* Adds the GPS dome */
   float domeColor[] = {0.85, 0.85, 0.88, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, domeColor);
   
   glPushMatrix();
   glTranslated(0, CABIN_HEIGHT * 0.52, CABIN_LENGTH * 0.28);
   glScaled(1.0, 0.45, 1.0);
   glutSolidSphere(0.038, 12, 8);
   glPopMatrix();
   
   /* Mounts the pitot tube */
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, antennaColor);
   
   glPushMatrix();
   glTranslated(-CABIN_WIDTH * 0.35, 0.08, CABIN_LENGTH * 0.52);
   glRotated(-8, 0, 1, 0);
   glRotated(4, 1, 0, 0);
   gluCylinder(quad, 0.007, 0.005, 0.14, 8, 2);
   glPopMatrix();
   
   /* Adds the tail antennas */
   double tailMid = -CABIN_LENGTH * 0.5 - TAIL_LENGTH * 0.5;
   glPushMatrix();
   glTranslated(0, -0.02, tailMid);
   glRotated(90, 1, 0, 0);
   gluCylinder(quad, 0.005, 0.003, 0.1, 6, 2);
   glPopMatrix();
   
   gluDeleteQuadric(quad);
}

/*
 * Adds the decorative black stripe
 */
static void DrawStripe(void)
{
   float stripeColor[] = {0.04, 0.04, 0.06, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stripeColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 35.0);
   
   /* Top stripe along the engine cowling */
   glBegin(GL_QUADS);
   glNormal3d(0, 1, 0);
   glVertex3d(-0.06, CABIN_HEIGHT * 0.5 + 0.28, 0.05);
   glVertex3d(0.06, CABIN_HEIGHT * 0.5 + 0.28, 0.05);
   glVertex3d(0.06, CABIN_HEIGHT * 0.5 + 0.27, -0.52);
   glVertex3d(-0.06, CABIN_HEIGHT * 0.5 + 0.27, -0.52);
   glEnd();
}

/*
 * Renders the full helicopter
 */
void Helicopter(double x, double y, double z, double scale,
                double facing, double time, double rotorSpeed, int engineOn)
{
   if (!texturesLoaded) InitHelicopterTextures();
   
   glPushMatrix();
   
   /* Positions and orients the model */
   glTranslated(x, y, z);
   glRotated(facing, 0, 1, 0);
   glScaled(scale, scale, scale);
   
   /* Calculates rotor angles */
   double mainRotorAngle = 0;
   double tailRotorAngle = 0;
   
   if (engineOn)
   {
      mainRotorAngle = time * 680.0 * rotorSpeed;
      tailRotorAngle = time * 2200.0 * rotorSpeed;
   }
   
   /* Draws all components in sequence */
   DrawFuselageBody();
   DrawTailBoom();
   DrawEngineCowling();
   DrawCockpit();
   DrawStabilizers();
   DrawFenestron(tailRotorAngle);
   DrawMainRotor(mainRotorAngle);
   DrawLandingSkids();
   DrawLights(time);
   DrawPanelDetails();
   DrawAntennas();
   DrawStripe();
   
   glPopMatrix();
}