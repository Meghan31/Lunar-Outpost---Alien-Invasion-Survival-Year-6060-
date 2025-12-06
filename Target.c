#include "CSCIx229.h"
#include "Target.h"
#include "Primitives.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// Target scale multiplier
double targetScaleUp = 1.0;

/*
 *  Archery target
 *     at (x,y,z)
 *     scale s
 */
void target(double x, double y, double z, double s)
{
   //  Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float specular[] = {0.3, 0.3, 0.3, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * targetScaleUp, s * targetScaleUp, s * targetScaleUp);
   
   //  POLE STAND (with rusty_car texture) 
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   //  Enable textures for pole
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[7]);
   }
   
   double pole_radius = 0.05;
   double pole_height = 1.0;
   cylinder(0, 0, 0, pole_radius, pole_height, 12);
   
   glDisable(GL_TEXTURE_2D);

   //  TARGET FACE (at top of pole)
   double target_y = pole_height;
   double target_radius = 0.5;
   double target_thickness = 0.05;
   int segments = 32;
   
   //  Target backing (thin cylinder for thickness with table_plank texture)
   glColor3f(1, 1, 1);
   float light_grey[] = {0.9, 0.9, 0.9, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, light_grey);
   
   //  Enable textures for target board
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);
   }
   
   //  Drawing thin cylinder for target body
   glPushMatrix();
   glTranslated(0, target_y, 0);
   glRotated(90, 1, 0, 0);
   cylinder(0, 0, 0, target_radius, target_thickness, segments);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   
   //  COLORED RINGS (front face - vertical, attached to backing) 
   double ring_z = -target_thickness + 0.025;
   
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
      
      //  Normal points forward in -Z direction
      glNormal3f(0, 0, -1);
      
      //  Drawing ring as quad strip in XY plane (vertical)
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= segments; i++)
      {
         double angle = 360.0 * i / segments;
         //  Outer vertex
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
   glVertex3f(0, target_y, ring_z);
   for (int i = 0; i <= segments; i++)
   {
      double angle = 360.0 * i / segments;
      glVertex3f(radii[4] * Cos(angle), target_y + radii[4] * Sin(angle), ring_z);
   }
   glEnd();
   
   glPopMatrix();
}
