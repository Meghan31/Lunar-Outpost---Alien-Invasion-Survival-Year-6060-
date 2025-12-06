/*
 *  Alien Flora Collection - 6 Unique Species
 * Took assistance from AI in building little parts for positioning
 */
#include "CSCIx229.h"

/*
 *  Drawing smooth organic sphere with variation
 * Took help from AI to finish this part
 */
static void DrawOrganicSphere(double r, int detail, double irregularity)
{
   for (int i = 0; i < detail; i++)
   {
      double lat0 = M_PI * (-0.5 + (double)(i) / detail);
      double lat1 = M_PI * (-0.5 + (double)(i + 1) / detail);
      
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= detail; j++)
      {
         double lng = 2 * M_PI * (double)j / detail;
         
         // Adding irregularity
         double var0 = 1.0 + irregularity * sin(lat0 * 5.0) * cos(lng * 7.0);
         double var1 = 1.0 + irregularity * sin(lat1 * 5.0) * cos(lng * 7.0);
         
         double z0 = sin(lat0);
         double zr0 = cos(lat0);
         double z1 = sin(lat1);
         double zr1 = cos(lat1);
         
         double x = cos(lng);
         double y = sin(lng);
         
         glNormal3d(x * zr0 * var0, z0 * var0, y * zr0 * var0);
         glVertex3d(r * x * zr0 * var0, r * z0 * var0, r * y * zr0 * var0);
         
         glNormal3d(x * zr1 * var1, z1 * var1, y * zr1 * var1);
         glVertex3d(r * x * zr1 * var1, r * z1 * var1, r * y * zr1 * var1);
      }
      glEnd();
   }
}

/*
 *  FLORA 1: A Cluster of Glowing Eggs
 */
void AlienFloraEggCluster(double x, double y, double z, double scale, double time)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   
   // Using texture for organic ground base
   float baseColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture colors
   float baseSpec[] = {0.2, 0.25, 0.2, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, baseColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, baseSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
   
   // Root base structure - irregular blob
   glPushMatrix();
   glScaled(1.2, 0.4, 1.0);
   DrawOrganicSphere(0.35, 16, 0.15);
   glPopMatrix();
   
   // Roots spreading out
   for (int i = 0; i < 6; i++)
   {
      glPushMatrix();
      double angle = i * 60.0;
      glRotated(angle, 0, 1, 0);
      glRotated(45, 0, 0, 1);
      
      glBegin(GL_TRIANGLE_STRIP);
      for (int j = 0; j <= 8; j++)
      {
         double t = j / 8.0;
         double thick = 0.03 * (1.0 - t);
         double extend = t * 0.4;
         
         glNormal3d(0, 1, 0);
         glVertex3d(extend - thick, -0.1, 0);
         glVertex3d(extend + thick, -0.1, 0);
      }
      glEnd();
      glPopMatrix();
   }
   
   // Using texture for egg-like pods
   float eggColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
   float eggSpec[] = {1.0, 1.0, 1.0, 1.0};
   float eggGlow[] = {0.2, 0.3, 0.25, 1.0};  // Reduced glow to let texture show
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, eggColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eggSpec);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, eggGlow);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
   
   // Main large egg
   glPushMatrix();
   glTranslated(-0.15, 0.25, 0.1);
   glRotated(20, 0, 0, 1);
   glScaled(1.0, 1.35, 0.95);
   DrawOrganicSphere(0.25, 20, 0.05);
   glPopMatrix();
   
   // Smaller eggs clustered
   double eggPositions[4][3] = {
      {0.15, 0.18, 0.0},
      {0.05, 0.15, -0.2},
      {-0.25, 0.12, -0.15},
      {0.2, 0.22, 0.15}
   };
   
   for (int i = 0; i < 4; i++)
   {
      glPushMatrix();
      glTranslated(eggPositions[i][0], eggPositions[i][1], eggPositions[i][2]);
      glScaled(1.0, 1.25, 0.9);
      double size = 0.12 + (i % 2) * 0.06;
      DrawOrganicSphere(size, 18, 0.05);
      glPopMatrix();
   }
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 *  FLORA 2: Antler-like Branching Structure with Bulbs
 */
