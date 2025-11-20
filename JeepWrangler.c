/*
 * Jeep Wrangler
 * Rendering off-road vehicle in classic style
 */
#include "CSCIx229.h"

/*
 * Drawing 7-slot vertical grille
 */
static void DrawJeepGrille(void)
{
   float grilleColor[] = {0.08, 0.08, 0.08, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grilleColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0);
   
   // Drawing grille frame
   glBegin(GL_QUADS);
   glNormal3d(0, 0.1, 1);
   glVertex3d(-0.6, 0.4, 0);
   glVertex3d(0.6, 0.4, 0);
   glVertex3d(0.6, 1.1, 0);
   glVertex3d(-0.6, 1.1, 0);
   glEnd();
   
   // Drawing vertical slots
   float slotColor[] = {0.03, 0.03, 0.03, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, slotColor);
   
   for (int i = 0; i < 7; i++)
   {
      double x = -0.52 + i * 0.173;
      glBegin(GL_QUADS);
      glNormal3d(0, 0.1, 1);
      glVertex3d(x, 0.45, 0.01);
      glVertex3d(x + 0.12, 0.45, 0.01);
      glVertex3d(x + 0.12, 1.05, 0.01);
      glVertex3d(x, 1.05, 0.01);
      glEnd();
   }
}

/*
 * Drawing round headlight with chrome bezel
 */
