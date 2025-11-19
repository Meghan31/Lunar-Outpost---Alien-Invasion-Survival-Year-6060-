#include "CSCIx229.h"
#include "Plane.c"
#include "FrontPlane.c"


// FPP mode variables
int fpp = 0;          // FPP mode toggle (0=normal, 1=FPP)
double fpx = 0;       // FPP camera X position
double fpy = 1.5;     // FPP camera Y position (eye height)
double fpz = 5;       // FPP camera Z position
double fpAngle = 0;   // FPP viewing angle (horizontal)
double fpPitch = 0;   // FPP pitch angle (vertical look)


int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;        //  Azimuth of view angle
int ph=30;        //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int obj=0;        //  Display mode (0=all, 1=house, 2=tree, 3=ground)
double asp=1;     //  Aspect ratio
double dim=5;     //  Size of world
int ntex=1;       //  Textures on/off
unsigned int texture[15]; // Texture names

// Light values
int light     =   1;  // Lighting
int distance  =   5;  // Light distance
int smooth    =   1;  // Smooth/Flat shading
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   2;  // Elevation of light

// double humanScaleUp = 1.0;  // Human scale multiplier

/*
 *  Ground plane
 *     size: dimension of the square ground
 */
static void ground(double size)
{
   //  Set material properties - grass green
   float green[] = {0.2, 0.6, 0.2, 1.0};
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float white[] = {0.3, 0.3, 0.3, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glColor3f(1, 1, 1);  // White color so texture shows properly
   
   //  Enable textures
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[0]);
   }
   
   //  Drawing ground as a flat square at y=0
   //  Normal points up (0, 1, 0) 
   glNormal3f(0, 1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3f(-size, 0, -size);
   glTexCoord2f(5, 0); glVertex3f(+size, 0, -size);
   glTexCoord2f(5, 5); glVertex3f(+size, 0, +size);
   glTexCoord2f(0, 5); glVertex3f(-size, 0, +size);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
}

/*
 *  Cylinder (for tree trunk, table legs, etc)
 *     at (x,y,z)
 *     radius r, height h
 *     with n segments around circumference
 *     useTexture: 0=no texture, otherwise texture ID to use
 */
static void cylinder(double x, double y, double z,
                     double r, double h, int n)
{
   glPushMatrix();
   glTranslated(x, y, z);
   
   //  Drawing curved surface using quad strip
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= n; i++)
   {
      double angle = 360.0 * i / n;
      double nx = Cos(angle);
      double nz = Sin(angle);
      double vx = r * nx;
      double vz = r * nz;
      
      //  Texture coordinates
      double s = (double)i / n;
      
      //  Normal points are radially outward (perpendicular to Y-axis)
      glNormal3d(nx, 0, nz);
      //  Bottom vertex
      glTexCoord2d(s, 0);
      glVertex3d(vx, 0, vz);
      //  Top vertex
      glTexCoord2d(s, 1);
      glVertex3d(vx, h, vz);
   }
   glEnd();
   
   //  Drawing bottom cap (circle at y=0)
   glNormal3d(0, -1, 0);  // Normal points down
   glBegin(GL_TRIANGLE_FAN);
   glTexCoord2d(0.5, 0.5);
   glVertex3d(0, 0, 0);  // Center point
   for (int i = 0; i <= n; i++)
   {
      double angle = 360.0 * i / n;
      glTexCoord2d(0.5 + 0.5*Cos(angle), 0.5 + 0.5*Sin(angle));
      glVertex3d(r * Cos(angle), 0, r * Sin(angle));
   }
   glEnd();
   
   //  Drawing top cap (circle at y=h)
   glNormal3d(0, 1, 0);  // Normal points up
   glBegin(GL_TRIANGLE_FAN);
   glTexCoord2d(0.5, 0.5);
   glVertex3d(0, h, 0);  // Center point
   for (int i = 0; i <= n; i++)
   {
      double angle = 360.0 * i / n;
      glTexCoord2d(0.5 + 0.5*Cos(angle), 0.5 + 0.5*Sin(angle));
      glVertex3d(r * Cos(angle), h, r * Sin(angle));
   }
   glEnd();
   
   glPopMatrix();
}

/*
 *  Cone (for tree foliage)
 *     at (x,y,z)
 *     base radius r, height h
 *     with n segments around base
 */
static void cone(double x, double y, double z,
                 double r, double h, int n)
{
   glPushMatrix();
   glTranslated(x, y, z);
   
   //  Drawing base (circle at y=0)
   glNormal3d(0, -1, 0);  // Normal points down
   glBegin(GL_TRIANGLE_FAN);
   glTexCoord2d(0.5, 0.5);
   glVertex3d(0, 0, 0);  // Center point
   for (int i = n; i >= 0; i--)  // Reverse order for correct winding
   {
      double angle = 360.0 * i / n;
      glTexCoord2d(0.5 + 0.5*Cos(angle), 0.5 + 0.5*Sin(angle));
      glVertex3d(r * Cos(angle), 0, r * Sin(angle));
   }
   glEnd();
   
   //  Drawing cone sides as triangles from base to apex
   //  For each triangle, we are calculating normal using cross product
   for (int i = 0; i < n; i++)
   {
      double angle1 = 360.0 * i / n;
      double angle2 = 360.0 * (i + 1) / n;
      
      //  Three vertices of the triangle
      double x1 = r * Cos(angle1);
      double z1 = r * Sin(angle1);
      double x2 = r * Cos(angle2);
      double z2 = r * Sin(angle2);
      //  Apex at top
      double x3 = 0;
      double y3 = h;
      double z3 = 0;

      //  Calculating two edge vectors
      //  Edge 1: from vertex 1 to apex
      double dx1 = x3 - x1;
      double dy1 = y3 - 0;
      double dz1 = z3 - z1;
      
      //  Edge 2: from vertex 1 to vertex 2
      double dx2 = x2 - x1;
      double dy2 = 0 - 0;
      double dz2 = z2 - z1;
      
      //  Normal = cross product of edge1 × edge2
      double nx = dy1 * dz2 - dz1 * dy2;
      double ny = dz1 * dx2 - dx1 * dz2;
      double nz = dx1 * dy2 - dy1 * dx2;
      
      //  Drawing the triangle with our calculated normal and texture coords
      glBegin(GL_TRIANGLES);
      glNormal3d(nx, ny, nz);
      glTexCoord2d((double)i/n, 0); glVertex3d(x1, 0, z1);
      glTexCoord2d((double)(i+1)/n, 0); glVertex3d(x2, 0, z2);
      glTexCoord2d((double)i/n + 0.5/n, 1); glVertex3d(x3, y3, z3);
      glEnd();
   }
   
   glPopMatrix();
}

/*
 *  Sphere (for human head, hands, feet)
 *     at (x,y,z)
 *     radius r
 */
static void sphere(double x, double y, double z, double r)
{
   int inc = 10;
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(r, r, r);
   
   // Bands of latitude
   for (int ph = -90; ph < 90; ph += inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += 2*inc)
      {
         double x = Sin(th)*Cos(ph);
         double y = Cos(th)*Cos(ph);
         double z = Sin(ph);
         glNormal3d(x, y, z);
         glVertex3d(x, y, z);
         
         x = Sin(th)*Cos(ph+inc);
         y = Cos(th)*Cos(ph+inc);
         z = Sin(ph+inc);
         glNormal3d(x, y, z);
         glVertex3d(x, y, z);
      }
      glEnd();
   }
   
   glPopMatrix();
}

/*
 *  Human figure
 *     at (x,y,z)
 *     scale s
 */


double humanScaleUp = 1.0;  // Human scale multiplier


//    Use 'h'/'H' keys to decrease/increase humanScaleUp
 
static void human(double x, double y, double z, double s) {
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float skin[] = {0.9, 0.7, 0.6, 1.0};      // Skin tone
   float shirt[] = {0.2, 0.3, 0.8, 1.0};     // Blue shirt //we can change color here for further use
   float pants[] = {0.3, 0.3, 0.3, 1.0};     // Dark grey pants
   float shoes[] = {0.4, 0.25, 0.15, 1.0};     // Brown shoes
   float hair[] = {0.35, 0.2, 0.1, 1.0};    // Brown hair for afro   
   float specular[] = {0.1, 0.1, 0.1, 1.0};
   float hair_specular[] = {0.3, 0.3, 0.3, 1.0};
   
   // Dimensions
   double head_radius = 0.15;
   double torso_radius = 0.12;
   double torso_height = 0.5;
   double arm_radius = 0.05;
   double arm_length = 0.4;
   double leg_radius = 0.06;
   double leg_length = 0.5;
   
   // Adjusting the Y position so feet stay on ground when scaling
   // The feet are at y = -leg_length in local coords, so we need to compensate
   double y_adjusted = y + (leg_length * s * (humanScaleUp - 1.0));
   
   glPushMatrix();
   glTranslated(x, y_adjusted, z);
   glScaled(s * humanScaleUp, s * humanScaleUp, s * humanScaleUp);  // Scaling up
   double hand_radius = 0.06;
   double foot_radius = 0.07;
   
   double head_y = torso_height + head_radius;
   
   // Afro hair (large sphere around head)
   // glColor3f(0.1, 0.05, 0.02);
   // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hair);
   // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hair_specular);
   // glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   // glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 0.5);  // Less shiny for textured look

   // Afro hair (large sphere around head)
glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hair);
glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hair_specular);
glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny );
glColor3f(0.35, 0.2, 0.1);
   
   // Large spherical afro - positioned at head center, extends in all directions
   glPushMatrix();
   glTranslated(0, head_y + head_radius - 0.02, -head_radius);  // Slightly raised
   glScaled(1.8, 1.8, 1.8);  // Much larger - 1.8x the head size for volume
   sphere(0, 0, 0, head_radius);
   glPopMatrix();
   
   // HEAD (face visible at front)
   glColor3f(0.9, 0.7, 0.6);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skin);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   sphere(0, head_y, 0, head_radius);
   
   // TORSO (shirt with texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shirt);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[12]);  // shirt texture
   }
   cylinder(0, 0, 0, torso_radius, torso_height, 16);
   glDisable(GL_TEXTURE_2D);
   
   // LEFT ARM (sleeve with shirt texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shirt);
   
   glPushMatrix();
   glTranslated(-torso_radius, torso_height, 0);
   glRotated(-20, 0, 0, 1);  // Slight angle outward
   glRotated(20, 1, 0, 0);   // Slight angle forward
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[12]);  // shirt texture
   }
   cylinder(0, 0, 0, arm_radius, -arm_length, 12);
   glDisable(GL_TEXTURE_2D);
   
   // Left hand (normal skin color)
   glColor3f(0.9, 0.7, 0.6);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skin);
   sphere(0, -arm_length, 0, hand_radius);
   glPopMatrix();
   
   //  RIGHT ARM (sleeve with shirt texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shirt);
   
   glPushMatrix();
   glTranslated(torso_radius, torso_height, 0);
   glRotated(20, 0, 0, 1);   // Slight angle outward
   glRotated(20, 1, 0, 0);   // Slight angle forward
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[12]);  // shirt texture
   }
   cylinder(0, 0, 0, arm_radius, -arm_length, 12);
   glDisable(GL_TEXTURE_2D);
   
   // Right hand (normal skin color)
   glColor3f(0.9, 0.7, 0.6);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skin);
   sphere(0, -arm_length, 0, hand_radius);
   glPopMatrix();
   
   //  LEFT LEG (pants with texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pants);
   
   glPushMatrix();
   glTranslated(-torso_radius/2, 0, 0);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[13]);  // pant texture
   }
   cylinder(0, 0, 0, leg_radius, -leg_length, 12);
   glDisable(GL_TEXTURE_2D);
   
   // Left foot (shoe with texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shoes);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[14]);  // shoe texture
   }
   
   glPushMatrix();
   glTranslated(0, -leg_length, foot_radius);
   glScaled(1.5, 1.0, 2.0);  // Elongate forward for shoe shape
   sphere(0, 0, 0, foot_radius);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();

   // RIGHT LEG (pants with texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pants);
   
   glPushMatrix();
   glTranslated(torso_radius/2, 0, 0);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[13]);  // pant texture
   }
   cylinder(0, 0, 0, leg_radius, -leg_length, 12);
   glDisable(GL_TEXTURE_2D);
   
   // Right foot (shoe with texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shoes);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[14]);  // shoe texture
   }
   
   glPushMatrix();
   glTranslated(0, -leg_length, foot_radius);
   glScaled(1.5, 1.0, 2.0);  // Elongate forward for shoe shape
   sphere(0, 0, 0, foot_radius);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
   
   glPopMatrix();
}

