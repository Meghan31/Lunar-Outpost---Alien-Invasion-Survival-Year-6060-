/*
 * Armored Transport Vehicle
 * Rendering armored hover vehicle with detailed panels and textures
 */
#include "CSCIx229.h"

// Texture IDs
static unsigned int armorTexture = 0;
static unsigned int metalPlateTexture = 0;
static unsigned int rustTexture = 0;
static unsigned int blueMetalTexture = 0;

/*
 * Loading vehicle textures
 */
static void LoadVehicleTextures(void)
{
   if (armorTexture == 0)
   {
      armorTexture = LoadTexBMP("textures/metals/green_metal_rust.bmp");
      metalPlateTexture = LoadTexBMP("textures/metals/metal_plate.bmp");
      rustTexture = LoadTexBMP("textures/metals/rust_coarse.bmp");
      blueMetalTexture = LoadTexBMP("textures/metals/blue_metal.bmp");
   }
}

/*
 * Drawing bolt or rivet
 */
static void DrawBolt(double radius)
{
   glPushMatrix();
   
   float boltColor[] = {0.3, 0.32, 0.30, 1.0};
   float boltSpec[] = {0.6, 0.62, 0.6, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, boltColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, boltSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);
   
   // Forming bolt head
   glPushMatrix();
   glScaled(1.0, 0.3, 1.0);
   glutSolidSphere(radius, 10, 6);
   glPopMatrix();
   
   // Adding bolt slot
   glDisable(GL_LIGHTING);
   glColor3f(0.05, 0.05, 0.05);
   glLineWidth(2.0);
   glBegin(GL_LINES);
   glVertex3d(-radius * 0.7, radius * 0.32, 0);
   glVertex3d(radius * 0.7, radius * 0.32, 0);
   glEnd();
   glEnable(GL_LIGHTING);
   
   glPopMatrix();
}

/*
 * Drawing hexagonal bolt
 */
static void DrawHexBolt(double size)
{
   float boltColor[] = {0.25, 0.27, 0.25, 1.0};
   float boltSpec[] = {0.7, 0.72, 0.7, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, boltColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, boltSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);
   
   glPushMatrix();
   
   // Creating hexagonal head
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(0, 0, 1);
   glVertex3d(0, 0, size * 0.3);
   for (int i = 0; i <= 6; i++)
   {
      double angle = i * 60 * M_PI / 180.0;
      glVertex3d(size * cos(angle), size * sin(angle), size * 0.3);
   }
   glEnd();
   
   // Building hex sides
   for (int i = 0; i < 6; i++)
   {
      double angle1 = i * 60 * M_PI / 180.0;
      double angle2 = (i + 1) * 60 * M_PI / 180.0;
      double nx = cos(angle1 + M_PI/6);
      double ny = sin(angle1 + M_PI/6);
      
      glBegin(GL_QUADS);
      glNormal3d(nx, ny, 0);
      glVertex3d(size * cos(angle1), size * sin(angle1), size * 0.3);
      glVertex3d(size * cos(angle2), size * sin(angle2), size * 0.3);
      glVertex3d(size * cos(angle2), size * sin(angle2), 0);
      glVertex3d(size * cos(angle1), size * sin(angle1), 0);
      glEnd();
   }
   
   glPopMatrix();
}

/*
 * Drawing armor panel with rivets and welds
 * Took help from AI to finish this part
 */
static void DrawArmorPanel(double width, double height, double depth, int boltCount, int useTexture)
{
   if (useTexture)
   {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, armorTexture);
   }
   
   float panelColor[] = {0.48, 0.52, 0.45, 1.0};
   float panelSpec[] = {0.4, 0.42, 0.4, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, panelColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, panelSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 35.0);
   
   // Drawing main panel face
   glBegin(GL_QUADS);
   glNormal3d(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3d(-width/2, -height/2, depth);
   glTexCoord2f(2, 0); glVertex3d(width/2, -height/2, depth);
   glTexCoord2f(2, 2); glVertex3d(width/2, height/2, depth);
   glTexCoord2f(0, 2); glVertex3d(-width/2, height/2, depth);
   glEnd();
   
   // Adding beveled edges for depth
   double bevel = 0.02;
   glBegin(GL_QUADS);
   
   // Forming bottom edge
   glNormal3d(0, -0.707, 0.707);
   glTexCoord2f(0, 0); glVertex3d(-width/2, -height/2, depth);
   glTexCoord2f(2, 0); glVertex3d(width/2, -height/2, depth);
   glTexCoord2f(2, 0); glVertex3d(width/2 - bevel, -height/2 - bevel, depth - bevel);
   glTexCoord2f(0, 0); glVertex3d(-width/2 + bevel, -height/2 - bevel, depth - bevel);
   
   // Forming top edge
   glNormal3d(0, 0.707, 0.707);
   glTexCoord2f(0, 1); glVertex3d(-width/2 + bevel, height/2 + bevel, depth - bevel);
   glTexCoord2f(2, 1); glVertex3d(width/2 - bevel, height/2 + bevel, depth - bevel);
   glTexCoord2f(2, 1); glVertex3d(width/2, height/2, depth);
   glTexCoord2f(0, 1); glVertex3d(-width/2, height/2, depth);
   
   // Forming left edge
   glNormal3d(-0.707, 0, 0.707);
   glTexCoord2f(0, 0); glVertex3d(-width/2, -height/2, depth);
   glTexCoord2f(0, 1); glVertex3d(-width/2, height/2, depth);
   glTexCoord2f(0, 1); glVertex3d(-width/2 + bevel, height/2 + bevel, depth - bevel);
   glTexCoord2f(0, 0); glVertex3d(-width/2 + bevel, -height/2 - bevel, depth - bevel);
   
   // Forming right edge
   glNormal3d(0.707, 0, 0.707);
   glTexCoord2f(1, 0); glVertex3d(width/2 - bevel, -height/2 - bevel, depth - bevel);
   glTexCoord2f(1, 1); glVertex3d(width/2 - bevel, height/2 + bevel, depth - bevel);
   glTexCoord2f(1, 1); glVertex3d(width/2, height/2, depth);
   glTexCoord2f(1, 0); glVertex3d(width/2, -height/2, depth);
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   // Placing bolts around panel edges
   if (boltCount > 0)
   {
      int boltsPerSide = boltCount / 4;
      
   // Placing top bolts
      for (int i = 0; i < boltsPerSide; i++)
      {
         double x = -width/2 + width * (i + 1.0) / (boltsPerSide + 1);
         glPushMatrix();
         glTranslated(x, height/2 - 0.05, depth + 0.01);
         DrawHexBolt(0.025);
         glPopMatrix();
      }
      
   // Placing bottom bolts
      for (int i = 0; i < boltsPerSide; i++)
      {
         double x = -width/2 + width * (i + 1.0) / (boltsPerSide + 1);
         glPushMatrix();
         glTranslated(x, -height/2 + 0.05, depth + 0.01);
         DrawHexBolt(0.025);
         glPopMatrix();
      }
      
   // Placing left bolts
      for (int i = 0; i < boltsPerSide; i++)
      {
         double y = -height/2 + height * (i + 1.0) / (boltsPerSide + 1);
         glPushMatrix();
         glTranslated(-width/2 + 0.05, y, depth + 0.01);
         DrawHexBolt(0.025);
         glPopMatrix();
      }
      
   // Placing right bolts
      for (int i = 0; i < boltsPerSide; i++)
      {
         double y = -height/2 + height * (i + 1.0) / (boltsPerSide + 1);
         glPushMatrix();
         glTranslated(width/2 - 0.05, y, depth + 0.01);
         DrawHexBolt(0.025);
         glPopMatrix();
      }
   }
   
   // Drawing weld seams
   glDisable(GL_LIGHTING);
   glColor3f(0.18, 0.16, 0.14);
   glLineWidth(2.5);
   
   glBegin(GL_LINE_LOOP);
   glVertex3d(-width/2 + 0.03, -height/2 + 0.03, depth + 0.005);
   glVertex3d(width/2 - 0.03, -height/2 + 0.03, depth + 0.005);
   glVertex3d(width/2 - 0.03, height/2 - 0.03, depth + 0.005);
   glVertex3d(-width/2 + 0.03, height/2 - 0.03, depth + 0.005);
   glEnd();
   
   glEnable(GL_LIGHTING);
}