static void DrawHeadlight(void)
{
   // Drawing chrome bezel ring
   float chromeColor[] = {0.7, 0.7, 0.75, 1.0};
   float chromeSpec[] = {0.9, 0.9, 0.95, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, chromeColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, chromeSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
   
   GLUquadric* quad = gluNewQuadric();
   gluCylinder(quad, 0.18, 0.18, 0.05, 20, 1);
   gluDisk(quad, 0.16, 0.18, 20, 1);
   
   // Drawing glass lens
   glPushMatrix();
   glTranslated(0, 0, 0.05);
   float glassColor[] = {0.9, 0.92, 0.95, 1.0};
   float glassSpec[] = {1.0, 1.0, 1.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glassColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
   
   gluDisk(quad, 0, 0.16, 20, 1);
   glPopMatrix();
   
   gluDeleteQuadric(quad);
}

/*
 * Drawing front hood with power dome
 */
static void DrawHood(void)
{
   float bodyColor[] = {0.15, 0.18, 0.15, 1.0};  // Dark olive green
   float bodySpec[] = {0.4, 0.45, 0.4, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bodyColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bodySpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);
   
   // Drawing main hood surface
   glBegin(GL_QUADS);
   glNormal3d(0, 0.8, 0.6);
   glVertex3d(-0.7, 0.35, 1.3);
   glVertex3d(0.7, 0.35, 1.3);
   glVertex3d(0.7, 1.15, 2.0);
   glVertex3d(-0.7, 1.15, 2.0);
   glEnd();
   
   // Drawing power dome bulge
   glPushMatrix();
   glTranslated(0, 0.9, 1.65);
   glScaled(0.4, 0.15, 0.3);
   glutSolidSphere(1.0, 16, 16);
   glPopMatrix();
   
   // Drawing hood vents
   glDisable(GL_LIGHTING);
   glColor3f(0.05, 0.05, 0.05);
   glLineWidth(2.0);
   
   for (int side = -1; side <= 1; side += 2)
   {
      for (int i = 0; i < 4; i++)
      {
         double xOffset = side * 0.35;
         double zPos = 1.5 + i * 0.1;
         glBegin(GL_LINES);
         glVertex3d(xOffset - 0.15, 0.75 + i * 0.05, zPos);
         glVertex3d(xOffset + 0.15, 0.75 + i * 0.05, zPos);
         glEnd();
      }
   }
   glEnable(GL_LIGHTING);
}

/*
 * Drawing windshield frame
 */
static void DrawWindshield(void)
{
   // Drawing frame
   float frameColor[] = {0.12, 0.12, 0.12, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, frameColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 25.0);
   
   glPushMatrix();
   glTranslated(0, 1.15, 1.95);
   glRotated(-70, 1, 0, 0);
   
   // Drawing frame outline
   glBegin(GL_QUADS);
   glNormal3d(0, 1, 0);
   // Left frame
   glVertex3d(-0.75, 0, 0);
   glVertex3d(-0.68, 0, 0);
   glVertex3d(-0.68, 0.7, 0);
   glVertex3d(-0.75, 0.7, 0);
   // Right frame
   glVertex3d(0.68, 0, 0);
   glVertex3d(0.75, 0, 0);
   glVertex3d(0.75, 0.7, 0);
   glVertex3d(0.68, 0.7, 0);
   // Top frame
   glVertex3d(-0.75, 0.7, 0);
   glVertex3d(0.75, 0.7, 0);
   glVertex3d(0.75, 0.78, 0);
   glVertex3d(-0.75, 0.78, 0);
   // Center divider
   glVertex3d(-0.03, 0, 0);
   glVertex3d(0.03, 0, 0);
   glVertex3d(0.03, 0.7, 0);
   glVertex3d(-0.03, 0.7, 0);
   glEnd();
   
   // Drawing glass panels
   float glassColor[] = {0.3, 0.35, 0.4, 0.7};
   float glassSpec[] = {0.9, 0.95, 1.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glassColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120.0);
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   // Drawing left pane
   glBegin(GL_QUADS);
   glNormal3d(0, 1, 0);
   glVertex3d(-0.68, 0.05, 0.01);
   glVertex3d(-0.03, 0.05, 0.01);
   glVertex3d(-0.03, 0.68, 0.01);
   glVertex3d(-0.68, 0.68, 0.01);
   // Drawing right pane
   glVertex3d(0.03, 0.05, 0.01);
   glVertex3d(0.68, 0.05, 0.01);
   glVertex3d(0.68, 0.68, 0.01);
   glVertex3d(0.03, 0.68, 0.01);
   glEnd();
   
   glDisable(GL_BLEND);
   glPopMatrix();
}

/*
 * Drawing main body tub
 */
static void DrawBodyTub(void)
{
   float bodyColor[] = {0.15, 0.18, 0.15, 1.0};
   float bodySpec[] = {0.4, 0.45, 0.4, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bodyColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bodySpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);
   
   // Drawing floor panel
   glBegin(GL_QUADS);
   glNormal3d(0, -1, 0);
   glVertex3d(-0.85, 0.35, -1.2);
   glVertex3d(0.85, 0.35, -1.2);
   glVertex3d(0.85, 0.35, 2.0);
   glVertex3d(-0.85, 0.35, 2.0);
   glEnd();
   
   // Drawing left side
   glBegin(GL_QUADS);
   glNormal3d(-1, 0.2, 0);
   glVertex3d(-0.85, 0.35, -1.2);
   glVertex3d(-0.85, 0.35, 2.0);
   glVertex3d(-0.85, 1.4, 1.5);
   glVertex3d(-0.85, 1.2, -0.8);
   glEnd();
   
   // Drawing right side
   glBegin(GL_QUADS);
   glNormal3d(1, 0.2, 0);
   glVertex3d(0.85, 0.35, -1.2);
   glVertex3d(0.85, 1.2, -0.8);
   glVertex3d(0.85, 1.4, 1.5);
   glVertex3d(0.85, 0.35, 2.0);
   glEnd();
   
   // Drawing rear panel
   glBegin(GL_QUADS);
   glNormal3d(0, 0.3, -1);
   glVertex3d(-0.85, 0.35, -1.2);
   glVertex3d(-0.85, 1.2, -0.8);
   glVertex3d(0.85, 1.2, -0.8);
   glVertex3d(0.85, 0.35, -1.2);
   glEnd();
   
   // Drawing door cutout lines
   glDisable(GL_LIGHTING);
   glColor3f(0.08, 0.08, 0.08);
   glLineWidth(3.0);
   
   for (int side = -1; side <= 1; side += 2)
   {
      glBegin(GL_LINE_STRIP);
      glVertex3d(side * 0.85, 0.5, 1.4);
      glVertex3d(side * 0.85, 0.5, 0.2);
      glVertex3d(side * 0.85, 1.3, 0.2);
      glVertex3d(side * 0.85, 1.3, 1.4);
      glEnd();
   }
   glEnable(GL_LIGHTING);
}

/*
 * Drawing roll cage / sport bar
 */
static void DrawRollCage(void)
{
   float barColor[] = {0.08, 0.08, 0.08, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, barColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0);
   
   GLUquadric* quad = gluNewQuadric();
   
   // Drawing vertical bars
   double barPositions[4][2] = {
      {-0.7, 1.3}, {0.7, 1.3},
      {-0.7, -0.5}, {0.7, -0.5}
   };
   
   for (int i = 0; i < 4; i++)
   {
      glPushMatrix();
      glTranslated(barPositions[i][0], 1.35, barPositions[i][1]);
      glRotated(-90, 1, 0, 0);
      gluCylinder(quad, 0.04, 0.04, 0.6, 12, 1);
      glPopMatrix();
   }
   
   // Drawing top horizontal bars
   glPushMatrix();
   glTranslated(-0.7, 1.95, 1.3);
   glRotated(90, 0, 1, 0);
   gluCylinder(quad, 0.04, 0.04, 1.4, 12, 1);
   glPopMatrix();
   
   glPushMatrix();
   glTranslated(-0.7, 1.95, -0.5);
   glRotated(90, 0, 1, 0);
   gluCylinder(quad, 0.04, 0.04, 1.4, 12, 1);
   glPopMatrix();
   
   glPushMatrix();
   glTranslated(-0.7, 1.95, 1.3);
   glRotated(90, 1, 0, 0);
   gluCylinder(quad, 0.04, 0.04, 1.8, 12, 1);
   glPopMatrix();
   
   glPushMatrix();
   glTranslated(0.7, 1.95, 1.3);
   glRotated(90, 1, 0, 0);
   gluCylinder(quad, 0.04, 0.04, 1.8, 12, 1);
   glPopMatrix();
   
   gluDeleteQuadric(quad);
}

/*
 * Drawing wheel with tire tread
 */
static void DrawWheel(double rotation)
{
   glPushMatrix();
   glRotated(rotation, 0, 0, 1);
   
   // Drawing tire
   float tireColor[] = {0.08, 0.08, 0.08, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tireColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 5.0);
   
   GLUquadric* quad = gluNewQuadric();
   
   // Drawing outer tire
   glPushMatrix();
   gluCylinder(quad, 0.4, 0.4, 0.3, 20, 1);
   gluDisk(quad, 0.2, 0.4, 20, 1);
   glTranslated(0, 0, 0.3);
   gluDisk(quad, 0.2, 0.4, 20, 1);
   glPopMatrix();
   
   // Drawing steel wheel rim
   float rimColor[] = {0.25, 0.25, 0.28, 1.0};
   float rimSpec[] = {0.6, 0.6, 0.65, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rimColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, rimSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);
   
   glPushMatrix();
   glTranslated(0, 0, 0.15);
   gluCylinder(quad, 0.18, 0.18, 0.02, 16, 1);
   gluDisk(quad, 0, 0.18, 16, 1);
   glPopMatrix();
   
   // Drawing lug bolts
   for (int i = 0; i < 5; i++)
   {
      double angle = i * 72.0;
      glPushMatrix();
      glTranslated(0.1 * Cos(angle), 0.1 * Sin(angle), 0.14);
      glutSolidSphere(0.015, 8, 8);
      glPopMatrix();
   }
   
   // Drawing tire tread pattern
   glDisable(GL_LIGHTING);
   glColor3f(0.15, 0.15, 0.15);
   glLineWidth(2.0);
   
   for (int i = 0; i < 24; i++)
   {
      double angle = i * 15.0;
      glBegin(GL_LINES);
      glVertex3d(0.38 * Cos(angle), 0.38 * Sin(angle), 0.05);
      glVertex3d(0.38 * Cos(angle), 0.38 * Sin(angle), 0.25);
      glEnd();
   }
   glEnable(GL_LIGHTING);
   
   gluDeleteQuadric(quad);
   glPopMatrix();
}

/*
 * Drawing front bumper with winch
 */
static void DrawFrontBumper(void)
{
   float bumperColor[] = {0.08, 0.08, 0.08, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bumperColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 25.0);
   
   // Drawing main bumper bar
   glPushMatrix();
   glTranslated(0, 0.45, 2.15);
   glScaled(1.6, 0.12, 0.12);
   glutSolidCube(1.0);
   glPopMatrix();
   
   // Drawing bumper end caps
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.8, 0.45, 2.1);
      glScaled(0.1, 0.15, 0.25);
      glutSolidCube(1.0);
      glPopMatrix();
   }
}