/*
 *  Tree
 *     at (x,y,z)
 *     scale s
 */

double treeScaleUp = 3.1;  // Tree scale multiplier //3.100000 looks good


static void tree(double x, double y, double z, double s)
{
   //  Set material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float brown[] = {0.4, 0.25, 0.1, 1.0};
   float green[] = {0.1, 0.5, 0.1, 1.0};
   float specular[] = {0.1, 0.1, 0.1, 1.0};
   
   // Tree dimensions (will be scaled by treeScaleUp)
   double trunk_radius = 0.1;
   double trunk_height = 0.4;
   double foliage_radius = 0.4;
   double foliage_height = 0.6;
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * treeScaleUp, s * treeScaleUp, s * treeScaleUp);
   
   //  Drawing trunk (brown cylinder with bark texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brown);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[3]);  // bark texture
   }
   cylinder(0, 0, 0, trunk_radius, trunk_height, 16);
   glDisable(GL_TEXTURE_2D);
   
   //  Drawing foliage (green cone with leaves texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[4]);  // leaves texture
   }
   //  Position cone at top of trunk
   cone(0, trunk_height, 0, foliage_radius, foliage_height, 16);
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}
/*
 *  House (cabin-type)
 *     at (x,y,z)
 *     scale s
 */

 double houseScaleUp = 2.0;  // House scale multiplier //3.400000 looks good

static void house(double x, double y, double z, double s)
{
   //  Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float grey[] = {0.5, 0.5, 0.5, 1.0};
   float dark_grey[] = {0.3, 0.3, 0.3, 1.0};
   float brown[] = {0.4, 0.2, 0.0, 1.0};
   float specular[] = {0.2, 0.2, 0.2, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * houseScaleUp, s * houseScaleUp, s * houseScaleUp);
   
   //  WALLS 
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   double wall_width = 0.8;
   double wall_height = 1.0;
   double wall_depth = 0.6;
   
   //  Enable textures for walls
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[1]);
   }
   
   glBegin(GL_QUADS);
   
   //  Front face (with door - we'll draw door separately)
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-wall_width, 0, wall_depth);
   glTexCoord2f(1, 0); glVertex3f(+wall_width, 0, wall_depth);
   glTexCoord2f(1, 1); glVertex3f(+wall_width, wall_height, wall_depth);
   glTexCoord2f(0, 1); glVertex3f(-wall_width, wall_height, wall_depth);
   
   //  Back face
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(+wall_width, 0, -wall_depth); //right-bottom
   glTexCoord2f(1, 0); glVertex3f(-wall_width, 0, -wall_depth); //left-bottom
   glTexCoord2f(1, 1); glVertex3f(-wall_width, wall_height+0.2, -wall_depth); //left-top
   glTexCoord2f(0, 1); glVertex3f(+wall_width, wall_height+0.2, -wall_depth); //right-top
   
   //  Right face
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(+wall_width, 0, +wall_depth); //bottom-left
   glTexCoord2f(1, 0); glVertex3f(+wall_width, 0, -wall_depth); //bottom-right
   glTexCoord2f(1, 1); glVertex3f(+wall_width, wall_height+0.2, -wall_depth); //top-right
   glTexCoord2f(0, 1); glVertex3f(+wall_width, wall_height, +wall_depth); //top-left
   
   //  Left face
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-wall_width, 0, -wall_depth); //bottom-right
   glTexCoord2f(1, 0); glVertex3f(-wall_width, 0, +wall_depth); //bottom-left
   glTexCoord2f(1, 1); glVertex3f(-wall_width, wall_height, +wall_depth); //top-left
   glTexCoord2f(0, 1); glVertex3f(-wall_width, wall_height+0.2, -wall_depth); //top-right
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   //  FLAT SLANTED ROOF 
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dark_grey);
   
   double roof_width = wall_width + 0.1;
   double roof_depth = wall_depth + 0.1;
   double roof_height = 0.15;
   double roof_y = wall_height;
   
   //  Enable textures for roof
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[2]);  // roof texture
   }
   
   glBegin(GL_QUADS);
   
   //  Roof top
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-roof_width, roof_y + roof_height+0.2, -roof_depth);
   glTexCoord2f(1, 0); glVertex3f(+roof_width, roof_y + roof_height+0.2, -roof_depth);
   glTexCoord2f(1, 1); glVertex3f(+roof_width, roof_y + roof_height, +roof_depth);
   glTexCoord2f(0, 1); glVertex3f(-roof_width, roof_y + roof_height, +roof_depth);
   
   //  Roof bottom
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(0, 1); glVertex3f(-roof_width, roof_y, +roof_depth);
   glTexCoord2f(1, 1); glVertex3f(+roof_width, roof_y, +roof_depth);
   glTexCoord2f(1, 0); glVertex3f(+roof_width, roof_y+0.2, -roof_depth);
   
   //  Roof front edge
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-roof_width, roof_y, roof_depth);
   glTexCoord2f(1, 0); glVertex3f(+roof_width, roof_y, roof_depth);
   glTexCoord2f(1, 1); glVertex3f(+roof_width, roof_y + roof_height, roof_depth);
   glTexCoord2f(0, 1); glVertex3f(-roof_width, roof_y + roof_height, roof_depth);
   
   //  Roof back edge
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(+roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(1, 0); glVertex3f(-roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(1, 1); glVertex3f(-roof_width, roof_y + roof_height+0.2, -roof_depth);
   glTexCoord2f(0, 1); glVertex3f(+roof_width, roof_y + roof_height+0.2, -roof_depth);
   
   //  Roof right edge
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(+roof_width, roof_y, +roof_depth);
   glTexCoord2f(1, 0); glVertex3f(+roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(1, 1); glVertex3f(+roof_width, roof_y + roof_height+0.2, -roof_depth);
   glTexCoord2f(0, 1); glVertex3f(+roof_width, roof_y + roof_height, +roof_depth);
   
   //  Roof left edge
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(1, 0); glVertex3f(-roof_width, roof_y, +roof_depth);
   glTexCoord2f(1, 1); glVertex3f(-roof_width, roof_y + roof_height, +roof_depth);
   glTexCoord2f(0, 1); glVertex3f(-roof_width, roof_y + roof_height+0.2, -roof_depth);
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   //  DOOR (with texture)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brown);
   
   double door_width = 0.25;
   double door_height = 0.5;
   double door_z = wall_depth + 0.01;  // Slightly in front to avoid z-fighting
   
   //  Enable textures for door
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);  // roof texture for door
   }
   
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-door_width, 0, door_z);
   glTexCoord2f(1, 0); glVertex3f(+door_width, 0, door_z);
   glTexCoord2f(1, 1); glVertex3f(+door_width, door_height, door_z);
   glTexCoord2f(0, 1); glVertex3f(-door_width, door_height, door_z);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}


/*
 *  Archery target
 *     at (x,y,z)
 *     scale s
 */

//  Used AI for the colored circles logic

