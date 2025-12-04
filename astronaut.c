/*
 * astronaut.c - draws the astronaut model
 */

#include "CSCIx229.h"
#include <math.h>

// textures load from main program
extern int ntex;
extern unsigned int texture[];

// texture indices for astronaut parts
static unsigned int texSpacesuit = 0;
static unsigned int texMetalVisor = 0;
static unsigned int texMetalPlate = 0;
static unsigned int texBoots = 0;
static unsigned int texGloves = 0;

/*
 * Initialize astronaut textures
 */
// void InitAstronautTextures(void)
// {
//    // Textures are loaded in main program
//    // We use the external texture array indices:
//    // For now, we'll reference them directly by the texture array
// }

/*
 * Helper: Draw smooth sphere with proper normals
 */
// draws a smooth sphere with normals
static void DrawSphere(double radius, int segments)
{
   int i, j;
   for (i = 0; i < segments; i++)
   {
      double lat0 = M_PI * (-0.5 + (double)i / segments);
      double lat1 = M_PI * (-0.5 + (double)(i + 1) / segments);
      double z0 = sin(lat0);
      double zr0 = cos(lat0);
      double z1 = sin(lat1);
      double zr1 = cos(lat1);

      glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= segments; j++)
      {
         double lng = 2 * M_PI * (double)j / segments;
         double x = cos(lng);
         double y = sin(lng);

         glTexCoord2f((double)j / segments, (double)i / segments);
         glNormal3d(x * zr0, z0, y * zr0);
         glVertex3d(radius * x * zr0, radius * z0, radius * y * zr0);

         glTexCoord2f((double)j / segments, (double)(i + 1) / segments);
         glNormal3d(x * zr1, z1, y * zr1);
         glVertex3d(radius * x * zr1, radius * z1, radius * y * zr1);
      }
      glEnd();
   }
}

/*
 * Helper: Draw cylinder along Y-axis
 */
// draws a cylinder along the Y-axis
static void DrawCylinder(double radius, double height, int segments)
{
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = 360.0 * i / segments;
      double ca = Cos(angle);
      double sa = Sin(angle);

      glNormal3d(ca, 0, sa);
      glTexCoord2f((double)i / segments, 0.0);
      glVertex3d(radius * ca, 0, radius * sa);

      glTexCoord2f((double)i / segments, 1.0);
      glVertex3d(radius * ca, height, radius * sa);
   }
   glEnd();
}

/*
 * Helper: Draw box with proper normals
 */