/*
 * Drawing main hull with armor plates
 */
static void DrawMainHull(void)
{
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, metalPlateTexture);
   
   float hullColor[] = {0.52, 0.54, 0.48, 1.0};
   float hullSpec[] = {0.5, 0.52, 0.5, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hullColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hullSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);
   
   // Drawing lower hull box
   glPushMatrix();
   glTranslated(0, 0.25, 0);
   glScaled(4.5, 0.7, 2.2);
   
   // Drawing bottom face
   glBegin(GL_QUADS);
   glNormal3d(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3d(-0.5, -0.5, -0.5);
   glTexCoord2f(4, 0); glVertex3d(0.5, -0.5, -0.5);
   glTexCoord2f(4, 2); glVertex3d(0.5, -0.5, 0.5);
   glTexCoord2f(0, 2); glVertex3d(-0.5, -0.5, 0.5);
   
   // Drawing top (angled forward)
   glNormal3d(0, 0.9, 0.1);
   glTexCoord2f(0, 0); glVertex3d(-0.5, 0.5, -0.48);
   glTexCoord2f(4, 0); glVertex3d(0.5, 0.5, -0.48);
   glTexCoord2f(4, 2); glVertex3d(0.5, 0.5, 0.5);
   glTexCoord2f(0, 2); glVertex3d(-0.5, 0.5, 0.5);
   
   // Drawing front (angled)
   glNormal3d(0, -0.4, 1);
   glTexCoord2f(0, 0); glVertex3d(-0.48, -0.5, 0.5);
   glTexCoord2f(4, 0); glVertex3d(0.48, -0.5, 0.5);
   glTexCoord2f(4, 1); glVertex3d(0.5, 0.5, 0.5);
   glTexCoord2f(0, 1); glVertex3d(-0.5, 0.5, 0.5);
   
   // Drawing back face
   glNormal3d(0, 0.1, -1);
   glTexCoord2f(0, 0); glVertex3d(-0.5, -0.5, -0.5);
   glTexCoord2f(0, 1); glVertex3d(-0.5, 0.5, -0.48);
   glTexCoord2f(4, 1); glVertex3d(0.5, 0.5, -0.48);
   glTexCoord2f(4, 0); glVertex3d(0.5, -0.5, -0.5);
   
   // Drawing left side (angled)
   glNormal3d(-1, 0.05, 0);
   glTexCoord2f(0, 0); glVertex3d(-0.5, -0.5, -0.5);
   glTexCoord2f(2, 0); glVertex3d(-0.5, -0.5, 0.5);
   glTexCoord2f(2, 1); glVertex3d(-0.5, 0.5, 0.5);
   glTexCoord2f(0, 1); glVertex3d(-0.5, 0.5, -0.48);
   
   // Drawing right side (angled)
   glNormal3d(1, 0.05, 0);
   glTexCoord2f(0, 0); glVertex3d(0.5, -0.5, -0.5);
   glTexCoord2f(0, 1); glVertex3d(0.5, 0.5, -0.48);
   glTexCoord2f(2, 1); glVertex3d(0.5, 0.5, 0.5);
   glTexCoord2f(2, 0); glVertex3d(0.5, -0.5, 0.5);
   
   glEnd();
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   
   // Drawing upper hull section
   glPushMatrix();
   glTranslated(0, 0.8, 0);
   glScaled(4.2, 0.5, 2.0);
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, armorTexture);
   
   glBegin(GL_QUADS);
   // Drawing top surface
   glNormal3d(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3d(-0.5, 0.5, -0.5);
   glTexCoord2f(4, 0); glVertex3d(0.5, 0.5, -0.5);
   glTexCoord2f(4, 2); glVertex3d(0.5, 0.5, 0.5);
   glTexCoord2f(0, 2); glVertex3d(-0.5, 0.5, 0.5);
   
   // Drawing side faces
   glNormal3d(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3d(-0.5, -0.5, -0.5);
   glTexCoord2f(2, 0); glVertex3d(-0.5, -0.5, 0.5);
   glTexCoord2f(2, 1); glVertex3d(-0.5, 0.5, 0.5);
   glTexCoord2f(0, 1); glVertex3d(-0.5, 0.5, -0.5);
   
   glNormal3d(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3d(0.5, -0.5, -0.5);
   glTexCoord2f(0, 1); glVertex3d(0.5, 0.5, -0.5);
   glTexCoord2f(2, 1); glVertex3d(0.5, 0.5, 0.5);
   glTexCoord2f(2, 0); glVertex3d(0.5, -0.5, 0.5);
   
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
   
   // Adding armor panels on sides
   for (int side = -1; side <= 1; side += 2)
   {
   // Drawing large side panel
      glPushMatrix();
      glTranslated(side * 2.28, 0.45, 0);
      glRotated(side * 90, 0, 1, 0);
      DrawArmorPanel(3.2, 1.0, 0.02, 16, 1);
      glPopMatrix();
      
   // Drawing forward detail panel
      glPushMatrix();
      glTranslated(side * 2.28, 0.45, 1.3);
      glRotated(side * 90, 0, 1, 0);
      DrawArmorPanel(0.8, 0.8, 0.02, 8, 1);
      glPopMatrix();
      
   // Drawing rear detail panel
      glPushMatrix();
      glTranslated(side * 2.28, 0.45, -1.3);
      glRotated(side * 90, 0, 1, 0);
      DrawArmorPanel(0.8, 0.8, 0.02, 8, 1);
      glPopMatrix();
      
   // Drawing top side panel
      glPushMatrix();
      glTranslated(side * 2.28, 0.95, 0);
      glRotated(side * 90, 0, 1, 0);
      DrawArmorPanel(2.6, 0.35, 0.02, 12, 1);
      glPopMatrix();
      
   // Drawing storage compartments
      for (int i = -1; i <= 1; i++)
      {
         glPushMatrix();
         glTranslated(side * 2.26, 0.25, i * 0.7);
         glRotated(side * 90, 0, 1, 0);
         DrawArmorPanel(0.45, 0.4, 0.015, 4, 0);
         glPopMatrix();
      }
   }
   
   // Drawing front glacis plate
   glPushMatrix();
   glTranslated(1.8, 0.35, 0);
   glRotated(-25, 1, 0, 0);
   DrawArmorPanel(2.6, 0.9, 0.03, 12, 1);
   glPopMatrix();
   
   // Drawing rear armor plate
   glPushMatrix();
   glTranslated(-2.2, 0.5, 0);
   glRotated(90, 0, 1, 0);
   glRotated(10, 1, 0, 0);
   DrawArmorPanel(2.8, 1.2, 0.03, 16, 1);
   glPopMatrix();
}

/*
 * Drawing angular cockpit with windows
 */
static void DrawCockpit(void)
{
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, blueMetalTexture);
   
   float cockpitColor[] = {0.42, 0.45, 0.43, 1.0};
   float cockpitSpec[] = {0.5, 0.52, 0.5, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cockpitColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cockpitSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 45.0);
   
   glPushMatrix();
   glTranslated(1.3, 1.1, 0);
   
   // Building main cabin structure
   glBegin(GL_QUADS);
   
   // Drawing front window slope
   glNormal3d(0.3, 0.7, 0.6);
   glTexCoord2f(0, 0); glVertex3d(-0.7, -0.1, 0.95);
   glTexCoord2f(1, 0); glVertex3d(0.7, -0.1, 0.95);
   glTexCoord2f(1, 1); glVertex3d(0.6, 0.6, 1.1);
   glTexCoord2f(0, 1); glVertex3d(-0.6, 0.6, 1.1);
   
   // Drawing top armor
   glNormal3d(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3d(-0.6, 0.6, -0.3);
   glTexCoord2f(2, 0); glVertex3d(0.6, 0.6, -0.3);
   glTexCoord2f(2, 1); glVertex3d(0.6, 0.6, 1.1);
   glTexCoord2f(0, 1); glVertex3d(-0.6, 0.6, 1.1);
   
   // Drawing left angled side
   glNormal3d(-0.9, 0.2, 0);
   glTexCoord2f(0, 0); glVertex3d(-0.7, -0.1, 0.95);
   glTexCoord2f(0, 1); glVertex3d(-0.6, 0.6, 1.1);
   glTexCoord2f(1, 1); glVertex3d(-0.6, 0.6, -0.3);
   glTexCoord2f(1, 0); glVertex3d(-0.7, -0.1, -0.35);
   
   // Drawing right angled side
   glNormal3d(0.9, 0.2, 0);
   glTexCoord2f(0, 0); glVertex3d(0.7, -0.1, 0.95);
   glTexCoord2f(1, 0); glVertex3d(0.7, -0.1, -0.35);
   glTexCoord2f(1, 1); glVertex3d(0.6, 0.6, -0.3);
   glTexCoord2f(0, 1); glVertex3d(0.6, 0.6, 1.1);
   
   // Drawing rear wall
   glNormal3d(0, 0.2, -1);
   glTexCoord2f(0, 0); glVertex3d(-0.7, -0.1, -0.35);
   glTexCoord2f(0, 1); glVertex3d(-0.6, 0.6, -0.3);
   glTexCoord2f(1, 1); glVertex3d(0.6, 0.6, -0.3);
   glTexCoord2f(1, 0); glVertex3d(0.7, -0.1, -0.35);
   
   glEnd();
   glDisable(GL_TEXTURE_2D);
   
   // Adding cockpit armor panel detail
   glPushMatrix();
   glTranslated(0, 0.62, 0.3);
   glRotated(90, 1, 0, 0);
   DrawArmorPanel(1.0, 1.2, 0.01, 8, 0);
   glPopMatrix();
   
   // Rendering windows with reflections
   float windowColor[] = {0.12, 0.15, 0.18, 0.85};
   float windowSpec[] = {0.95, 0.97, 1.0, 1.0};
   float windowEmission[] = {0.02, 0.03, 0.05, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, windowColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, windowSpec);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, windowEmission);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
   
   // Enabling transparency for windows
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);
   
   // Drawing main windshield
   glBegin(GL_QUADS);
   glNormal3d(0.3, 0.7, 0.6);
   glVertex3d(-0.52, 0.0, 0.98);
   glVertex3d(0.52, 0.0, 0.98);
   glVertex3d(0.45, 0.55, 1.12);
   glVertex3d(-0.45, 0.55, 1.12);
   glEnd();
   
   // Side windows with frame details
   for (int side = -1; side <= 1; side += 2)
   {
   // Drawing main side window
      glBegin(GL_QUADS);
      glNormal3d(side * 0.9, 0.2, 0.3);
      glVertex3d(side * 0.62, 0.05, 0.4);
      glVertex3d(side * 0.62, 0.05, 0.85);
      glVertex3d(side * 0.52, 0.52, 0.9);
      glVertex3d(side * 0.52, 0.52, 0.35);
      glEnd();
      
   // Drawing small rear side window
      glBegin(GL_QUADS);
      glNormal3d(side * 0.95, 0.2, 0);
      glVertex3d(side * 0.62, 0.05, -0.05);
      glVertex3d(side * 0.62, 0.05, 0.25);
      glVertex3d(side * 0.52, 0.52, 0.25);
      glVertex3d(side * 0.52, 0.52, -0.05);
      glEnd();
   }
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   // Drawing window frames
   float frameColor[] = {0.18, 0.19, 0.18, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, frameColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
   
   glDisable(GL_LIGHTING);
   glColor3f(0.15, 0.15, 0.15);
   glLineWidth(3.5);
   
   // Outlining windshield frame
   glBegin(GL_LINE_LOOP);
   glVertex3d(-0.52, 0.0, 0.98);
   glVertex3d(0.52, 0.0, 0.98);
   glVertex3d(0.45, 0.55, 1.12);
   glVertex3d(-0.45, 0.55, 1.12);
   glEnd();
   
   // Drawing center windshield support
   glBegin(GL_LINES);
   glVertex3d(0, 0.0, 0.98);
   glVertex3d(0, 0.55, 1.12);
   glEnd();
   
   glEnable(GL_LIGHTING);
   
   // Drawing wipers
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(side * 0.25, 0.02, 1.0);
      glRotated(side * 15, 0, 0, 1);
      
      glDisable(GL_LIGHTING);
      glColor3f(0.1, 0.1, 0.1);
      glLineWidth(2.5);
      glBegin(GL_LINES);
      glVertex3d(0, 0, 0);
      glVertex3d(side * 0.35, 0.45, 0.1);
      glEnd();
      glEnable(GL_LIGHTING);
      
      glPopMatrix();
   }
   
   // Adding roof equipment
   glPushMatrix();
   glTranslated(0, 0.65, 0.2);
   
   float equipColor[] = {0.22, 0.23, 0.22, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, equipColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 55.0);
   
   // Drawing antenna
   glPushMatrix();
   glTranslated(0.3, 0, 0);
   glScaled(0.02, 0.4, 0.02);
   glutSolidCube(1.0);
   glPopMatrix();
   
   // Drawing sensor sphere
   glPushMatrix();
   glTranslated(-0.25, 0.05, 0.3);
   glutSolidSphere(0.06, 10, 10);
   glPopMatrix();
   
   glPopMatrix();
   
   glPopMatrix();
}