double targetScaleUp = 1.0;  // Target scale multiplier //1.0 looks good for now
static void target(double x, double y, double z, double s)
{
   //  Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
//    float white[] = {1.0, 1.0, 1.0, 1.0};
   float specular[] = {0.3, 0.3, 0.3, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * targetScaleUp, s * targetScaleUp, s * targetScaleUp);
   
   //  POLE STAND (with rusty_car texture) 
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   //  Enable textures for pole
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[7]);  // rusty_car texture
   }
   
   double pole_radius = 0.05;
   double pole_height = 1.0;
   cylinder(0, 0, 0, pole_radius, pole_height, 12); //using the tree trunk function for pole
   
   glDisable(GL_TEXTURE_2D);

   //  TARGET FACE (at top of pole)
   double target_y = pole_height;
   double target_radius = 0.5;
   double target_thickness = 0.05;
   int segments = 32;  // Smooth circles
   
   //  Target backing (thin cylinder for thickness with table_plank texture)
   glColor3f(1, 1, 1);  // White for texture
   float light_grey[] = {0.9, 0.9, 0.9, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, light_grey);
   
   //  Enable textures for target board
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);  // table_plank texture
   }
   
   //  Drawing thin cylinder for target body
   glPushMatrix();
   glTranslated(0, target_y, 0);
   glRotated(90, 1, 0, 0);  // Rotate to face forward
   cylinder(0, 0, 0, target_radius, target_thickness, segments);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   
   //  COLORED RINGS (front face - vertical, attached to backing) 
   // The cylinder is rotated 90° around X, so its front face is at z = -target_thickness
   // Rings should be in XY plane (vertical) at z position just beyond the cylinder
   double ring_z = -target_thickness + 0.025;  // Slightly in front of cylinder face
   
   //  Helper arrays for ring colors and radii
   float colors[5][3] = {
      {1.0, 1.0, 1.0},  // White (outer)
      {0.0, 0.0, 0.0},  // Black
      {0.0, 0.4, 0.9},  // Blue
      {0.9, 0.1, 0.1},  // Red
      {1.0, 1.0, 0.0}   // Yellow (center)
   };
   
   double radii[5] = {
      target_radius * 1.0,   // White outer
      target_radius * 0.8,   // Black
      target_radius * 0.6,   // Blue
      target_radius * 0.4,   // Red
      target_radius * 0.2    // Yellow center
   };
   
   //  Drawing each ring in XY plane (vertical, facing forward)
   for (int ring = 0; ring < 5; ring++)
   {
      glColor3f(colors[ring][0], colors[ring][1], colors[ring][2]);
      float mat_color[] = {colors[ring][0], colors[ring][1], colors[ring][2], 1.0};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_color);
      
      double outer_r = (ring == 0) ? radii[ring] : radii[ring-1];
      double inner_r = radii[ring];
      
      //  Normal points forward in -Z direction (toward viewer)
      glNormal3f(0, 0, -1);
      
      //  Drawing ring as quad strip in XY plane (vertical)
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= segments; i++)
      {
         double angle = 360.0 * i / segments;
         //  Outer vertex (X varies, Y varies, Z constant)
         glVertex3f(outer_r * Cos(angle), target_y + outer_r * Sin(angle), ring_z);
         //  Inner vertex
         glVertex3f(inner_r * Cos(angle), target_y + inner_r * Sin(angle), ring_z);
      }
      glEnd();
   }
   
   //  Center circle (yellow bullseye) in XY plane
   glColor3f(1.0, 1.0, 0.0);
   float yellow[] = {1.0, 1.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, yellow);
   
   glNormal3f(0, 0, -1);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3f(0, target_y, ring_z);  // Center at pole height
   for (int i = 0; i <= segments; i++)
   {
      double angle = 360.0 * i / segments;
      glVertex3f(radii[4] * Cos(angle), target_y + radii[4] * Sin(angle), ring_z);
   }
   glEnd();
   
   
   glPopMatrix();
}

/*
 *  Broken table (two pieces fallen on ground)
 *     at (x,y,z)
 *     scale s
 */
double tableScaleUp = 1.0;  // Table scale multiplier //1.0 looks good 
static void broken_table(double x, double y, double z, double s)
{
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float wood[] = {0.4, 0.25, 0.15, 1.0};      // Brown wood color
   // float dark_wood[] = {0.3, 0.18, 0.10, 1.0}; // Darker wood for edges
   float specular[] = {0.2, 0.2, 0.2, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * tableScaleUp, s * tableScaleUp, s * tableScaleUp);
   
   // Dimensions
   double table_thickness = 0.08;
   
   // === LEFT PIECE (larger, fallen on ground tilted left) ===
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wood);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   //  Enable textures for table top
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);  // table plank texture
   }
   
   glPushMatrix();
   glTranslated(-0.8, 0.35, 0);  // Position
   glRotated(-50, 0, 0, 1);      // Tilt so it lays on ground
   
   // Top surface
   glBegin(GL_QUADS);
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.6, table_thickness, -0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, table_thickness, -0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, table_thickness, 0.4);
   glEnd();
   
   // Bottom surface (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.6, 0, -0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, 0, 0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, 0, 0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.4);
   glEnd();
   
   // Front edge (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-0.6, 0, 0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, table_thickness, 0.4);
   glEnd();
   
   // Back edge (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(-0.6, 0, -0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, table_thickness, -0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, -0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.4);
   glEnd();
   
   // Left edge (with texture)
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.6, 0, -0.4);
   glTexCoord2f(1, 0); glVertex3f(-0.6, 0, 0.4);
   glTexCoord2f(1, 1); glVertex3f(-0.6, table_thickness, 0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, table_thickness, -0.4);
   glEnd();
   
   // Right edge (broken/jagged) (with texture)
   glBegin(GL_QUADS);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(0.5, 0, -0.4);
   glTexCoord2f(0, 1); glVertex3f(0.5, table_thickness, -0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0.4);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   // Left table leg 1 (at positive z, extends down to ground)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wood);
   
   //  Enable textures for table legs
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[6]);  // table legs texture
   }
   
   double leg1_y_bottom = -0.75;  // Extended to reach ground
   
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(-0.3, table_thickness, 0.3);
   glVertex3f(-0.2, table_thickness, 0.3);
   glVertex3f(-0.2, leg1_y_bottom, 0.3);
   glVertex3f(-0.3, leg1_y_bottom, 0.3);
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(-0.3, table_thickness, 0.2);
   glVertex3f(-0.3, leg1_y_bottom, 0.2);
   glVertex3f(-0.2, leg1_y_bottom, 0.2);
   glVertex3f(-0.2, table_thickness, 0.2);
   // Left face
   glNormal3f(-1, 0, 0);
   glVertex3f(-0.3, table_thickness, 0.2);
   glVertex3f(-0.3, table_thickness, 0.3);
   glVertex3f(-0.3, leg1_y_bottom, 0.3);
   glVertex3f(-0.3, leg1_y_bottom, 0.2);
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(-0.2, table_thickness, 0.2);
   glVertex3f(-0.2, leg1_y_bottom, 0.2);
   glVertex3f(-0.2, leg1_y_bottom, 0.3);
   glVertex3f(-0.2, table_thickness, 0.3);
   // Bottom face
   glNormal3f(0, -1, 0);
   glVertex3f(-0.3, leg1_y_bottom, 0.2);
   glVertex3f(-0.3, leg1_y_bottom, 0.3);
   glVertex3f(-0.2, leg1_y_bottom, 0.3);
   glVertex3f(-0.2, leg1_y_bottom, 0.2);
   glEnd();
   
   // Left table leg 2 (at negative z, extends down to ground)
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(-0.3, table_thickness, -0.2);
   glVertex3f(-0.2, table_thickness, -0.2);
   glVertex3f(-0.2, leg1_y_bottom, -0.2);
   glVertex3f(-0.3, leg1_y_bottom, -0.2);
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(-0.3, table_thickness, -0.3);
   glVertex3f(-0.3, leg1_y_bottom, -0.3);
   glVertex3f(-0.2, leg1_y_bottom, -0.3);
   glVertex3f(-0.2, table_thickness, -0.3);
   // Left face
   glNormal3f(-1, 0, 0);
   glVertex3f(-0.3, table_thickness, -0.3);
   glVertex3f(-0.3, table_thickness, -0.2);
   glVertex3f(-0.3, leg1_y_bottom, -0.2);
   glVertex3f(-0.3, leg1_y_bottom, -0.3);
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(-0.2, table_thickness, -0.3);
   glVertex3f(-0.2, leg1_y_bottom, -0.3);
   glVertex3f(-0.2, leg1_y_bottom, -0.2);
   glVertex3f(-0.2, table_thickness, -0.2);
   // Bottom face
   glNormal3f(0, -1, 0);
   glVertex3f(-0.3, leg1_y_bottom, -0.3);
   glVertex3f(-0.3, leg1_y_bottom, -0.2);
   glVertex3f(-0.2, leg1_y_bottom, -0.2);
   glVertex3f(-0.2, leg1_y_bottom, -0.3);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
   
   // === RIGHT PIECE (smaller, fallen on ground tilted right) ===
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wood);
   
   glPushMatrix();
   glTranslated(0.9, 0.25, 0);   // Position
   glRotated(45, 0, 0, 1);       // Tilt opposite direction
   glRotated(-15, 1, 0, 0);      // Slight forward tilt
   
   //  Enable textures for second table piece
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);  // table plank texture
   }
   
   // Top surface
   glBegin(GL_QUADS);
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.4, table_thickness, -0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, table_thickness, -0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, table_thickness, 0.35);
   glEnd();
   
   // Bottom surface (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.4, 0, -0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, 0, 0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, 0, 0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.35);
   glEnd();
   
   // Front edge (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-0.4, 0, 0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, table_thickness, 0.35);
   glEnd();
   
   // Back edge (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(-0.4, 0, -0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, table_thickness, -0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, -0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.35);
   glEnd();
   
   // Left edge (broken/jagged) (with texture)
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.4, 0, -0.35);
   glTexCoord2f(1, 0); glVertex3f(-0.4, 0, 0.35);
   glTexCoord2f(1, 1); glVertex3f(-0.4, table_thickness, 0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, table_thickness, -0.35);
   glEnd();
   
   // Right edge (with texture)
   glBegin(GL_QUADS);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(0.5, 0, -0.35);
   glTexCoord2f(0, 1); glVertex3f(0.5, table_thickness, -0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0.35);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   // Right table leg 1 (at positive z, extends down to ground)
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wood);
   
   //  Enable textures for second set of table legs
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[6]);  // table legs texture
   }
   
   double leg2_y_bottom = -0.5;  // Extended to reach ground
   
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(0.1, table_thickness, 0.25);
   glVertex3f(0.2, table_thickness, 0.25);
   glVertex3f(0.2, leg2_y_bottom, 0.25);
   glVertex3f(0.1, leg2_y_bottom, 0.25);
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(0.1, table_thickness, 0.15);
   glVertex3f(0.1, leg2_y_bottom, 0.15);
   glVertex3f(0.2, leg2_y_bottom, 0.15);
   glVertex3f(0.2, table_thickness, 0.15);
   // Left face
   glNormal3f(-1, 0, 0);
   glVertex3f(0.1, table_thickness, 0.15);
   glVertex3f(0.1, table_thickness, 0.25);
   glVertex3f(0.1, leg2_y_bottom, 0.25);
   glVertex3f(0.1, leg2_y_bottom, 0.15);
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(0.2, table_thickness, 0.15);
   glVertex3f(0.2, leg2_y_bottom, 0.15);
   glVertex3f(0.2, leg2_y_bottom, 0.25);
   glVertex3f(0.2, table_thickness, 0.25);
   // Bottom face
   glNormal3f(0, -1, 0);
   glVertex3f(0.1, leg2_y_bottom, 0.15);
   glVertex3f(0.1, leg2_y_bottom, 0.25);
   glVertex3f(0.2, leg2_y_bottom, 0.25);
   glVertex3f(0.2, leg2_y_bottom, 0.15);
   glEnd();
   
   // Right table leg 2 (at negative z, extends down to ground)
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(0.1, table_thickness, -0.15);
   glVertex3f(0.2, table_thickness, -0.15);
   glVertex3f(0.2, leg2_y_bottom, -0.15);
   glVertex3f(0.1, leg2_y_bottom, -0.15);
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(0.1, table_thickness, -0.25);
   glVertex3f(0.1, leg2_y_bottom, -0.25);
   glVertex3f(0.2, leg2_y_bottom, -0.25);
   glVertex3f(0.2, table_thickness, -0.25);
   // Left face
   glNormal3f(-1, 0, 0);
   glVertex3f(0.1, table_thickness, -0.25);
   glVertex3f(0.1, table_thickness, -0.15);
   glVertex3f(0.1, leg2_y_bottom, -0.15);
   glVertex3f(0.1, leg2_y_bottom, -0.25);
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(0.2, table_thickness, -0.25);
   glVertex3f(0.2, leg2_y_bottom, -0.25);
   glVertex3f(0.2, leg2_y_bottom, -0.15);
   glVertex3f(0.2, table_thickness, -0.15);
   // Bottom face
   glNormal3f(0, -1, 0);
   glVertex3f(0.1, leg2_y_bottom, -0.25);
   glVertex3f(0.1, leg2_y_bottom, -0.15);
   glVertex3f(0.2, leg2_y_bottom, -0.15);
   glVertex3f(0.2, leg2_y_bottom, -0.25);
   glEnd();
   
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}