// draws a box with proper normals
static void DrawBox(double w, double h, double d)
{
   // front face (+Z)
   glNormal3d(0, 0, 1);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3d(-w, -h, +d);
   glTexCoord2f(1, 0); glVertex3d(+w, -h, +d);
   glTexCoord2f(1, 1); glVertex3d(+w, +h, +d);
   glTexCoord2f(0, 1); glVertex3d(-w, +h, +d);
   glEnd();

   // back face (-Z)
   glNormal3d(0, 0, -1);
   glBegin(GL_QUADS);
   glTexCoord2f(1, 0); glVertex3d(+w, -h, -d);
   glTexCoord2f(0, 0); glVertex3d(-w, -h, -d);
   glTexCoord2f(0, 1); glVertex3d(-w, +h, -d);
   glTexCoord2f(1, 1); glVertex3d(+w, +h, -d);
   glEnd();

   // right face (+X)
   glNormal3d(1, 0, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3d(+w, -h, +d);
   glTexCoord2f(1, 0); glVertex3d(+w, -h, -d);
   glTexCoord2f(1, 1); glVertex3d(+w, +h, -d);
   glTexCoord2f(0, 1); glVertex3d(+w, +h, +d);
   glEnd();

   // left face (-X)
   glNormal3d(-1, 0, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(1, 0); glVertex3d(-w, -h, -d);
   glTexCoord2f(0, 0); glVertex3d(-w, -h, +d);
   glTexCoord2f(0, 1); glVertex3d(-w, +h, +d);
   glTexCoord2f(1, 1); glVertex3d(-w, +h, -d);
   glEnd();

   // top face (+Y)
   glNormal3d(0, 1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3d(-w, +h, +d);
   glTexCoord2f(1, 0); glVertex3d(+w, +h, +d);
   glTexCoord2f(1, 1); glVertex3d(+w, +h, -d);
   glTexCoord2f(0, 1); glVertex3d(-w, +h, -d);
   glEnd();

   // bottom face (-Y)
   glNormal3d(0, -1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 1); glVertex3d(-w, -h, -d);
   glTexCoord2f(1, 1); glVertex3d(+w, -h, -d);
   glTexCoord2f(1, 0); glVertex3d(+w, -h, +d);
   glTexCoord2f(0, 0); glVertex3d(-w, -h, +d);
   glEnd();
}

/*
 * Draw astronaut helmet with gold visor
 */
// draws the astronaut helmet with gold visor
static void DrawHelmet(double time)
{
   float white[] = {0.95, 0.95, 0.95, 1.0};
   float gold[] = {1.0, 0.84, 0.0, 1.0};
   float specular[] = {0.8, 0.8, 0.8, 1.0};
   float goldSpecular[] = {1.0, 0.95, 0.6, 1.0};
   float black[] = {0.0, 0.0, 0.0, 1.0};

   glPushMatrix();
   glTranslated(0, 1.65, 0);

   // main helmet shell in white
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[11]); // spacesuit
   }

   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0);

   // back of helmet is larger
   glPushMatrix();
   glScaled(0.95, 1.0, 1.0);
   DrawSphere(0.18, 32);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   // gold reflective visor in front
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[7]); // blue metal for shine
   }

   glColor3f(1.0, 0.9, 0.3);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gold);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, goldSpecular);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);

   // visor is a partial sphere in front
   glPushMatrix();
   glTranslated(0, 0, 0.08);
   glScaled(0.95, 0.95, 0.6);

   // draw front hemisphere of visor
   for (int i = 0; i < 16; i++)
   {
      double lat0 = M_PI * (-0.3 + (double)i / 32.0);
      double lat1 = M_PI * (-0.3 + (double)(i + 1) / 32.0);
      double z0 = sin(lat0);
      double zr0 = cos(lat0);
      double z1 = sin(lat1);
      double zr1 = cos(lat1);

      glBegin(GL_QUAD_STRIP);
      for (int j = -12; j <= 12; j++)
      {
         double lng = M_PI * (double)j / 24.0;
         double x = cos(lng);
         double y = sin(lng);

         glTexCoord2f((double)(j + 12) / 24.0, (double)i / 16.0);
         glNormal3d(x * zr0, z0, y * zr0);
         glVertex3d(0.18 * x * zr0, 0.18 * z0, 0.18 * y * zr0);

         glTexCoord2f((double)(j + 12) / 24.0, (double)(i + 1) / 16.0);
         glNormal3d(x * zr1, z1, y * zr1);
         glVertex3d(0.18 * x * zr1, 0.18 * z1, 0.18 * y * zr1);
      }
      glEnd();
   }
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   // neck ring is metal
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal plate
   }

   float metalGrey[] = {0.7, 0.7, 0.75, 1.0};
   glColor3f(0.7, 0.7, 0.75);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, metalGrey);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);

   glPushMatrix();
   glTranslated(0, -0.2, 0);
   glRotated(-90, 1, 0, 0);
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   gluQuadricTexture(quad, GL_TRUE);
   gluCylinder(quad, 0.13, 0.14, 0.06, 24, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

/*
 * Draws backpack
 */
// draws the BP backpack
static void DrawBP(void)
{
   float white[] = {0.95, 0.95, 0.95, 1.0};
   float metalGrey[] = {0.6, 0.6, 0.65, 1.0};
   float specular[] = {0.5, 0.5, 0.5, 1.0};
   float black[] = {0.0, 0.0, 0.0, 1.0};

   glPushMatrix();
   glTranslated(0, 1.1, -0.25);

   // main BP box
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[11]); // spacesuit
   }

   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);

   DrawBox(0.25, 0.35, 0.12);

   glDisable(GL_TEXTURE_2D);

   // oxygen tanks in cylinder shape
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal_plate
   }

   glColor3f(0.6, 0.6, 0.65);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, metalGrey);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);

   for (int i = -1; i <= 1; i += 2)
   {
      glPushMatrix();
      glTranslated(i * 0.12, 0, -0.08);
      glRotated(90, 1, 0, 0);
      GLUquadric* quad = gluNewQuadric();
      gluQuadricNormals(quad, GLU_SMOOTH);
      gluQuadricTexture(quad, GL_TRUE);
      gluCylinder(quad, 0.055, 0.055, 0.6, 16, 1);
      gluDeleteQuadric(quad);
      glPopMatrix();
   }

   // top manifold connection
   glPushMatrix();
   glTranslated(0, 0.35, -0.05);
   glScaled(0.2, 0.05, 0.08);
   DrawBox(1, 1, 1);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

/*
 * Draw torso with chest control panel
 */