/*
 * Drawing rear cargo and engine section
 */
static void DrawRearSection(void)
{
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, rustTexture);
   
   float darkColor[] = {0.25, 0.27, 0.24, 1.0};
   float darkSpec[] = {0.35, 0.37, 0.35, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, darkColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, darkSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0);
   
   glPushMatrix();
   glTranslated(-1.9, 0.65, 0);
   
   // Drawing main rear block
   glBegin(GL_QUADS);
   
   // Back face
   glNormal3d(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3d(-0.65, -0.5, -0.95);
   glTexCoord2f(0, 1); glVertex3d(-0.65, 0.5, -0.95);
   glTexCoord2f(1, 1); glVertex3d(0.65, 0.5, -0.95);
   glTexCoord2f(1, 0); glVertex3d(0.65, -0.5, -0.95);
   
   // Top
   glNormal3d(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3d(-0.65, 0.5, -0.95);
   glTexCoord2f(2, 0); glVertex3d(-0.65, 0.5, 0.95);
   glTexCoord2f(2, 1); glVertex3d(0.65, 0.5, 0.95);
   glTexCoord2f(0, 1); glVertex3d(0.65, 0.5, -0.95);
   
   // Left side
   glNormal3d(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3d(-0.65, -0.5, -0.95);
   glTexCoord2f(2, 0); glVertex3d(-0.65, -0.5, 0.95);
   glTexCoord2f(2, 1); glVertex3d(-0.65, 0.5, 0.95);
   glTexCoord2f(0, 1); glVertex3d(-0.65, 0.5, -0.95);
   
   // Right side
   glNormal3d(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3d(0.65, -0.5, -0.95);
   glTexCoord2f(0, 1); glVertex3d(0.65, 0.5, -0.95);
   glTexCoord2f(2, 1); glVertex3d(0.65, 0.5, 0.95);
   glTexCoord2f(2, 0); glVertex3d(0.65, -0.5, 0.95);
   
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
   
   // Adding engine exhaust vents
   float ventColor[] = {0.15, 0.15, 0.15, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ventColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);
   
   for (int i = 0; i < 4; i++)
   {
      double xPos = (i % 2) * 0.5 - 0.25;
      double yPos = (i / 2) * 0.4 + 0.3;
      
      glPushMatrix();
      glTranslated(-2.56, yPos, xPos);
      glRotated(90, 0, 1, 0);
      
      GLUquadric* quad = gluNewQuadric();
      gluCylinder(quad, 0.12, 0.14, 0.08, 16, 1);
      
   // Drawing vent interior
      glPushMatrix();
      glTranslated(0, 0, 0.01);
      float darkVent[] = {0.05, 0.05, 0.05, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, darkVent);
      gluDisk(quad, 0, 0.11, 16, 1);
      glPopMatrix();
      
      gluDeleteQuadric(quad);
      glPopMatrix();
   }
   
   // Adding hydraulic pipes
   glPushMatrix();
   glTranslated(-2.0, 0.4, 0);
   
   float pipeColor[] = {0.28, 0.29, 0.28, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pipeColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 45.0);
   
   for (int i = -1; i <= 1; i++)
   {
      glPushMatrix();
      glTranslated(0, i * 0.15, i * 0.5);
      glRotated(90, 0, 1, 0);
      
      GLUquadric* quad = gluNewQuadric();
      gluCylinder(quad, 0.04, 0.04, 0.8, 8, 1);
      gluDeleteQuadric(quad);
      
      glPopMatrix();
   }
   
   glPopMatrix();
   
   // Drawing cooling vents and grille
   glDisable(GL_LIGHTING);
   glColor3f(0.08, 0.08, 0.08);
   glLineWidth(2.0);
   
   for (int i = 0; i < 12; i++)
   {
      double y = 0.15 + i * 0.08;
      glBegin(GL_LINES);
      glVertex3d(-2.55, y, -0.8);
      glVertex3d(-2.55, y, 0.8);
      glEnd();
   }
   
   // Drawing vertical grille lines
   for (int i = 0; i < 8; i++)
   {
      double z = -0.7 + i * 0.2;
      glBegin(GL_LINES);
      glVertex3d(-2.55, 0.15, z);
      glVertex3d(-2.55, 1.0, z);
      glEnd();
   }
   
   glEnable(GL_LIGHTING);
   
   // Adding rear access panels
   glPushMatrix();
   glTranslated(-2.54, 0.55, 0.5);
   glRotated(90, 0, 1, 0);
   DrawArmorPanel(0.4, 0.4, 0.01, 4, 0);
   glPopMatrix();
   
   glPushMatrix();
   glTranslated(-2.54, 0.55, -0.5);
   glRotated(90, 0, 1, 0);
   DrawArmorPanel(0.4, 0.4, 0.01, 4, 0);
   glPopMatrix();
}

/*
 * Drawing weapon turret
 */
static void DrawWeaponTurret(double time)
{
   glPushMatrix();
   glTranslated(0.4, 1.75, 0);
   glRotated(sin(time * 0.3) * 25, 0, 1, 0);  // Slow scanning rotation
   
   float turretColor[] = {0.32, 0.34, 0.32, 1.0};
   float turretSpec[] = {0.55, 0.57, 0.55, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, turretColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, turretSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);
   
   // Drawing turret base ring
   glPushMatrix();
   glRotated(90, 1, 0, 0);
   GLUquadric* quad = gluNewQuadric();
   gluCylinder(quad, 0.35, 0.3, 0.15, 20, 1);
   gluDisk(quad, 0, 0.35, 20, 1);
   glPopMatrix();
   
   // Drawing turret body
   glPushMatrix();
   glTranslated(0, 0.1, 0);
   glScaled(1.2, 0.5, 0.8);
   glutSolidCube(0.5);
   glPopMatrix();
   
   // Adding turret armor panels
   for (int side = -1; side <= 1; side += 2)
   {
      glPushMatrix();
      glTranslated(0, 0.1, side * 0.42);
      DrawArmorPanel(0.8, 0.35, 0.01, 4, 0);
      glPopMatrix();
   }
   
   // Drawing gun mount
   glPushMatrix();
   glTranslated(0.35, 0.08, 0);
   
   float gunColor[] = {0.22, 0.23, 0.22, 1.0};
   float gunSpec[] = {0.6, 0.62, 0.6, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gunColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, gunSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);
   
   // Building dual barrel assembly
   for (int barrel = -1; barrel <= 1; barrel += 2)
   {
      glPushMatrix();
      glTranslated(0, barrel * 0.08, 0);
      glRotated(90, 0, 1, 0);
      
   // Drawing main barrel
      gluCylinder(quad, 0.06, 0.055, 1.1, 16, 1);
      
   // Adding barrel tip detail
      glPushMatrix();
      glTranslated(0, 0, 1.1);
      gluCylinder(quad, 0.055, 0.07, 0.08, 16, 1);
      glPopMatrix();
      
   // Adding muzzle brake
      glPushMatrix();
      glTranslated(0, 0, 1.18);
      gluCylinder(quad, 0.075, 0.06, 0.05, 16, 1);
      glPopMatrix();
      
   // Adding barrel bands
      for (int band = 0; band < 3; band++)
      {
         glPushMatrix();
         glTranslated(0, 0, 0.3 + band * 0.3);
         gluCylinder(quad, 0.068, 0.068, 0.03, 16, 1);
         glPopMatrix();
      }
      
      glPopMatrix();
   }
   
   // Drawing ammo feed
   glPushMatrix();
   glTranslated(-0.15, 0, 0);
   glRotated(90, 1, 0, 0);
   
   float ammoColor[] = {0.45, 0.42, 0.38, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ammoColor);
   gluCylinder(quad, 0.12, 0.12, 0.25, 12, 1);
   glPopMatrix();
   
   glPopMatrix();
   
   // Drawing targeting sensor
   glPushMatrix();
   glTranslated(0, 0.32, 0);
   
   float sensorColor[] = {0.15, 0.15, 0.18, 1.0};
   float sensorEmission[] = {0.1, 0.05, 0.05, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sensorColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sensorEmission);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80.0);
   
   glutSolidSphere(0.08, 12, 12);
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
   
   gluDeleteQuadric(quad);
   glPopMatrix();
}

/*
 * Drawing hover thrusters
 * Took help from AI to finish this part
 */
static void DrawHoverThrusters(double time)
{
   float thrusterColor[] = {0.18, 0.19, 0.18, 1.0};
   float thrusterSpec[] = {0.7, 0.72, 0.7, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, thrusterColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, thrusterSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 65.0);
   
   // Positioning thruster pods
   double positions[6][2] = {
      {1.7, 0.9},
      {1.7, -0.9},
      {0.2, 0.9},
      {0.2, -0.9},
      {-1.3, 0.9},
      {-1.3, -0.9}
   };
   
   GLUquadric* quad = gluNewQuadric();
   
   for (int i = 0; i < 6; i++)
   {
      glPushMatrix();
      glTranslated(positions[i][0], -0.15, positions[i][1]);
      
   // Drawing thruster housing
      glPushMatrix();
      glRotated(90, 1, 0, 0);
      gluCylinder(quad, 0.28, 0.32, 0.45, 20, 1);
      
   // Drawing housing cap
      gluDisk(quad, 0, 0.28, 20, 1);
      glPopMatrix();
      
   // Drawing inner thruster cone
      glPushMatrix();
      glTranslated(0, -0.02, 0);
      glRotated(90, 1, 0, 0);
      
      float innerColor[] = {0.12, 0.13, 0.12, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, innerColor);
      gluCylinder(quad, 0.22, 0.18, 0.3, 16, 1);
      glPopMatrix();
      
   // Adding hexagonal nozzle pattern
      glPushMatrix();
      glTranslated(0, -0.32, 0);
      glRotated(90, 1, 0, 0);
      
      for (int ring = 0; ring < 7; ring++)
      {
         double radius = (ring == 0) ? 0 : 0.08 + ring * 0.03;
         int count = (ring == 0) ? 1 : 6;
         
         for (int j = 0; j < count; j++)
         {
            double angle = (j * 360.0 / count) * M_PI / 180.0;
            double x = radius * cos(angle);
            double z = radius * sin(angle);
            
            glPushMatrix();
            glTranslated(x, z, 0);
            
            // Drawing hexagonal nozzle
            glBegin(GL_TRIANGLE_FAN);
            glNormal3d(0, 0, 1);
            glVertex3d(0, 0, 0);
            for (int k = 0; k <= 6; k++)
            {
               double hexAngle = k * 60 * M_PI / 180.0;
               glVertex3d(0.025 * cos(hexAngle), 0.025 * sin(hexAngle), 0);
            }
            glEnd();
            
            glPopMatrix();
         }
      }
      glPopMatrix();
      
   // Rendering thruster core glow
      float glowIntensity = 0.7 + 0.3 * sin(time * 6.0 + i * 1.2);
      float thrusterGlow[] = {0.25 * glowIntensity, 0.5 * glowIntensity, 0.95 * glowIntensity, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, thrusterGlow);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, thrusterGlow);
      
      glPushMatrix();
      glTranslated(0, -0.42, 0);
      glScaled(1.0, 0.6, 1.0);
      glutSolidSphere(0.24, 16, 12);
      glPopMatrix();
      
      float noEmission[] = {0, 0, 0, 1};
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
      
   // Rendering thruster glow layers
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glDepthMask(GL_FALSE);
      glDisable(GL_LIGHTING);
      
   // Drawing layered glow
      for (int layer = 0; layer < 3; layer++)
      {
         float alpha = (0.25 - layer * 0.07) * glowIntensity;
         float scale = 1.0 + layer * 0.4;
         
         glColor4f(0.2 * glowIntensity, 0.45 * glowIntensity, 0.95 * glowIntensity, alpha);
         glPushMatrix();
         glTranslated(0, -0.5 - layer * 0.15, 0);
         glScaled(scale, scale * 1.8, scale);
         glutSolidSphere(0.28, 14, 12);
         glPopMatrix();
      }
      
      glEnable(GL_LIGHTING);
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      
   // Drawing support struts
      float strutColor[] = {0.2, 0.21, 0.2, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, strutColor);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
      
      for (int strut = 0; strut < 4; strut++)
      {
         double angle = strut * 90 * M_PI / 180.0;
         double sx = 0.25 * cos(angle);
         double sz = 0.25 * sin(angle);
         
         glPushMatrix();
         glTranslated(sx, 0, sz);
         glRotated(strut * 90, 0, 1, 0);
         glRotated(90, 0, 0, 1);
         glScaled(0.04, 0.25, 0.04);
         glutSolidCube(1.0);
         glPopMatrix();
      }
      
      glPopMatrix();
   }
   
   gluDeleteQuadric(quad);
}

/*
 * Drawing headlights
 */
static void DrawHeadlights(double time)
{
   float housingColor[] = {0.12, 0.12, 0.12, 1.0};
   float housingSpec[] = {0.5, 0.5, 0.5, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, housingColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, housingSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);
   
   double positions[2][2] = {{0.65, 0.95}, {0.65, -0.95}};
   
   GLUquadric* quad = gluNewQuadric();
   
   for (int i = 0; i < 2; i++)
   {
      glPushMatrix();
      glTranslated(positions[i][0], positions[i][1], 2.12);
      
   // Drawing light housing cylinder
      glPushMatrix();
      glRotated(90, 0, 1, 0);
      gluCylinder(quad, 0.14, 0.12, 0.12, 16, 1);
      glPopMatrix();
      
   // Drawing housing back
      glPushMatrix();
      glTranslated(-0.12, 0, 0);
      glRotated(90, 0, 1, 0);
      gluDisk(quad, 0, 0.14, 16, 1);
      glPopMatrix();
      
   // Drawing lens cover
      float lensColor[] = {0.85, 0.87, 0.9, 0.7};
      float lensSpec[] = {0.95, 0.97, 1.0, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lensColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lensSpec);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
      
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(GL_FALSE);
      
      glPushMatrix();
      glRotated(90, 0, 1, 0);
      glutSolidSphere(0.11, 16, 16);
      glPopMatrix();
      
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      
   // Drawing light bulb with glow
      float lightGlow[] = {1.0, 0.95, 0.75, 1.0};
      float lightEmission[] = {0.9, 0.85, 0.6, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lightGlow);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, lightEmission);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
      
      glPushMatrix();
      glTranslated(-0.02, 0, 0);
      glRotated(90, 0, 1, 0);
      glutSolidSphere(0.08, 12, 12);
      glPopMatrix();
      
      float noEmission[] = {0, 0, 0, 1};
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
      
   // Rendering light beam effect
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glDepthMask(GL_FALSE);
      glDisable(GL_LIGHTING);
      
      glColor4f(1.0, 0.95, 0.75, 0.15);
      glPushMatrix();
      glTranslated(0.3, 0, 0);
      glRotated(90, 0, 1, 0);
      glScaled(0.5, 1.2, 1.2);
      GLUquadric* beam = gluNewQuadric();
      gluCylinder(beam, 0.12, 0.35, 1.5, 12, 1);
      gluDeleteQuadric(beam);
      glPopMatrix();
      
      glEnable(GL_LIGHTING);
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      
   // Drawing protective grille
      glDisable(GL_LIGHTING);
      glColor3f(0.15, 0.15, 0.15);
      glLineWidth(1.5);
      
      for (int line = -1; line <= 1; line++)
      {
         glBegin(GL_LINES);
         glVertex3d(0.01, line * 0.08, -0.1);
         glVertex3d(0.01, line * 0.08, 0.1);
         glEnd();
      }
      
      glEnable(GL_LIGHTING);
      glPopMatrix();
   }
   
   gluDeleteQuadric(quad);
}