/*
 *  Old abandoned car 
 *     at (x,y,z)
 *     scale s
 */

 double carScaleUp = 2.3; //2.300000 looks good

static void old_car(double x, double y, double z, double s)
{
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float car_body[] = {0.8, 0.3, 0.2, 1.0};      // Rusty orange/red exterior
   float dark_interior[] = {0.4, 0.15, 0.1, 1.0}; // Darker red interior
   float wheel_well[] = {0.3, 0.1, 0.08, 1.0};    // Very dark red for wheel wells
   float specular[] = {0.1, 0.1, 0.1, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * carScaleUp, s * carScaleUp, s * carScaleUp);
   
   // Dimensions
   double body_width = 0.5;
   double body_height = 0.3;
   double cabin_height = 0.4;
   double cabin_start = -0.1;
   double cabin_end = cabin_start + 0.7;
   double cabin_top = body_height + cabin_height;
   double hood_length = 0.6;
   double wall_thickness = 0.05;
   
   //  LOWER BODY (main chassis) 
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, car_body);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   //  Enable textures for car
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[7]);  // rusty car texture
   }
   
   // Main body box
   glBegin(GL_QUADS);
   
   // Top
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-hood_length, body_height, -body_width);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, body_height, -body_width);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, body_height, body_width);
   glTexCoord2f(0, 1); glVertex3f(-hood_length, body_height, body_width);
   
   // Bottom (sitting on ground)
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-hood_length, 0, -body_width);
   glTexCoord2f(0, 1); glVertex3f(-hood_length, 0, body_width);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, 0, body_width);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, 0, -body_width);
   
   // Front
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-hood_length, 0, -body_width);
   glTexCoord2f(1, 0); glVertex3f(-hood_length, 0, body_width);
   glTexCoord2f(1, 1); glVertex3f(-hood_length, body_height, body_width);
   glTexCoord2f(0, 1); glVertex3f(-hood_length, body_height, -body_width);
   
   // Back
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(cabin_end, 0, -body_width);
   glTexCoord2f(0, 1); glVertex3f(cabin_end, body_height, -body_width);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, body_height, body_width);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, 0, body_width);
   
   // Left side
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(-hood_length, 0, -body_width);
   glTexCoord2f(0, 1); glVertex3f(-hood_length, body_height, -body_width);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, body_height, -body_width);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, 0, -body_width);
   
   // Right side
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-hood_length, 0, body_width);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, 0, body_width);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, body_height, body_width);
   glTexCoord2f(0, 1); glVertex3f(-hood_length, body_height, body_width);
   
   glEnd();
   
   //  Hollow cabin structure (with windows and walls)
   double cabin_width_outer = body_width * 0.8;
   double cabin_width_inner = cabin_width_outer - wall_thickness;
   
   // Cabin roof (top) (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(cabin_start, cabin_top, -cabin_width_outer);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, cabin_top, -cabin_width_outer);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, cabin_top, cabin_width_outer);
   glTexCoord2f(0, 1); glVertex3f(cabin_start, cabin_top, cabin_width_outer);
   glEnd();
   
   // Front wall (exterior) (with texture)
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(cabin_start, body_height, -cabin_width_outer);
   glTexCoord2f(1, 0); glVertex3f(cabin_start, body_height, cabin_width_outer);
   glTexCoord2f(1, 1); glVertex3f(cabin_start, cabin_top, cabin_width_outer);
   glTexCoord2f(0, 1); glVertex3f(cabin_start, cabin_top, -cabin_width_outer);
   glEnd();
   
   // Back wall (exterior) (with texture)
   glBegin(GL_QUADS);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(cabin_end, body_height, -cabin_width_outer);
   glTexCoord2f(0, 1); glVertex3f(cabin_end, cabin_top, -cabin_width_outer);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, cabin_top, cabin_width_outer);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, body_height, cabin_width_outer);
   glEnd();
   
   // Window dimensions
   double window1_start = cabin_start + 0.1;
   double window1_end = cabin_start + 0.35;
   double window2_start = cabin_start + 0.4;
   double window2_end = cabin_end - 0.1;
   double window_bottom = body_height + 0.05;
   double window_top = cabin_top - 0.05;
   
   // Left outer wall with window openings (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   // Left of first window
   glTexCoord2f(0, 0); glVertex3f(cabin_start, body_height, -cabin_width_outer);
   glTexCoord2f(0.2, 0); glVertex3f(window1_start, body_height, -cabin_width_outer);
   glTexCoord2f(0.2, 1); glVertex3f(window1_start, cabin_top, -cabin_width_outer);
   glTexCoord2f(0, 1); glVertex3f(cabin_start, cabin_top, -cabin_width_outer);
   // Between windows
   glTexCoord2f(0.3, 0); glVertex3f(window1_end, body_height, -cabin_width_outer);
   glTexCoord2f(0.5, 0); glVertex3f(window2_start, body_height, -cabin_width_outer);
   glTexCoord2f(0.5, 1); glVertex3f(window2_start, cabin_top, -cabin_width_outer);
   glTexCoord2f(0.3, 1); glVertex3f(window1_end, cabin_top, -cabin_width_outer);
   // Right of second window
   glTexCoord2f(0.8, 0); glVertex3f(window2_end, body_height, -cabin_width_outer);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, body_height, -cabin_width_outer);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, cabin_top, -cabin_width_outer);
   glTexCoord2f(0.8, 1); glVertex3f(window2_end, cabin_top, -cabin_width_outer);
   // Below first window
   glTexCoord2f(0.2, 0); glVertex3f(window1_start, body_height, -cabin_width_outer);
   glTexCoord2f(0.3, 0); glVertex3f(window1_end, body_height, -cabin_width_outer);
   glTexCoord2f(0.3, 0.2); glVertex3f(window1_end, window_bottom, -cabin_width_outer);
   glTexCoord2f(0.2, 0.2); glVertex3f(window1_start, window_bottom, -cabin_width_outer);
   // Below second window
   glTexCoord2f(0.5, 0); glVertex3f(window2_start, body_height, -cabin_width_outer);
   glTexCoord2f(0.8, 0); glVertex3f(window2_end, body_height, -cabin_width_outer);
   glTexCoord2f(0.8, 0.2); glVertex3f(window2_end, window_bottom, -cabin_width_outer);
   glTexCoord2f(0.5, 0.2); glVertex3f(window2_start, window_bottom, -cabin_width_outer);
   // Above first window
   glTexCoord2f(0.2, 0.8); glVertex3f(window1_start, window_top, -cabin_width_outer);
   glTexCoord2f(0.3, 0.8); glVertex3f(window1_end, window_top, -cabin_width_outer);
   glTexCoord2f(0.3, 1); glVertex3f(window1_end, cabin_top, -cabin_width_outer);
   glTexCoord2f(0.2, 1); glVertex3f(window1_start, cabin_top, -cabin_width_outer);
   // Above second window
   glTexCoord2f(0.5, 0.8); glVertex3f(window2_start, window_top, -cabin_width_outer);
   glTexCoord2f(0.8, 0.8); glVertex3f(window2_end, window_top, -cabin_width_outer);
   glTexCoord2f(0.8, 1); glVertex3f(window2_end, cabin_top, -cabin_width_outer);
   glTexCoord2f(0.5, 1); glVertex3f(window2_start, cabin_top, -cabin_width_outer);
   glEnd();
   
   // Right outer wall with window openings (mirror of left) (with texture)
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   // Left of first window
   glTexCoord2f(0, 0); glVertex3f(cabin_start, body_height, cabin_width_outer);
   glTexCoord2f(0, 1); glVertex3f(cabin_start, cabin_top, cabin_width_outer);
   glTexCoord2f(0.2, 1); glVertex3f(window1_start, cabin_top, cabin_width_outer);
   glTexCoord2f(0.2, 0); glVertex3f(window1_start, body_height, cabin_width_outer);
   // Between windows
   glTexCoord2f(0.3, 0); glVertex3f(window1_end, body_height, cabin_width_outer);
   glTexCoord2f(0.3, 1); glVertex3f(window1_end, cabin_top, cabin_width_outer);
   glTexCoord2f(0.5, 1); glVertex3f(window2_start, cabin_top, cabin_width_outer);
   glTexCoord2f(0.5, 0); glVertex3f(window2_start, body_height, cabin_width_outer);
   // Right of second window
   glTexCoord2f(0.8, 0); glVertex3f(window2_end, body_height, cabin_width_outer);
   glTexCoord2f(0.8, 1); glVertex3f(window2_end, cabin_top, cabin_width_outer);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, cabin_top, cabin_width_outer);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, body_height, cabin_width_outer);
   // Below first window
   glTexCoord2f(0.2, 0); glVertex3f(window1_start, body_height, cabin_width_outer);
   glTexCoord2f(0.2, 0.2); glVertex3f(window1_start, window_bottom, cabin_width_outer);
   glTexCoord2f(0.3, 0.2); glVertex3f(window1_end, window_bottom, cabin_width_outer);
   glTexCoord2f(0.3, 0); glVertex3f(window1_end, body_height, cabin_width_outer);
   // Below second window
   glTexCoord2f(0.5, 0); glVertex3f(window2_start, body_height, cabin_width_outer);
   glTexCoord2f(0.5, 0.2); glVertex3f(window2_start, window_bottom, cabin_width_outer);
   glTexCoord2f(0.8, 0.2); glVertex3f(window2_end, window_bottom, cabin_width_outer);
   glTexCoord2f(0.8, 0); glVertex3f(window2_end, body_height, cabin_width_outer);
   // Above first window
   glTexCoord2f(0.2, 0.8); glVertex3f(window1_start, window_top, cabin_width_outer);
   glTexCoord2f(0.2, 1); glVertex3f(window1_start, cabin_top, cabin_width_outer);
   glTexCoord2f(0.3, 1); glVertex3f(window1_end, cabin_top, cabin_width_outer);
   glTexCoord2f(0.3, 0.8); glVertex3f(window1_end, window_top, cabin_width_outer);
   // Above second window
   glTexCoord2f(0.5, 0.8); glVertex3f(window2_start, window_top, cabin_width_outer);
   glTexCoord2f(0.5, 1); glVertex3f(window2_start, cabin_top, cabin_width_outer);
   glTexCoord2f(0.8, 1); glVertex3f(window2_end, cabin_top, cabin_width_outer);
   glTexCoord2f(0.8, 0.8); glVertex3f(window2_end, window_top, cabin_width_outer);
   glEnd();
   
   //  Interior walls (darker color)
   glColor3f(0.4, 0.15, 0.1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dark_interior);
   
   // Interior roof (bottom of roof)
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glVertex3f(cabin_start, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(cabin_start, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(cabin_end, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(cabin_end, cabin_top - wall_thickness, -cabin_width_inner);
   glEnd();
   
   // Front interior wall
   glBegin(GL_QUADS);
   glNormal3f(1, 0, 0);
   glVertex3f(cabin_start + wall_thickness, body_height, -cabin_width_inner);
   glVertex3f(cabin_start + wall_thickness, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(cabin_start + wall_thickness, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(cabin_start + wall_thickness, body_height, cabin_width_inner);
   glEnd();
   
   // Back interior wall
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glVertex3f(cabin_end - wall_thickness, body_height, -cabin_width_inner);
   glVertex3f(cabin_end - wall_thickness, body_height, cabin_width_inner);
   glVertex3f(cabin_end - wall_thickness, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(cabin_end - wall_thickness, cabin_top - wall_thickness, -cabin_width_inner);
   glEnd();
   
   // Left interior wall with window openings (mirrors exterior openings)
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   // Left of first window
   glVertex3f(cabin_start, body_height, -cabin_width_inner);
   glVertex3f(cabin_start, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(window1_start, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(window1_start, body_height, -cabin_width_inner);
   // Between windows
   glVertex3f(window1_end, body_height, -cabin_width_inner);
   glVertex3f(window1_end, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(window2_start, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(window2_start, body_height, -cabin_width_inner);
   // Right of second window
   glVertex3f(window2_end, body_height, -cabin_width_inner);
   glVertex3f(window2_end, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(cabin_end, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(cabin_end, body_height, -cabin_width_inner);
   // Below first window
   glVertex3f(window1_start, body_height, -cabin_width_inner);
   glVertex3f(window1_start, window_bottom, -cabin_width_inner);
   glVertex3f(window1_end, window_bottom, -cabin_width_inner);
   glVertex3f(window1_end, body_height, -cabin_width_inner);
   // Below second window
   glVertex3f(window2_start, body_height, -cabin_width_inner);
   glVertex3f(window2_start, window_bottom, -cabin_width_inner);
   glVertex3f(window2_end, window_bottom, -cabin_width_inner);
   glVertex3f(window2_end, body_height, -cabin_width_inner);
   // Above first window
   glVertex3f(window1_start, window_top, -cabin_width_inner);
   glVertex3f(window1_start, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(window1_end, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(window1_end, window_top, -cabin_width_inner);
   // Above second window
   glVertex3f(window2_start, window_top, -cabin_width_inner);
   glVertex3f(window2_start, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(window2_end, cabin_top - wall_thickness, -cabin_width_inner);
   glVertex3f(window2_end, window_top, -cabin_width_inner);
   glEnd();
   
   // Right interior wall with window openings
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   // Left of first window
   glVertex3f(cabin_start, body_height, cabin_width_inner);
   glVertex3f(window1_start, body_height, cabin_width_inner);
   glVertex3f(window1_start, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(cabin_start, cabin_top - wall_thickness, cabin_width_inner);
   // Between windows
   glVertex3f(window1_end, body_height, cabin_width_inner);
   glVertex3f(window2_start, body_height, cabin_width_inner);
   glVertex3f(window2_start, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(window1_end, cabin_top - wall_thickness, cabin_width_inner);
   // Right of second window
   glVertex3f(window2_end, body_height, cabin_width_inner);
   glVertex3f(cabin_end, body_height, cabin_width_inner);
   glVertex3f(cabin_end, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(window2_end, cabin_top - wall_thickness, cabin_width_inner);
   // Below first window
   glVertex3f(window1_start, body_height, cabin_width_inner);
   glVertex3f(window1_end, body_height, cabin_width_inner);
   glVertex3f(window1_end, window_bottom, cabin_width_inner);
   glVertex3f(window1_start, window_bottom, cabin_width_inner);
   // Below second window
   glVertex3f(window2_start, body_height, cabin_width_inner);
   glVertex3f(window2_end, body_height, cabin_width_inner);
   glVertex3f(window2_end, window_bottom, cabin_width_inner);
   glVertex3f(window2_start, window_bottom, cabin_width_inner);
   // Above first window
   glVertex3f(window1_start, window_top, cabin_width_inner);
   glVertex3f(window1_end, window_top, cabin_width_inner);
   glVertex3f(window1_end, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(window1_start, cabin_top - wall_thickness, cabin_width_inner);
   // Above second window
   glVertex3f(window2_start, window_top, cabin_width_inner);
   glVertex3f(window2_end, window_top, cabin_width_inner);
   glVertex3f(window2_end, cabin_top - wall_thickness, cabin_width_inner);
   glVertex3f(window2_start, cabin_top - wall_thickness, cabin_width_inner);
   glEnd();
   
   //  Half-circle cutouts touching ground as wheels spots
   glColor3f(0.3, 0.1, 0.08);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wheel_well);
   
   double wheel_radius = 0.15;
   double wheel_center_height = 0;  // Center at ground level
   int segments = 20;
   
   // Front left wheel well (half circle on left side)
   double front_wheel_x = -0.35;
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, -1);
   glVertex3f(front_wheel_x, wheel_center_height, -body_width - 0.01);
   for (int i = 0; i <= segments; i++) {
      double angle = 180.0 * i / segments;  // Half circle (0 to 180 degrees)
      double dx = wheel_radius * Cos(angle);
      double dy = wheel_radius * Sin(angle);
      glVertex3f(front_wheel_x + dx, wheel_center_height + dy, -body_width - 0.01);
   }
   glEnd();
   
   // Front right wheel well
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, 1);
   glVertex3f(front_wheel_x, wheel_center_height, body_width + 0.01);
   for (int i = 0; i <= segments; i++) {
      double angle = 180.0 * i / segments;
      double dx = wheel_radius * Cos(angle);
      double dy = wheel_radius * Sin(angle);
      glVertex3f(front_wheel_x + dx, wheel_center_height + dy, body_width + 0.01);
   }
   glEnd();
   
   // Rear left wheel well
   double rear_wheel_x = 0.35;
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, -1);
   glVertex3f(rear_wheel_x, wheel_center_height, -body_width - 0.01);
   for (int i = 0; i <= segments; i++) {
      double angle = 180.0 * i / segments;
      double dx = wheel_radius * Cos(angle);
      double dy = wheel_radius * Sin(angle);
      glVertex3f(rear_wheel_x + dx, wheel_center_height + dy, -body_width - 0.01);
   }
   glEnd();
   
   // Rear right wheel well
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, 1);
   glVertex3f(rear_wheel_x, wheel_center_height, body_width + 0.01);
   for (int i = 0; i <= segments; i++) {
      double angle = 180.0 * i / segments;
      double dx = wheel_radius * Cos(angle);
      double dy = wheel_radius * Sin(angle);
      glVertex3f(rear_wheel_x + dx, wheel_center_height + dy, body_width + 0.01);
   }
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}

double boulderScaleUp = 1.0;  // Boulder scale multiplier

/*
 *  Stone boulder
 *     at (x,y,z)
 *     scale s
 *    
 *  Use 'r'/'R' keys to decrease/increase boulderScaleUp (R for Rock)
 */
static void boulder(double x, double y, double z, double s)
{

   // Base material (matte stone)
   float black[]         = {0.0, 0.0, 0.0, 1.0};
   float stone_base[]    = {0.45f, 0.42f, 0.38f, 1.0f};
   float stone_specular[] = {0.05f, 0.05f, 0.05f, 1.0f};
   float moss_col[]      = {0.25f, 0.35f, 0.25f, 1.0f};

   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * boulderScaleUp, s * boulderScaleUp, s * boulderScaleUp);

   // Parameters controlling shape detail
   const double base_radius = 0.5;          // Approximate overall radius
   const int inc = 10;                      // Angular step (smaller = more tris)
   const double amp = 0.18;                 // Amplitude of radial noise
   const double mossHeight = base_radius * 0.4;  // Height below which moss appears


   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stone_base);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, stone_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 0.08); // Very low shine

   //  Enable textures for boulder
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[8]);  // rock texture
   }

   // Center used for normal derivation (rock sits on ground y≈0)
   const double cy = base_radius; // So bottom (phi=-90) ~ y=0



    // Used AI to generate rock-like noise pattern
   // We'll just define an inline-like macro for noise.
   #define ROCK_NOISE(th,ph) (0.5*Sin(3*(th)) + 0.3*Sin(5*(ph)) + 0.2*Sin(2*(th)+(ph)))

   // Build latitude bands with quad strips
   for (int ph = -90; ph < 90; ph += inc)
   {
      int ph2 = ph + inc;
      if (ph2 > 90) ph2 = 90;
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += inc)
      {
         // Two latitudes: ph2 (top) then ph (bottom) for current segment
         for (int pass = 0; pass < 2; ++pass)
         {
            int p = pass ? ph : ph2; // order: top then bottom to maintain winding (adjust if backface)
            double noise = ROCK_NOISE(th, p);
            // Clamp noise to [-1,1] (safe)
            if (noise > 1.0) noise = 1.0; else if (noise < -1.0) noise = -1.0;
            double radius = base_radius * (1.0 + amp * noise);

            // Spherical direction (degrees based)
            double cp = Cos(p);
            double sp = Sin(p);
            double ct = Cos(th);
            double st = Sin(th);

            double vx = radius * ct * cp;
            double vy = radius * sp + cy;  // shift up so bottom stays near 0
            if (vy < 0.0) vy = 0.0;         // Prevent going underground
            double vz = radius * st * cp;

            // Normal approximated by vector from center (0,cy,0) to vertex then normalized
            double nx = vx;
            double ny = vy - cy;
            double nz = vz;
            double len = sqrt(nx*nx + ny*ny + nz*nz);
            if (len == 0) len = 1; // Safety
            nx /= len; ny /= len; nz /= len;

            // Lighting-based highlight factor (simulate sun from +Y + some X)
            double sunDir[3] = {0.3, 0.9, 0.2};
            double sunLen = sqrt(sunDir[0]*sunDir[0]+sunDir[1]*sunDir[1]+sunDir[2]*sunDir[2]);
            sunDir[0]/=sunLen; sunDir[1]/=sunLen; sunDir[2]/=sunLen;
            double sunDot = nx*sunDir[0] + ny*sunDir[1] + nz*sunDir[2];
            if (sunDot < 0) sunDot = 0;

            // Base color variation from noise (darker crevices)
            double shade = -0.07 * noise; // Invert so positive noise -> slight dark or light; tweak
            double r = stone_base[0] + shade;
            double g = stone_base[1] + shade;
            double b = stone_base[2] + shade;

            // Highlight from sun
            double highlight = 0.15 * pow(sunDot, 1.5);
            r += highlight; g += highlight; b += highlight;

            // Ambient darkening for downward-facing parts
            if (ny < 0.0)
            {
               double f = (-ny); if (f > 1.0) f = 1.0;
               r -= 0.15 * f; g -= 0.15 * f; b -= 0.15 * f;
            }

            // Moss tint near ground & shaded (low height + low sunDot)
            double localHeight = vy; // since ground ~0
            if (localHeight < mossHeight && sunDot < 0.4)
            {
               double mossFactor = (mossHeight - localHeight)/mossHeight; // 0..1
               if (mossFactor > 1.0) mossFactor = 1.0;
               // Blend toward moss color
               r = r*(1.0 - mossFactor) + moss_col[0]*mossFactor;
               g = g*(1.0 - mossFactor) + moss_col[1]*mossFactor;
               b = b*(1.0 - mossFactor) + moss_col[2]*mossFactor;
            }

            // Clamp color
            if (r < 0) r = 0; else if (r > 1) r = 1;
            if (g < 0) g = 0; else if (g > 1) g = 1;
            if (b < 0) b = 0; else if (b > 1) b = 1;

            glColor3f((float)r, (float)g, (float)b);
            glNormal3f((float)nx, (float)ny, (float)nz);
            glTexCoord2f((float)th/360.0, (float)(p+90)/180.0);
            glVertex3f((float)vx, (float)vy, (float)vz);
         }
      }
      glEnd();
   }

   glBegin(GL_TRIANGLES);
   for (int i=0;i<6;i++)
   {
      double th = 30 + i*47; // random spread
      double ph = -20 + (i*29)%60;
      double noise = ROCK_NOISE(th, ph);
      double radius = base_radius * (1.05 + amp * noise * 0.5);
      double ct = Cos(th), st = Sin(th), cp = Cos(ph), sp = Sin(ph);
      double vx = radius*ct*cp;
      double vy = radius*sp + cy; if (vy<0) vy=0; 
      double vz = radius*st*cp;
      double nx = vx, ny = vy-cy, nz = vz; double len = sqrt(nx*nx+ny*ny+nz*nz); if(len==0) len=1; nx/=len; ny/=len; nz/=len;
      glColor3f(stone_base[0]*0.9f, stone_base[1]*0.9f, stone_base[2]*0.9f);
      glNormal3f(nx,ny,nz); glVertex3f(vx,vy,vz);
      glNormal3f(nx,ny,nz); glVertex3f(vx*0.94,vy*0.96,vz*0.94);
      glNormal3f(nx,ny,nz); glVertex3f(vx*0.90,vy*0.92,vz*0.90);
   }
   glEnd();

   #undef ROCK_NOISE

   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

// Grenade
/*
 *  Draw a grenade
 *     at (x,y,z)
 *     scale s
 */

double grenadeScaleUp = 1.0;  // Grenade scale multiplier

static void grenade(double x, double y, double z, double s)
{
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float dark_green[] = {0.2, 0.3, 0.2, 1.0};      // Dark military green
   float metal[] = {0.4, 0.4, 0.4, 1.0};           // Grey metal
   float specular[] = {0.3, 0.3, 0.3, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * grenadeScaleUp, s * grenadeScaleUp, s * grenadeScaleUp);
   
   //  GRENADE BODY (oval sphere - stretched vertically) 
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dark_green);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 0.5);
   
   //  Enable textures for grenade
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[9]);  // grenade texture
   }
   
   int inc = 10;
   double body_radius = 0.15;
   double body_height_scale = 1.4;  // Stretch vertically
   
   glPushMatrix();
   glScaled(1.0, body_height_scale, 1.0);  // Make oval
   
   // Draw body sphere
   for (int ph = -90; ph < 90; ph += inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += 2*inc)
      {
         double x = Sin(th)*Cos(ph);
         double y = Cos(th)*Cos(ph);
         double z = Sin(ph);
         glNormal3d(x, y/body_height_scale, z);  // Adjust normal for stretching
         glTexCoord2d((double)th/360.0, (double)(ph+90)/180.0);
         glVertex3d(body_radius*x, body_radius*y, body_radius*z);
         
         x = Sin(th)*Cos(ph+inc);
         y = Cos(th)*Cos(ph+inc);
         z = Sin(ph+inc);
         glNormal3d(x, y/body_height_scale, z);
         glTexCoord2d((double)th/360.0, (double)(ph+inc+90)/180.0);
         glVertex3d(body_radius*x, body_radius*y, body_radius*z);
      }
      glEnd();
   }
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   
   //  lines
   glColor3f(0.15, 0.2, 0.15);
   float darker_green[] = {0.15, 0.2, 0.15, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, darker_green);
   
   double groove_positions[] = {-0.08, 0.0, 0.08};
   for (int g = 0; g < 3; g++)
   {
      double groove_y = groove_positions[g];
      double groove_r = body_radius * 1.01;  // Slightly larger radius
      
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += 10)
      {
         double nx = Cos(th);
         double nz = Sin(th);
         glNormal3d(nx, 0, nz);
         glVertex3d(groove_r * nx, groove_y - 0.01, groove_r * nz);
         glVertex3d(groove_r * nx, groove_y + 0.01, groove_r * nz);
      }
      glEnd();
   }
   
   // metal Cylinder
   glColor3f(0.4, 0.4, 0.4);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, metal);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   double fuse_radius = 0.04;
   double fuse_height = 0.12;
   double fuse_y = body_radius * body_height_scale;
   
   cylinder(0, fuse_y, 0, fuse_radius, fuse_height, 12);
   
   // Top Cap
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(0, 1, 0);
   glVertex3d(0, fuse_y + fuse_height, 0);
   for (int i = 0; i <= 12; i++)
   {
      double angle = 360.0 * i / 12;
      glVertex3d(fuse_radius * Cos(angle), fuse_y + fuse_height, fuse_radius * Sin(angle));
   }
   glEnd();
   
   // Ring
   glColor3f(0.5, 0.5, 0.5);
   float light_metal[] = {0.5, 0.5, 0.5, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, light_metal);
   
   double ring_radius = 0.03;
   double ring_thickness = 0.008;
   double ring_y = fuse_y + fuse_height * 0.7;
   
   // Ring as torus (just outer and inner circles)
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= 16; i++)
   {
      double angle = 360.0 * i / 16;
      double cx = (fuse_radius + ring_radius) * Cos(angle);
      double cz = (fuse_radius + ring_radius) * Sin(angle);
      
      // Normal points outward from ring center
      double nx = Cos(angle);
      double nz = Sin(angle);
      glNormal3d(nx, 0, nz);
      
      glVertex3d(cx + ring_thickness * nx, ring_y - ring_thickness, cz + ring_thickness * nz);
      glVertex3d(cx + ring_thickness * nx, ring_y + ring_thickness, cz + ring_thickness * nz);
   }
   glEnd();
   
   //  SAFETY LEVER
   glColor3f(0.4, 0.4, 0.4);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, metal);
   
   double lever_width = 0.04;
   double lever_length = body_radius * body_height_scale;
   double lever_thickness = 0.02;
   double lever_x = fuse_radius * 1.2;
   
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(lever_x, fuse_y, lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y, lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y + lever_length, lever_thickness);
   glVertex3f(lever_x, fuse_y + lever_length, lever_thickness);
   
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(lever_x, fuse_y, -lever_thickness);
   glVertex3f(lever_x, fuse_y + lever_length, -lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y + lever_length, -lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y, -lever_thickness);
   
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(lever_x + lever_width, fuse_y, -lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y + lever_length, -lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y + lever_length, lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y, lever_thickness);
   
   // Top face
   glNormal3f(0, 1, 0);
   glVertex3f(lever_x, fuse_y + lever_length, -lever_thickness);
   glVertex3f(lever_x, fuse_y + lever_length, lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y + lever_length, lever_thickness);
   glVertex3f(lever_x + lever_width, fuse_y + lever_length, -lever_thickness);
   glEnd();
   
   glPopMatrix();
}