void AlienFloraAntlerBranch(double x, double y, double z, double scale, double time)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   
   // Using texture for woody base
   float woodColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, woodColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0);
   
   // Complex root base with texture
   glPushMatrix();
   glScaled(1.3, 0.5, 1.2);
   DrawOrganicSphere(0.3, 16, 0.2);
   glPopMatrix();
   
   // Root tendrils
   for (int i = 0; i < 8; i++)
   {
      glPushMatrix();
      double angle = i * 45.0 + time * 5.0;
      glRotated(angle, 0, 1, 0);
      glRotated(55, 0, 0, 1);
      
      GLUquadric* quad = gluNewQuadric();
      gluCylinder(quad, 0.02, 0.01, 0.3, 8, 1);
      gluDeleteQuadric(quad);
      glPopMatrix();
   }
   
   // Main branching antler structures (2 main branches)
   for (int branch = 0; branch < 2; branch++)
   {
      glPushMatrix();
      double branchAngle = branch * 45.0 - 22.5;
      glRotated(branchAngle, 0, 0, 1);
      
      // Main trunk
      glPushMatrix();
      glRotated(-90, 1, 0, 0);
      GLUquadric* quad = gluNewQuadric();
      gluCylinder(quad, 0.06, 0.04, 0.8, 12, 1);
      gluDeleteQuadric(quad);
      glPopMatrix();
      
      // Sub-branches sprouting from main trunk
      int numSubBranches = 4 + branch * 2;
      for (int sub = 0; sub < numSubBranches; sub++)
      {
         glPushMatrix();
         double subHeight = 0.2 + sub * 0.15;
         double subAngle = (sub % 2 ? 1 : -1) * (25 + sub * 8);
         
         glTranslated(0, subHeight, 0);
         glRotated(subAngle, 0, 0, 1);
         glRotated(sub * 60, 0, 1, 0);
         
         // Branch segment
         glPushMatrix();
         glRotated(-90, 1, 0, 0);
         quad = gluNewQuadric();
         gluCylinder(quad, 0.03, 0.015, 0.25, 10, 1);
         gluDeleteQuadric(quad);
         glPopMatrix();
         
         // Glowing cyan bulb at tip
         glTranslated(0, 0.25, 0);
         
         float bulbColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
         float bulbGlow[] = {0.1, 0.3, 0.35, 1.0};  // Reduced glow
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bulbColor);
         glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, bulbGlow);
         glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80.0);
         
         double bulbSize = 0.04 + 0.02 * sin(time * 2.0 + sub);
         glutSolidSphere(bulbSize, 12, 12);
         
         // Small spiky protrusions
         for (int spike = 0; spike < 4; spike++)
         {
            glPushMatrix();
            glRotated(spike * 90, 0, 1, 0);
            glRotated(45, 1, 0, 0);
            glScaled(0.015, 0.06, 0.015);
            glutSolidCube(1.0);
            glPopMatrix();
         }
         
         glPopMatrix();
      }
      
      glPopMatrix();
   }
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 *  FLORA 3: Vertical Stacked Sphere Cluster
 *  (Third from left - cyan bubbles)
 */
void AlienFloraBubbleStack(double x, double y, double z, double scale, double time)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   
   // Using texture for stem base
   float stemColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stemColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);
   
   // Central stem
   glPushMatrix();
   glRotated(-90, 1, 0, 0);
   GLUquadric* quad = gluNewQuadric();
   gluCylinder(quad, 0.08, 0.06, 1.2, 16, 1);
   gluDeleteQuadric(quad);
   glPopMatrix();
   
   // Segmentation rings on stem
   glDisable(GL_LIGHTING);
   glColor3f(0.1, 0.15, 0.18);
   glLineWidth(2.0);
   for (int ring = 0; ring <= 8; ring++)
   {
      double h = ring * 0.15;
      double r = 0.08 - ring * 0.002;
      glBegin(GL_LINE_LOOP);
      for (int i = 0; i < 16; i++)
      {
         double angle = i * 22.5;
         glVertex3d(r * Cos(angle), h, r * Sin(angle));
      }
      glEnd();
   }
   glEnable(GL_LIGHTING);
   
   // Glowing cyan bubble spheres with texture
   float bubbleColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
   float bubbleSpec[] = {1.0, 1.0, 1.0, 1.0};
   float bubbleGlow[] = {0.08, 0.25, 0.3, 1.0};  // Reduced glow
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bubbleColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bubbleSpec);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, bubbleGlow);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120.0);
   
   // Stacked bubbles with size variation
   double bubbleHeights[] = {0.15, 0.35, 0.58, 0.85, 1.1};
   double bubbleSizes[] = {0.12, 0.15, 0.18, 0.14, 0.1};
   double bubbleOffsets[][2] = {{0.05, 0.03}, {-0.04, -0.02}, {0.03, 0.04}, {-0.02, 0.02}, {0, 0}};
   
   for (int i = 0; i < 5; i++)
   {
      glPushMatrix();
      glTranslated(bubbleOffsets[i][0], bubbleHeights[i], bubbleOffsets[i][1]);
      
      double pulse = 1.0 + 0.08 * sin(time * 1.5 + i * 0.8);
      DrawOrganicSphere(bubbleSizes[i] * pulse, 20, 0.03);
      
      glPopMatrix();
   }
   
   // Smaller satellite bubbles
   for (int i = 0; i < 3; i++)
   {
      double angle = i * 120.0 + time * 15.0;
      double h = 0.6 + i * 0.15;
      double r = 0.18;
      
      glPushMatrix();
      glTranslated(r * Cos(angle), h, r * Sin(angle));
      glutSolidSphere(0.06, 14, 14);
      glPopMatrix();
   }
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 *  FLORA 4: Multi-Branch Coral-like with Sphere Clusters
 */