/*
 * Drawing wheel assemblies
 */
static void DrawWheelAssembly(int side, double time)
{
   glPushMatrix();
   glTranslated(0, 0, side * 2.0);
   
   // Drawing suspension arm
   float armColor[] = {0.18, 0.19, 0.18, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, armColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
   
   // Iterating wheel positions
   double wheelPos[] = {1.8, 0.6, -0.6, -1.8};
   
   GLUquadric* quad = gluNewQuadric();
   
   for (int i = 0; i < 4; i++)
   {
      glPushMatrix();
      glTranslated(wheelPos[i], -0.05, 0);
      
   // Drawing suspension hydraulic
      glPushMatrix();
      glRotated(90, 0, 0, 1);
      gluCylinder(quad, 0.05, 0.05, 0.25, 8, 1);
      glPopMatrix();
      
   // Drawing wheel hub
      glPushMatrix();
      glTranslated(0, 0, side * 0.05);
      glRotated(90 - side * 90, 0, 0, 1);
      glRotated(time * 30, 1, 0, 0);  // Rotating wheel
      
   // Rendering tire
      float tireColor[] = {0.08, 0.08, 0.08, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tireColor);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
      
      glutSolidTorus(0.12, 0.35, 12, 20);
      
   // Rendering wheel rim
      float rimColor[] = {0.28, 0.29, 0.28, 1.0};
      float rimSpec[] = {0.6, 0.62, 0.6, 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rimColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, rimSpec);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 65.0);
      
      gluDisk(quad, 0, 0.28, 20, 1);
      
   // Drawing wheel spokes
      for (int spoke = 0; spoke < 6; spoke++)
      {
         glPushMatrix();
         glRotated(spoke * 60, 0, 0, 1);
         glTranslated(0.15, 0, 0);
         glScaled(0.2, 0.04, 0.05);
         glutSolidCube(1.0);
         glPopMatrix();
      }
      
   // Drawing center hub cap
      glPushMatrix();
      glTranslated(0, 0, 0.03);
      glutSolidSphere(0.08, 12, 12);
      
   // Placing hub bolts
      for (int bolt = 0; bolt < 5; bolt++)
      {
         double angle = bolt * 72 * M_PI / 180.0;
         glPushMatrix();
         glTranslated(0.06 * cos(angle), 0.06 * sin(angle), 0.01);
         DrawBolt(0.015);
         glPopMatrix();
      }
      
      glPopMatrix();
      
      glPopMatrix();
      glPopMatrix();
   }
   
   // Drawing track armor skirt
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, armorTexture);
   
   float skirtColor[] = {0.38, 0.40, 0.37, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skirtColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 35.0);
   
   glBegin(GL_QUADS);
   glNormal3d(0, 0, side);
   glTexCoord2f(0, 0); glVertex3d(2.0, -0.3, side * 0.15);
   glTexCoord2f(4, 0); glVertex3d(-2.0, -0.3, side * 0.15);
   glTexCoord2f(4, 1); glVertex3d(-2.0, 0.1, side * 0.15);
   glTexCoord2f(0, 1); glVertex3d(2.0, 0.1, side * 0.15);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   // Placing skirt bolts
   for (int bolt = 0; bolt < 8; bolt++)
   {
      double x = 1.8 - bolt * 0.5;
      glPushMatrix();
      glTranslated(x, 0, side * 0.16);
      DrawHexBolt(0.02);
      glPopMatrix();
   }
   
   gluDeleteQuadric(quad);
   glPopMatrix();
}