// draws the torso with chest control panel
static void DrawTorso(double time)
{
   float white[] = {0.95, 0.95, 0.95, 1.0};
   float specular[] = {0.4, 0.4, 0.4, 1.0};
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float darkGrey[] = {0.3, 0.3, 0.35, 1.0};

   // breathing animation
   double breathScale = 1.0 + 0.02 * Sin(time * 40.0);

   glPushMatrix();

   // upper torso
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[11]); // spacesuit
   }

   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0);

   glPushMatrix();
   glTranslated(0, 1.2, 0);
   glScaled(0.35 * breathScale, 0.42, 0.24 * breathScale);
   DrawSphere(1.0, 32);
   glPopMatrix();

   // lower torso
   glPushMatrix();
   glTranslated(0, 0.72, 0);
   glScaled(0.32, 0.25, 0.22);
   DrawSphere(1.0, 32);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   // chest control panel
   glColor3f(0.3, 0.3, 0.35);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, darkGrey);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 5.0);

   glPushMatrix();
   glTranslated(0, 1.15, 0.22);
   glScaled(0.12, 0.18, 0.02);
   DrawBox(1, 1, 1);
   glPopMatrix();

   // control panel details with buttons
   float red[] = {0.8, 0.1, 0.1, 1.0};
   float yellow[] = {0.9, 0.9, 0.1, 1.0};

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
   glPushMatrix();
   glTranslated(-0.05, 1.2, 0.245);
   DrawSphere(0.015, 12);
   glPopMatrix();

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, yellow);
   glPushMatrix();
   glTranslated(0.05, 1.2, 0.245);
   DrawSphere(0.015, 12);
   glPopMatrix();

   // waist bearing ring
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal_plate
   }

   float metalGrey[] = {0.65, 0.65, 0.7, 1.0};
   glColor3f(0.65, 0.65, 0.7);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, metalGrey);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);

   glPushMatrix();
   glTranslated(0, 0.95, 0);
   glRotated(-90, 1, 0, 0);
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   gluQuadricTexture(quad, GL_TRUE);
   gluCylinder(quad, 0.33, 0.33, 0.08, 32, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

// draws arm with joints and glove
static void DrawArm(int side, double shoulderAngle, double elbowAngle, double time)
{
   float white[] = {0.95, 0.95, 0.95, 1.0};
   float grey[] = {0.5, 0.5, 0.55, 1.0};
   float specular[] = {0.4, 0.4, 0.4, 1.0};
   float black[] = {0.0, 0.0, 0.0, 1.0};

   glPushMatrix();

   // position at shoulder
   glTranslated(side * 0.38, 1.35, 0);
   glRotated(side * 10, 0, 0, 1);
   glRotated(shoulderAngle, 1, 0, 0);

   // shoulder bearing
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal_plate
   }

   glColor3f(0.65, 0.65, 0.7);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);

   DrawSphere(0.09, 20);

   glDisable(GL_TEXTURE_2D);

   // upper arm
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[11]); // spacesuit
   }

   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0);

   glPushMatrix();
   glTranslated(0, -0.18, 0);
   glRotated(-90, 1, 0, 0);
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   gluQuadricTexture(quad, GL_TRUE);
   gluCylinder(quad, 0.07, 0.065, 0.36, 20, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   // elbow joint
   glTranslated(0, -0.36, 0);

   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal_plate
   }

   glColor3f(0.6, 0.6, 0.65);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);

   DrawSphere(0.075, 20);

   glDisable(GL_TEXTURE_2D);

   glRotated(elbowAngle, 1, 0, 0);

   // forearm
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[11]); // spacesuit
   }

   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0);

   glPushMatrix();
   glTranslated(0, -0.16, 0);
   glRotated(-90, 1, 0, 0);
   quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   gluQuadricTexture(quad, GL_TRUE);
   gluCylinder(quad, 0.065, 0.06, 0.32, 20, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   // wrist bearing
   glTranslated(0, -0.32, 0);

   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal_plate
   }

   glColor3f(0.6, 0.6, 0.65);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);

   DrawSphere(0.065, 16);

   glDisable(GL_TEXTURE_2D);

   // glove
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[10]); // glove texture
   }

   float gloveColor[] = {0.85, 0.85, 0.88, 1.0};
   glColor3f(0.85, 0.85, 0.88);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gloveColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);

   // palm
   glPushMatrix();
   glTranslated(0, -0.08, 0);
   glScaled(0.08, 0.1, 0.06);
   DrawBox(1, 1, 1);
   glPopMatrix();

   // fingers and thumb
   for (int f = 0; f < 4; f++)
   {
      glPushMatrix();
      glTranslated((f - 1.5) * 0.025, -0.18, 0);
      glScaled(0.018, 0.06, 0.015);
      DrawBox(1, 1, 1);
      glPopMatrix();
   }

   // thumb
   glPushMatrix();
   glTranslated(side * 0.07, -0.12, 0.03);
   glRotated(side * 60, 0, 1, 0);
   glScaled(0.018, 0.05, 0.015);
   DrawBox(1, 1, 1);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