/*
 *  Health-kit (white suitcase with red cross)
 *     at (x,y,z)
 *     scale s
 *     rotation angle (for spinning effect)
 */

double healthkitScaleUp = 1.0;  // Health-kit scale multiplier
double healthkitRotation = 0.0; // Rotation angle for spinning

static void healthkit(double x, double y, double z, double s)
{

   float black[] = {0.0, 0.0, 0.0, 1.0};
   float white[] = {0.95, 0.95, 0.95, 1.0};        // Bright white
   float red[] = {0.9, 0.1, 0.1, 1.0};             // Red cross
   float metal[] = {0.6, 0.6, 0.6, 1.0};           // Metal latches
   float specular[] = {0.4, 0.4, 0.4, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(healthkitRotation, 0, 1, 0);  // Rotate around Y-axis
   glScaled(s * healthkitScaleUp, s * healthkitScaleUp, s * healthkitScaleUp);
   
   // Suitcase dimensions
   double width = 0.4;
   double height = 0.3;
   double depth = 0.15;
   
   //  MAIN SUITCASE BODY (white box with texture) 
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 0.8);
   
   //  Enable textures for healthkit box
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[15]);  // healthkit texture
   }
   
   glBegin(GL_QUADS);
   
   // Front face (with texture)
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-width, 0, depth);
   glTexCoord2f(1, 0); glVertex3f(width, 0, depth);
   glTexCoord2f(1, 1); glVertex3f(width, height, depth);
   glTexCoord2f(0, 1); glVertex3f(-width, height, depth);
   
   // Back face (with texture)
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(-width, 0, -depth);
   glTexCoord2f(0, 1); glVertex3f(-width, height, -depth);
   glTexCoord2f(1, 1); glVertex3f(width, height, -depth);
   glTexCoord2f(1, 0); glVertex3f(width, 0, -depth);
   
   // Top face (with texture)
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-width, height, -depth);
   glTexCoord2f(0, 1); glVertex3f(-width, height, depth);
   glTexCoord2f(1, 1); glVertex3f(width, height, depth);
   glTexCoord2f(1, 0); glVertex3f(width, height, -depth);
   
   // Bottom face (with texture)
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-width, 0, -depth);
   glTexCoord2f(1, 0); glVertex3f(width, 0, -depth);
   glTexCoord2f(1, 1); glVertex3f(width, 0, depth);
   glTexCoord2f(0, 1); glVertex3f(-width, 0, depth);
   
   // Right face (with texture)
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(width, 0, -depth);
   glTexCoord2f(0, 1); glVertex3f(width, height, -depth);
   glTexCoord2f(1, 1); glVertex3f(width, height, depth);
   glTexCoord2f(1, 0); glVertex3f(width, 0, depth);
   
   // Left face (with texture)
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-width, 0, -depth);
   glTexCoord2f(1, 0); glVertex3f(-width, 0, depth);
   glTexCoord2f(1, 1); glVertex3f(-width, height, depth);
   glTexCoord2f(0, 1); glVertex3f(-width, height, -depth);
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   // === RED CROSS ON FRONT (vertical bar) ===
   glColor3f(0.9, 0.1, 0.1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 0.5);
   
   double cross_width = 0.08;
   double cross_length_v = 0.24;  // Vertical length
   double cross_length_h = 0.24;  // Horizontal length
   double cross_z = depth + 0.002; // Slightly in front
   double center_y = height / 2.0;
   
   // Vertical bar of cross
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glVertex3f(-cross_width/2, center_y - cross_length_v/2, cross_z);
   glVertex3f(cross_width/2, center_y - cross_length_v/2, cross_z);
   glVertex3f(cross_width/2, center_y + cross_length_v/2, cross_z);
   glVertex3f(-cross_width/2, center_y + cross_length_v/2, cross_z);
   glEnd();
   
   // Horizontal bar of cross
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glVertex3f(-cross_length_h/2, center_y - cross_width/2, cross_z);
   glVertex3f(cross_length_h/2, center_y - cross_width/2, cross_z);
   glVertex3f(cross_length_h/2, center_y + cross_width/2, cross_z);
   glVertex3f(-cross_length_h/2, center_y + cross_width/2, cross_z);
   glEnd();
   
   //  RED CROSS ON BACK (reverse normals) 
   cross_z = -depth - 0.002;
   
   // Vertical bar
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glVertex3f(-cross_width/2, center_y - cross_length_v/2, cross_z);
   glVertex3f(-cross_width/2, center_y + cross_length_v/2, cross_z);
   glVertex3f(cross_width/2, center_y + cross_length_v/2, cross_z);
   glVertex3f(cross_width/2, center_y - cross_length_v/2, cross_z);
   glEnd();
   
   // Horizontal bar
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glVertex3f(-cross_length_h/2, center_y - cross_width/2, cross_z);
   glVertex3f(-cross_length_h/2, center_y + cross_width/2, cross_z);
   glVertex3f(cross_length_h/2, center_y + cross_width/2, cross_z);
   glVertex3f(cross_length_h/2, center_y - cross_width/2, cross_z);
   glEnd();
   
   //  METAL HANDLE ON TOP 
   glColor3f(0.6, 0.6, 0.6);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, metal);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   double handle_width = 0.25;
   double handle_height = 0.08;
   double handle_thickness = 0.02;
   double handle_y = height;
   
   // Handle base mounts (two small rectangles)
   double mount_width = 0.04;
   double mount_positions[] = {-handle_width/2, handle_width/2 - mount_width};
   
   for (int i = 0; i < 2; i++)
   {
      double mx = mount_positions[i];
      glBegin(GL_QUADS);
      // Top
      glNormal3f(0, 1, 0);
      glVertex3f(mx, handle_y + 0.01, -handle_thickness/2);
      glVertex3f(mx, handle_y + 0.01, handle_thickness/2);
      glVertex3f(mx + mount_width, handle_y + 0.01, handle_thickness/2);
      glVertex3f(mx + mount_width, handle_y + 0.01, -handle_thickness/2);
      glEnd();
   }
   
   // Handle arc (simplified as rectangle)
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(-handle_width/2, handle_y + 0.01, handle_thickness/2);
   glVertex3f(handle_width/2, handle_y + 0.01, handle_thickness/2);
   glVertex3f(handle_width/2, handle_y + handle_height, handle_thickness/2);
   glVertex3f(-handle_width/2, handle_y + handle_height, handle_thickness/2);
   
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(-handle_width/2, handle_y + 0.01, -handle_thickness/2);
   glVertex3f(-handle_width/2, handle_y + handle_height, -handle_thickness/2);
   glVertex3f(handle_width/2, handle_y + handle_height, -handle_thickness/2);
   glVertex3f(handle_width/2, handle_y + 0.01, -handle_thickness/2);
   
   // Top of handle
   glNormal3f(0, 1, 0);
   glVertex3f(-handle_width/2, handle_y + handle_height, -handle_thickness/2);
   glVertex3f(-handle_width/2, handle_y + handle_height, handle_thickness/2);
   glVertex3f(handle_width/2, handle_y + handle_height, handle_thickness/2);
   glVertex3f(handle_width/2, handle_y + handle_height, -handle_thickness/2);
   glEnd();
   
   //  METAL LATCHES ON FRONT 
   double latch_width = 0.06;
   double latch_height = 0.03;
   double latch_positions_y[] = {height * 0.25, height * 0.75};
   
   for (int i = 0; i < 2; i++)
   {
      double ly = latch_positions_y[i];
      glBegin(GL_QUADS);
      glNormal3f(0, 0, 1);
      glVertex3f(-latch_width/2, ly - latch_height/2, depth + 0.005);
      glVertex3f(latch_width/2, ly - latch_height/2, depth + 0.005);
      glVertex3f(latch_width/2, ly + latch_height/2, depth + 0.005);
      glVertex3f(-latch_width/2, ly + latch_height/2, depth + 0.005);
      glEnd();
   }
   
   glPopMatrix();
}