void AlienFloraCoralBranch(double x, double y, double z, double scale, double time)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   
   // Using texture for organic base
   float baseColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, baseColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 12.0);
   
   // Root mass
   glPushMatrix();
   glScaled(1.5, 0.35, 1.3);
   DrawOrganicSphere(0.35, 16, 0.25);
   glPopMatrix();
   
   // Multiple branching stems (3 main branches)
   for (int mainBranch = 0; mainBranch < 3; mainBranch++)
   {
      glPushMatrix();
      double mainAngle = mainBranch * 120.0 + 30;
      glRotated(mainAngle, 0, 1, 0);
      glRotated(35 + mainBranch * 5, 0, 0, 1);
      
      // Main branch trunk with curve
      glBegin(GL_TRIANGLE_STRIP);
      for (int seg = 0; seg <= 12; seg++)
      {
         double t = seg / 12.0;
         double h = t * 0.9;
         double curve = 0.1 * sin(t * M_PI);
         double thick = 0.05 * (1.0 - t * 0.5);
         
         for (int side = 0; side <= 1; side++)
         {
            double offset = (side ? 1 : -1) * thick;
            glNormal3d(side ? 1 : -1, 0, 0);
            glVertex3d(curve + offset, h, 0);
         }
      }
      glEnd();
      
      // Sub-branches with sphere clusters
      for (int sub = 0; sub < 4; sub++)
      {
         glPushMatrix();
         double subH = 0.25 + sub * 0.2;
         double subAngle = (sub % 2 ? 30 : -30);
         
         glTranslated(0, subH, 0);
         glRotated(subAngle, 0, 0, 1);
         glRotated(sub * 45, 0, 1, 0);
         
         // Branch segment
         GLUquadric* quad = gluNewQuadric();
         glPushMatrix();
         glRotated(-90, 1, 0, 0);
         gluCylinder(quad, 0.03, 0.02, 0.2, 10, 1);
         glPopMatrix();
         gluDeleteQuadric(quad);
         
         // Cyan sphere cluster at end with texture
         glTranslated(0, 0.2, 0);
         
         float sphereColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
         float sphereGlow[] = {0.1, 0.25, 0.3, 1.0};  // Reduced glow
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sphereColor);
         glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sphereGlow);
         glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 90.0);
         
         // 3-5 spheres clustered
         int numSpheres = 3 + (sub % 3);
         for (int s = 0; s < numSpheres; s++)
         {
            glPushMatrix();
            double sAngle = s * (360.0 / numSpheres) + time * 20.0;
            double sRad = 0.06;
            glTranslated(sRad * Cos(sAngle), sRad * Sin(sAngle), 0);
            
            double sSize = 0.04 + 0.01 * sin(time * 2.5 + s);
            DrawOrganicSphere(sSize, 12, 0.05);
            glPopMatrix();
         }
         
         glPopMatrix();
      }
      
      glPopMatrix();
   }
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 *  FLORA 5: Tall Blade-like Structures
 */