// draws leg with joints and moon boot
static void DrawLeg(int side, double hipAngle, double kneeAngle, double time)
{
   float white[] = {0.95, 0.95, 0.95, 1.0};
   float grey[] = {0.6, 0.6, 0.65, 1.0};
   float bootColor[] = {0.9, 0.9, 0.92, 1.0};
   float specular[] = {0.4, 0.4, 0.4, 1.0};
   float black[] = {0.0, 0.0, 0.0, 1.0};

   glPushMatrix();

   // position at hip
   glTranslated(side * 0.15, 0.65, 0);
   glRotated(hipAngle, 1, 0, 0);

   // hip bearing
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal_plate
   }

   glColor3f(0.65, 0.65, 0.7);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);

   DrawSphere(0.1, 20);

   glDisable(GL_TEXTURE_2D);

   // thigh
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[11]); // spacesuit
   }

   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0);

   glPushMatrix();
   glTranslated(0, -0.22, 0);
   glRotated(-90, 1, 0, 0);
   GLUquadric* quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   gluQuadricTexture(quad, GL_TRUE);
   gluCylinder(quad, 0.095, 0.09, 0.44, 24, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   // knee joint
   glTranslated(0, -0.44, 0);

   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal_plate
   }

   glColor3f(0.6, 0.6, 0.65);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);

   DrawSphere(0.095, 20);

   glDisable(GL_TEXTURE_2D);

   glRotated(kneeAngle, 1, 0, 0);

   // shin
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[11]); // spacesuit
   }

   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);

   glPushMatrix();
   glTranslated(0, -0.24, 0);
   glRotated(-90, 1, 0, 0);
   quad = gluNewQuadric();
   gluQuadricNormals(quad, GLU_SMOOTH);
   gluQuadricTexture(quad, GL_TRUE);
   gluCylinder(quad, 0.09, 0.085, 0.48, 24, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

   // ankle
   glTranslated(0, -0.48, 0);

   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[9]); // metal_plate
   }

   glColor3f(0.6, 0.6, 0.65);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);

   DrawSphere(0.09, 16);

   glDisable(GL_TEXTURE_2D);

   // moon boot
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture[12]); // shoe texture
   }

   glColor3f(0.9, 0.9, 0.92);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bootColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0);

   glPushMatrix();
   glTranslated(0, -0.1, 0.05);
   glScaled(0.12, 0.12, 0.18);
   DrawBox(1, 1, 1);
   glPopMatrix();

   // boot sole is thick
   float soleColor[] = {0.2, 0.2, 0.22, 1.0};
   glColor3f(0.2, 0.2, 0.22);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, soleColor);

   glPushMatrix();
   glTranslated(0, -0.23, 0.05);
   glScaled(0.14, 0.04, 0.2);
   DrawBox(1, 1, 1);
   glPopMatrix();

   // sole treads
   for (int t = 0; t < 4; t++)
   {
      glPushMatrix();
      glTranslated(0, -0.255, -0.1 + t * 0.08);
      glScaled(0.12, 0.01, 0.015);
      DrawBox(1, 1, 1);
      glPopMatrix();
   }

   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

// main astronaut rendering function
void Astronaut(double x, double y, double z, double scale, double rotation, double time)
{
   glPushMatrix();

   // position and orient
   glTranslated(x, y, z);
   glRotated(rotation, 0, 1, 0);
   glScaled(scale, scale, scale);

   // gentle idle bob animation
   double bobOffset = 0.015 * Sin(time * 30.0);
   glTranslated(0, bobOffset, 0);

   // slight sway
   glRotated(1.5 * Sin(time * 25.0), 0, 0, 1);

   // draw all body parts
   DrawTorso(time);
   DrawBP();
   DrawHelmet(time);

   // arms with gentle swing
   double armSwing = 5.0 * Sin(time * 20.0);
   double leftElbow = -15.0;
   double rightElbow = -15.0;

   DrawArm(-1, armSwing, leftElbow, time);
   DrawArm(1, -armSwing, rightElbow, time);

   // legs in standing position
   double legSwing = 2.0 * Sin(time * 20.0);
   double leftKnee = 5.0;
   double rightKnee = 5.0;

   DrawLeg(-1, legSwing, leftKnee, time);
   DrawLeg(1, -legSwing, rightKnee, time);

   glPopMatrix();
}