// Faculty's Code for lighting ball
// changed according to my variable names and Keys

/*
 *  Draw a ball for light position
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x, double y, double z, double r)
{
   int inc = 10;
   
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y+3.0,z);
   glScaled(r,r,r);
   //  White ball with yellow specular
   float yellow[]   = {1.0,1.0,0.0,1.0};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   
   //  Bands of latitude
   for (int ph=-90; ph<90; ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0; th<=360; th+=2*inc)
      {
         double x = Sin(th)*Cos(ph);
         double y = Cos(th)*Cos(ph);
         double z = Sin(ph);
         glNormal3d(x,y,z);
         glVertex3d(x,y,z);
         
         x = Sin(th)*Cos(ph+inc);
         y = Cos(th)*Cos(ph+inc);
         z = Sin(ph+inc);
         glNormal3d(x,y,z);
         glVertex3d(x,y,z);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 *  
 *  SCENE LAYOUT:
 *  - House (back left corner) - Main structure
 *  - Tree (near house, left side) - Natural element
 *  - Human (front center-left) - Practicing archery
 *  - Target (front center) - Archery target for human
 *  - Broken Table (right side) - Abandoned furniture
 *  - Old Car (back right) - Abandoned vehicle
 *  - Ground plane covering entire area
 */
void display()
{
   //  Set sky blue background color
   glClearColor(0.53, 0.81, 0.98, 1.0);  // Sky blue (RGB: 135, 206, 250)
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   //  First-person perspective mode
if (fpp)
{
   // Calculate look-at point based on camera position and angles
   double lx = fpx + Sin(fpAngle)*Cos(fpPitch);
   double ly = fpy + Sin(fpPitch);
   double lz = fpz - Cos(fpAngle)*Cos(fpPitch);
   gluLookAt(fpx, fpy, fpz,  // Eye position
             lx, ly, lz,      // Look-at point
             0, 1, 0);        // Up vector
}
//  Perspective - set eye position
else if (mode)
{
   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);
   gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
}
//  Orthogonal - set world orientation
else
{
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);
}

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2] , 0.1);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
      glDisable(GL_LIGHTING);

   //  Draws scene based on display mode
   if (obj == 0)  // Show all objects - Neat organized layout
   {
//       // Ground plane (covers entire area)
//     //   groundObject(6.0);
//     //   ground(6.0);
      
//     //   // BACK LEFT - House with nearby tree
//     //   house(-1.8, 0, -2.5, 1.0);      // House in back left corner
//     // //   tree(-1.5, 0, -2.0, 1.2);       // Tree near house, slightly larger
      
//     //   // FRONT CENTER-LEFT - Archery practice area
//     //   human(-1.5, 0.6, 1.5, 1.0);     // Human standing, ready to shoot
//     //     tree(-3.5, 0, 1.8, 1.2);        // Tree to the LEFT of human with gap
//     //   target(0.5, 0, 1.5, 1.0);       // Target in front of human
      
//     //   // RIGHT SIDE - Abandoned items area
//     //   broken_table(2.5, 0, 0.5, 1.0); // Broken table on right
//     //   old_car(3.0, 0, -2.5, 1.0);     // Old car in back right corner
//     // boulder(0.8, 0, -0.5, 1.0);  // boulder near center
//     // grenade(-1.0, 0.22, 0.5, 1.0);  // Grenade on ground near human
//     // healthkit(2.0, 0.3, 2.5, 1.0);  // Health-kit on ground, will rotate
// //    crashed_airplane(-1.0, 0.0, -1.0, 1.0);  // Crashed near the house (y=0 so it rests on ground)
//    crashed_front_airplane(-1.0, 0.0, -1.0, 1.0);  // Crashed near the house


 ground(6.0);

// BACK LEFT - House with nearby tree (far back corner)
house(-4.0, 0, -4.0, 1.0);           // House pushed to corner
// tree(-4.5, 0, -2.5, 1.0);            // Tree near house

// CENTER - Crashed airplanes (main focal points, separated)
crashed_airplane(-1.5, 0.2, -1.0, 1.0);      // Full wreckage on left center
crashed_front_airplane(1.5, 0.35, -0.5, 1.0); // Front section on right center

// FRONT LEFT - Human and target (archery practice area)
human(-3.5, 0.5, 3.0, 1.0);          // Human far front left
target(-1.0, 0, 3.5, 1.0);           // Target with good distance from human

// RIGHT SIDE - Scattered items (spread out)
broken_table(3.5, 0, 2.0, 1.0);      // Broken table front right
boulder(4.5, 0, -1.5, 1.0);          // Boulder mid right

// BACK RIGHT - Old car (far back corner)
old_car(4.5, 0, -4.0, 1.0);          // Old car in opposite corner from house

// SCATTERED ITEMS - Health kit and grenade (separated)
grenade(-2.5, 0.15, 1.0, 1.0);       // Grenade between human and center
healthkit(0.5, 0.15, 1.5, 1.0);      // Health-kit in open area

// Additional trees for atmosphere (spread around perimeter)
// tree(-1.5, 0, -4.5, 1.0);            // Tree back center
tree(4.0, 0, 3.5, 1.0);              // Tree front right corner


   }
   else if (obj == 1)  // House only
   {
      house(0, 0, 0, 1.0);
   }
   else if (obj == 2)  // Tree only
   {
      tree(0, 0, 0, 1.0);
   }
   else if (obj == 3)  // Ground only
   {
      ground(6.0);
   }
   else if (obj == 4)  // Target only
   {
      target(0, 0, 0, 1.0); 
   }
   else if (obj == 5)  // Human only
   {
      human(0, 0.6, 0, 1.0); 
   }
   else if (obj == 6)  // Broken table only
   {
      broken_table(0, 0, 0, 1.0);
   }
   else if (obj == 7)  // Old car only
   {
      old_car(0, 0, 0, 1.0);
   }
   else if (obj == 8)  // Boulder only
   {
      boulder(0, 0, 0, 1.0);
   }
    else if (obj == 9)  // Grenade only
    {
        grenade(0, 0.22, 0, 1.0);
    }
    else if (obj == 10)  // Health-kit only
{
   healthkit(0, 0.3, 0, 1.0);
}
    else if (obj == 11)  // Crashed airplane only
    {
        crashed_airplane(0, 0.2, 0, 1.0);
    }
    else if (obj == 12)  // Crashed front airplane only
    {
        crashed_front_airplane(0, 0.35, 0, 1.0);
    }

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
      const double len=2.0;  //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