/*
 * Drawing rear tire carrier
 */
static void DrawRearTireCarrier(void)
{
   float carrierColor[] = {0.08, 0.08, 0.08, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, carrierColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 25.0);
   
   // Drawing carrier frame
   glBegin(GL_QUADS);
   glNormal3d(0, 0, -1);
   glVertex3d(-0.5, 0.6, -1.3);
   glVertex3d(0.5, 0.6, -1.3);
   glVertex3d(0.5, 1.4, -1.3);
   glVertex3d(-0.5, 1.4, -1.3);
   glEnd();
   
   // Drawing spare tire
   glPushMatrix();
   glTranslated(0, 1.0, -1.32);
   glRotated(90, 0, 1, 0);
   DrawWheel(0);
   glPopMatrix();
}

/*
 * Drawing fender flare
 */
static void DrawFenderFlare(void)
{
   float fenderColor[] = {0.12, 0.15, 0.12, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, fenderColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);

   // Drawing fender flare shape
   glPushMatrix();
   glScaled(0.25, 0.15, 0.5);
   glutSolidSphere(1.0, 16, 16);
   glPopMatrix();
}

/*
 * Drawing complete Jeep Wrangler
 */
void JeepWrangler(double x, double y, double z, double scale,
                 double facing, double time)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   glRotated(facing, 0, 1, 0);
   
   // Drawing body components
   DrawBodyTub();
   DrawHood();
   DrawWindshield();
   DrawRollCage();
   
   // Drawing front grille area
   glPushMatrix();
   glTranslated(0, 0.75, 2.0);
   DrawJeepGrille();
   glPopMatrix();
   
   // Drawing headlights
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.5, 0.75, 2.05);
      DrawHeadlight();
      glPopMatrix();
   }
   
   // Drawing front bumper
   DrawFrontBumper();
   
   // Drawing rear tire carrier
   DrawRearTireCarrier();
   
   // Computing wheel rotation
   double wheelRotation = time * 100.0;
   
   // Drawing front wheels
   glPushMatrix();
   glTranslated(-0.95, 0.4, 1.4);
   glRotated(90, 0, 1, 0);
   DrawWheel(wheelRotation);
   glPopMatrix();
   
   glPushMatrix();
   glTranslated(0.95, 0.4, 1.4);
   glRotated(-90, 0, 1, 0);
   DrawWheel(wheelRotation);
   glPopMatrix();
   
   // Drawing rear wheels
   glPushMatrix();
   glTranslated(-0.95, 0.4, -0.8);
   glRotated(90, 0, 1, 0);
   DrawWheel(wheelRotation);
   glPopMatrix();
   
   glPushMatrix();
   glTranslated(0.95, 0.4, -0.8);
   glRotated(-90, 0, 1, 0);
   DrawWheel(wheelRotation);
   glPopMatrix();
   
   // Drawing fender flares
   double flarePositions[4][2] = {
      {-1.0, 1.5}, {1.0, 1.5},
      {-1.0, -0.7}, {1.0, -0.7}
   };
   
   for (int i = 0; i < 4; i++)
   {
      glPushMatrix();
      glTranslated(flarePositions[i][0], 0.6, flarePositions[i][1]);
      DrawFenderFlare();
      glPopMatrix();
   }
   
   glPopMatrix();
}