/*
 * Drawing access hatches, doors, and panels
 */
static void DrawAccessPanels(void)
{
   // Drawing main side door
   glPushMatrix();
   glTranslated(-0.2, 0.35, 2.15);
   
   DrawArmorPanel(1.4, 1.2, 0.02, 12, 1);
   
   // Drawing door handle mechanism
   float handleColor[] = {0.22, 0.23, 0.22, 1.0};
   float handleSpec[] = {0.65, 0.67, 0.65, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, handleColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, handleSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);
   
   glPushMatrix();
   glTranslated(0.5, 0, 0.03);
   glRotated(90, 0, 1, 0);
   GLUquadric* quad = gluNewQuadric();
   gluCylinder(quad, 0.03, 0.03, 0.15, 8, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();
   
   // Drawing handle grip
   glPushMatrix();
   glTranslated(0.5, 0, 0.18);
   glScaled(0.08, 0.25, 0.06);
   glutSolidCube(1.0);
   glPopMatrix();
   
   // Drawing hinges
   for (int i = -1; i <= 1; i += 2)
   {
      glPushMatrix();
      glTranslated(-0.65, i * 0.5, 0.03);
      glScaled(0.08, 0.15, 0.05);
      glutSolidCube(1.0);
      glPopMatrix();
   }
   
   // Outlining door frame
   glDisable(GL_LIGHTING);
   glColor3f(0.12, 0.12, 0.12);
   glLineWidth(3.5);
   glBegin(GL_LINE_LOOP);
   glVertex3d(-0.7, -0.6, 0.025);
   glVertex3d(0.7, -0.6, 0.025);
   glVertex3d(0.7, 0.6, 0.025);
   glVertex3d(-0.7, 0.6, 0.025);
   glEnd();
   glEnable(GL_LIGHTING);
   
   glPopMatrix();
   
   // Drawing top hatch
   glPushMatrix();
   glTranslated(-0.5, 1.12, 0);
   glRotated(90, 1, 0, 0);
   
   DrawArmorPanel(0.7, 0.7, 0.01, 8, 0);
   
   // Drawing hatch wheel lock
   float lockColor[] = {0.85, 0.75, 0.25, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lockColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80.0);
   
   glPushMatrix();
   glTranslated(0, 0, 0.03);
   glutSolidTorus(0.04, 0.12, 8, 16);
   
   // Drawing spoke handles
   for (int spoke = 0; spoke < 4; spoke++)
   {
      glPushMatrix();
      glRotated(spoke * 90, 0, 0, 1);
      glTranslated(0.1, 0, 0);
      glScaled(0.12, 0.03, 0.04);
      glutSolidCube(1.0);
      glPopMatrix();
   }
   glPopMatrix();
   
   glPopMatrix();
   
   // Drawing engine access panels on top
   for (int panel = 0; panel < 3; panel++)
   {
      glPushMatrix();
      glTranslated(-1.2 + panel * 0.6, 1.12, 0.5);
      glRotated(90, 1, 0, 0);
      DrawArmorPanel(0.45, 0.45, 0.01, 4, 0);
      glPopMatrix();
      
      glPushMatrix();
      glTranslated(-1.2 + panel * 0.6, 1.12, -0.5);
      glRotated(90, 1, 0, 0);
      DrawArmorPanel(0.45, 0.45, 0.01, 4, 0);
      glPopMatrix();
   }
   
   // Drawing warning markings
   glDisable(GL_LIGHTING);
   
   // Drawing caution stripes on rear
   glColor3f(0.95, 0.85, 0.1);
   glLineWidth(4.0);
   
   for (int stripe = 0; stripe < 6; stripe++)
   {
      double z = -0.6 + stripe * 0.25;
      glBegin(GL_LINES);
      glVertex3d(-2.54, 0.15, z);
      glVertex3d(-2.54, 0.35, z + 0.15);
      glEnd();
   }
   
   glEnable(GL_LIGHTING);
}

/*
 * Drawing armored vehicle
 */
void ArmoredTransportVehicle(double x, double y, double z, double scale,
                             double facing, double time)
{
   // Loading textures if needed
   LoadVehicleTextures();
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   glRotated(facing, 0, 1, 0);
   
   // Applying hover motion and tilt
   double hover = 0.06 * sin(time * 2.2);
   double tilt = 0.8 * sin(time * 1.8);
   glTranslated(0, hover, 0);
   glRotated(tilt, 0, 0, 1);
   
   // Drawing components in order
   DrawMainHull();
   DrawCockpit();
   DrawRearSection();
   DrawWeaponTurret(time);
   DrawHoverThrusters(time);
   DrawHeadlights(time);
   DrawAccessPanels();
   
   // Drawing wheel assemblies with rotation
   DrawWheelAssembly(1, time);
   DrawWheelAssembly(-1, time);
   
   // Adding surface details
   
   // Drawing antenna array
   float antennaColor[] = {0.15, 0.15, 0.15, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, antennaColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);
   
   for (int i = 0; i < 3; i++)
   {
      glPushMatrix();
      glTranslated(0.8 - i * 0.4, 1.75, -0.6 + i * 0.2);
      glScaled(0.015, 0.5 + i * 0.15, 0.015);
      glutSolidCube(1.0);
      glPopMatrix();
   }
   
   // Drawing smoke grenade launchers
   float launcherColor[] = {0.25, 0.27, 0.25, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, launcherColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 55.0);
   
   for (int side = -1; side <= 1; side += 2)
   {
      for (int tube = 0; tube < 4; tube++)
      {
         glPushMatrix();
         glTranslated(0.3, 1.05, side * 1.0);
         glRotated(side * 45, 0, 1, 0);
         glRotated(tube * 8, 1, 0, 0);
         glRotated(90, 0, 1, 0);
         
         GLUquadric* quad = gluNewQuadric();
         gluCylinder(quad, 0.035, 0.035, 0.15, 8, 1);
         gluDeleteQuadric(quad);
         
         glPopMatrix();
      }
   }
   
   // Drawing tow hooks
   float hookColor[] = {0.35, 0.32, 0.3, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hookColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);
   
   glPushMatrix();
   glTranslated(2.1, 0.2, 0);
   glutSolidTorus(0.04, 0.12, 8, 12);
   glPopMatrix();
   
   glPushMatrix();
   glTranslated(-2.4, 0.3, 0);
   glutSolidTorus(0.05, 0.14, 8, 12);
   glPopMatrix();
   
   // Drawing periscope device
   glPushMatrix();
   glTranslated(0.2, 1.8, 0);
   
   float periscopeColor[] = {0.18, 0.19, 0.18, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, periscopeColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 65.0);
   
   glPushMatrix();
   glScaled(0.12, 0.2, 0.12);
   glutSolidCube(1.0);
   glPopMatrix();
   
   // Periscope lenses
   float lensColor[] = {0.1, 0.12, 0.15, 1.0};
   float lensSpec[] = {0.9, 0.92, 0.95, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lensColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lensSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
   
   for (int lens = -1; lens <= 1; lens += 2)
   {
      glPushMatrix();
      glTranslated(0, 0, lens * 0.07);
      glScaled(1.0, 1.0, 0.5);
      glutSolidSphere(0.04, 10, 10);
      glPopMatrix();
   }
   
   glPopMatrix();
   
   glPopMatrix();
}