//    //  Display parameters
//    glWindowPos2i(5,5);
//    Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
//      th,ph,dim,fov,mode?"Perspective":"Orthogonal",light?"On":"Off");


    //  Display parameters
glWindowPos2i(5,5);
Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s FPP=%s Texture=%s",
  th,ph,dim,fov,mode?"Perspective":"Orthogonal",light?"On":"Off",fpp?"On":"Off",ntex?"On":"Off");




   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}




/*
 *  GLUT calls this routine when idle
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   healthkitRotation = fmod(60*t, 360.0); // Rotate health-kit at 60 degrees per second
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   if (fpp)
   {
      // FPP mode: arrow keys move camera
      double moveSpeed = 0.3;  // Movement speed
      
      if (key == GLUT_KEY_UP)
      {
         // Move forward
         fpx += moveSpeed * Sin(fpAngle);
         fpz -= moveSpeed * Cos(fpAngle);
      }
      else if (key == GLUT_KEY_DOWN)
      {
         // Move backward
         fpx -= moveSpeed * Sin(fpAngle);
         fpz += moveSpeed * Cos(fpAngle);
      }
      else if (key == GLUT_KEY_RIGHT)
      {
         // Turn right
         fpAngle += 5;
      }
      else if (key == GLUT_KEY_LEFT)
      {
         // Turn left
         fpAngle -= 5;
      }
      else if (key == GLUT_KEY_PAGE_UP)
      {
         // Look up
         fpPitch += 5;
         if (fpPitch > 89) fpPitch = 89;  // Limit pitch
      }
      else if (key == GLUT_KEY_PAGE_DOWN)
      {
         // Look down
         fpPitch -= 5;
         if (fpPitch < -89) fpPitch = -89;  // Limit pitch
      }
      
      // Keep angle in range
      fpAngle = fmod(fpAngle, 360.0);
   }
   else
   {
      // Normal mode: original behavior
      if (key == GLUT_KEY_RIGHT)
         th += 5;
      else if (key == GLUT_KEY_LEFT)
         th -= 5;
      else if (key == GLUT_KEY_UP)
         ph += 5;
      else if (key == GLUT_KEY_DOWN)
         ph -= 5;
      else if (key == GLUT_KEY_PAGE_DOWN)
         dim += 0.1;
      else if (key == GLUT_KEY_PAGE_UP && dim>1)
         dim -= 0.1;
      else if (key == GLUT_KEY_F1)
         smooth = 1-smooth;
      
      th %= 360;
      ph %= 360;
   }
   
   //  Update projection
   Project(fpp ? fov : (mode?fov:0), asp, dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}
/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
else if (ch == 'f' || ch == 'F'){
   fpp = 1-fpp;
   mode = 0; // Switch to FPP mode
   }
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Toggle textures
   else if (ch == 't' || ch == 'T')
      ntex = 1-ntex;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P'){
      fpp = 0;
      mode = 1-mode;}
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Change field of view angle (perspective) or zoom (orthogonal)
   else if (ch == '-' && dim<20)
      dim += 0.5;  // Zoom out
   else if (ch == '=' && dim>1)  // '+' key (without shift)
      dim -= 0.5;  // Zoom in
   else if (ch == 'z' && ch>1)
      fov--;
   else if (ch == 'Z' && ch<179)
      fov++;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
    else if (ch=='h' && humanScaleUp > 0.1){
      humanScaleUp -= 0.1;
      printf("human size is decreased to %f\n", humanScaleUp);}
   else if (ch=='H' && humanScaleUp < 5.0){
      humanScaleUp += 0.1; 
      printf("human size is increased to %f\n", humanScaleUp);}
   //  Tree scale
   else if (ch=='t' && treeScaleUp > 0.1){
      treeScaleUp -= 0.1;
      printf("tree size is decreased to %f\n", treeScaleUp);}
   else if (ch=='T' && treeScaleUp < 5.0){
      treeScaleUp += 0.1;
      printf("tree size is increased to %f\n", treeScaleUp);}
   //  House scale
   else if (ch=='y' && houseScaleUp > 0.1){
      houseScaleUp -= 0.1;
      printf("house size is decreased to %f\n", houseScaleUp);}
   else if (ch=='Y' && houseScaleUp < 5.0){
      houseScaleUp += 0.1;
      printf("house size is increased to %f\n", houseScaleUp);}
   //  Car scale
   else if (ch=='c' && carScaleUp > 0.1){
      carScaleUp -= 0.1;
      printf("car size is decreased to %f\n", carScaleUp);}
   else if (ch=='C' && carScaleUp < 5.0){
      carScaleUp += 0.1;
      printf("car size is increased to %f\n", carScaleUp);}
//    //  Table scale
   else if (ch=='b' && tableScaleUp > 0.1){
      tableScaleUp -= 0.1;
      printf("table size is decreased to %f\n", tableScaleUp);}
   else if (ch=='B' && tableScaleUp < 5.0){
      tableScaleUp += 0.1;
      printf("table size is increased to %f\n", tableScaleUp);}
   //  Target scale
   else if (ch=='g' && targetScaleUp > 0.1){
      targetScaleUp -= 0.1;
      printf("target size is decreased to %f\n", targetScaleUp);}
   else if (ch=='G' && targetScaleUp < 5.0){
      targetScaleUp += 0.1;
      printf("target size is increased to %f\n", targetScaleUp);}
   //  Rock scale
    else if (ch=='r' && boulderScaleUp > 0.1){
        boulderScaleUp -= 0.1;
        printf("boulder size is decreased to %f\n", boulderScaleUp);}
    else if (ch=='R' && boulderScaleUp < 5.0){
        boulderScaleUp += 0.1;
        printf("boulder size is increased to %f\n", boulderScaleUp);}
    //  Grenade scale
    else if (ch=='j' && grenadeScaleUp > 0.1){
        grenadeScaleUp -= 0.1;
        printf("grenade size is decreased to %f\n", grenadeScaleUp);}
    else if (ch=='J' && grenadeScaleUp < 5.0){
        grenadeScaleUp += 0.1;
        printf("grenade size is increased to %f\n", grenadeScaleUp);}
        // Health-kit scale (use 'k'/'K' for kit)
else if (ch=='k' && healthkitScaleUp > 0.1){
   healthkitScaleUp -= 0.1;
   printf("health-kit size is decreased to %f\n", healthkitScaleUp);}
else if (ch=='K' && healthkitScaleUp < 5.0){
   healthkitScaleUp += 0.1;
   printf("health-kit size is increased to %f\n", healthkitScaleUp);}
    //  Car scale
    else if (ch=='w' && airplaneScaleUp > 0.1){
        airplaneScaleUp -= 0.1;
        printf("airplane size is decreased to %f\n", airplaneScaleUp);}
    else if (ch=='W' && airplaneScaleUp < 5.0){
        airplaneScaleUp += 0.1;
        printf("airplane size is increased to %f\n", airplaneScaleUp);}
        //  Crashed front airplane scale
    else if (ch=='q' && airplaneFrontScaleUp > 0.1){
        airplaneFrontScaleUp -= 0.1;
        printf("front airplane size is decreased to %f\n", airplaneFrontScaleUp);}
    else if (ch=='Q' && airplaneFrontScaleUp < 5.0){
        airplaneFrontScaleUp += 0.1;
        printf("front airplane size is increased to %f\n", airplaneFrontScaleUp);}
   //  Switch display mode
   else if (ch == 'o')
      obj = (obj+1)%13;
   else if (ch == 'O')
      obj = (obj+3)%13;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project(mode?fov:0,asp,dim);
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(800,800);
   glutCreateWindow("Meghasrivardhan - HW6");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   
   //  Load textures
   texture[0] = LoadTexBMP("textures/rock_ground.bmp");  // Ground plane
   texture[1] = LoadTexBMP("textures/wall.bmp");         // House walls
   texture[2] = LoadTexBMP("textures/roof.bmp");         // House roof
   texture[3] = LoadTexBMP("textures/bark.bmp");         // Tree trunk
   texture[4] = LoadTexBMP("textures/leaves-1.bmp");       // Tree foliage
   texture[5] = LoadTexBMP("textures/table_plank.bmp");  // Table top
   texture[6] = LoadTexBMP("textures/table_legs.bmp");   // Table legs
   texture[7] = LoadTexBMP("textures/rusty_car.bmp");    // Old car
   texture[8] = LoadTexBMP("textures/rock.bmp");         // Boulder
   texture[9] = LoadTexBMP("textures/grenade.bmp");      // Grenade
   texture[10] = LoadTexBMP("textures/rusty_plane.bmp"); // Crashed airplane
   texture[11] = LoadTexBMP("textures/rusty_plane.bmp"); // Front plane section
   texture[12] = LoadTexBMP("textures/shirt-1.bmp");     // Human shirt
   texture[13] = LoadTexBMP("textures/pant.bmp");        // Human pants
   texture[14] = LoadTexBMP("textures/shoe.bmp");        // Human shoes
   texture[15] = LoadTexBMP("textures/healthkit.bmp");   // Health kit box
   
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}