void AlienFloraBladeFan(double x, double y, double z, double scale, double time)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   
   // Small root base with texture
   float baseColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, baseColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0);
   
   glPushMatrix();
   glScaled(0.8, 0.3, 0.8);
   DrawOrganicSphere(0.2, 12, 0.15);
   glPopMatrix();
   
   // Translucent glowing blades with texture
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   float bladeColor[] = {1.0, 1.0, 1.0, 0.85};  // White to show texture, slightly transparent
   float bladeSpec[] = {1.0, 0.95, 1.0, 1.0};
   float bladeGlow[] = {0.25, 0.2, 0.3, 1.0};  // Reduced glow
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bladeColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bladeSpec);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, bladeGlow);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
   
   // 5-6 tall curved blades
   // Took help from AI to finish this part
   int numBlades = 6;
   for (int blade = 0; blade < numBlades; blade++)
   {
      glPushMatrix();
      double bladeAngle = blade * (360.0 / numBlades);
      glRotated(bladeAngle, 0, 1, 0);
      glRotated(10 + blade * 3, 0, 0, 1);
      
      // Blade curve - tall and elegant
      double bladeHeight = 1.8 + (blade % 3) * 0.3;
      double bladeWidth = 0.15;
      
      glBegin(GL_TRIANGLE_STRIP);
      for (int seg = 0; seg <= 20; seg++)
      {
         double t = seg / 20.0;
         double h = t * bladeHeight;
         double curve = 0.25 * sin(t * M_PI) * cos(time * 0.5 + blade);
         double width = bladeWidth * (1.0 - t * 0.7) * (1.0 + 0.1 * sin(t * M_PI * 4));
         
         // Double-sided blade
         glNormal3d(0, 0, 1);
         glVertex3d(curve - width, h, 0);
         glVertex3d(curve + width, h, 0);
      }
      glEnd();
      
      // Glowing edge highlight
      glDisable(GL_LIGHTING);
      glColor4f(1.0, 0.95, 1.0, 0.8);
      glLineWidth(2.0);
      glBegin(GL_LINE_STRIP);
      for (int seg = 0; seg <= 20; seg++)
      {
         double t = seg / 20.0;
         double h = t * bladeHeight;
         double curve = 0.25 * sin(t * M_PI) * cos(time * 0.5 + blade);
         glVertex3d(curve, h, 0.001);
      }
      glEnd();
      glEnable(GL_LIGHTING);
      
      glPopMatrix();
   }
   
   glDisable(GL_BLEND);
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 *  FLORA 6: Mushroom-like Cap Cluster
 */
void AlienFloraMushroomCaps(double x, double y, double z, double scale, double time)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(scale, scale, scale);
   
   // Organic ground base with texture
   float baseColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, baseColor);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0);
   
   glPushMatrix();
   glScaled(1.2, 0.4, 1.1);
   DrawOrganicSphere(0.3, 14, 0.2);
   glPopMatrix();
   
   // Multiple mushroom stalks with caps
   double capPositions[][4] = {
      {0, 0.8, 0, 0.3},      // x, height, z, size
      {-0.25, 1.1, 0.15, 0.35},
      {0.2, 0.9, -0.1, 0.25},
      {0.15, 1.3, 0.2, 0.28}
   };
   
   for (int cap = 0; cap < 4; cap++)
   {
      glPushMatrix();
      glTranslated(capPositions[cap][0], 0, capPositions[cap][2]);
      
      // Stalk with texture
      float stalkColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stalkColor);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0);
      
      double stalkHeight = capPositions[cap][1];
      glPushMatrix();
      glRotated(-90, 1, 0, 0);
      GLUquadric* quad = gluNewQuadric();
      gluCylinder(quad, 0.05, 0.06, stalkHeight, 12, 1);
      gluDeleteQuadric(quad);
      glPopMatrix();
      
      // Mushroom cap - curved underside with texture
      glTranslated(0, stalkHeight, 0);
      
      float capColor[] = {1.0, 1.0, 1.0, 1.0};  // White to show texture
      float capGlow[] = {0.15, 0.12, 0.18, 1.0};  // Reduced glow
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, capColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, capGlow);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);
      
      double capSize = capPositions[cap][3];
      
      // Cap dome
      glPushMatrix();
      glScaled(1.0, 0.6, 1.0);
      DrawOrganicSphere(capSize, 18, 0.06);
      glPopMatrix();
      
      // Gill structure underneath
      glDisable(GL_LIGHTING);
      glColor3f(0.6, 0.55, 0.65);
      glLineWidth(1.0);
      
      for (int gill = 0; gill < 16; gill++)
      {
         double gillAngle = gill * 22.5;
         glBegin(GL_LINES);
         glVertex3d(0, -0.02, 0);
         glVertex3d(capSize * 0.9 * Cos(gillAngle), -0.05, capSize * 0.9 * Sin(gillAngle));
         glEnd();
      }
      glEnable(GL_LIGHTING);
      
      glPopMatrix();
   }
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}
