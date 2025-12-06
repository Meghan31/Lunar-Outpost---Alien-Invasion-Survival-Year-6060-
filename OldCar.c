#include "CSCIx229.h"
#include "OldCar.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// Car scale multiplier
double carScaleUp = 2.3;

/*
 *  Old abandoned car 
 *     at (x,y,z)
 *     scale s
 */
void old_car(double x, double y, double z, double s)
{
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float car_body[] = {0.8, 0.3, 0.2, 1.0};
   float dark_interior[] = {0.4, 0.15, 0.1, 1.0};
   float wheel_well[] = {0.3, 0.1, 0.08, 1.0};
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
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, car_body);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[7]);
   }
   
   // Main body box
   glBegin(GL_QUADS);
   
   // Top
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-hood_length, body_height, -body_width);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, body_height, -body_width);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, body_height, body_width);
   glTexCoord2f(0, 1); glVertex3f(-hood_length, body_height, body_width);
   
   // Bottom
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
   
   //  Hollow cabin structure
   double cabin_width_outer = body_width * 0.8;
   double cabin_width_inner = cabin_width_outer - wall_thickness;
   
   // Cabin roof (top)
   glBegin(GL_QUADS);
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(cabin_start, cabin_top, -cabin_width_outer);
   glTexCoord2f(1, 0); glVertex3f(cabin_end, cabin_top, -cabin_width_outer);
   glTexCoord2f(1, 1); glVertex3f(cabin_end, cabin_top, cabin_width_outer);
   glTexCoord2f(0, 1); glVertex3f(cabin_start, cabin_top, cabin_width_outer);
   glEnd();
   
   // Front wall (exterior)
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(cabin_start, body_height, -cabin_width_outer);
   glTexCoord2f(1, 0); glVertex3f(cabin_start, body_height, cabin_width_outer);
   glTexCoord2f(1, 1); glVertex3f(cabin_start, cabin_top, cabin_width_outer);
   glTexCoord2f(0, 1); glVertex3f(cabin_start, cabin_top, -cabin_width_outer);
   glEnd();
   
   // Back wall (exterior)
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
   
   // Left outer wall with window openings
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
   
   // Right outer wall with window openings
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
   
   // Interior roof
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
   
   // Left interior wall with window openings
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
   
   //  Half-circle cutouts as wheel spots
   glColor3f(0.3, 0.1, 0.08);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wheel_well);
   
   double wheel_radius = 0.15;
   double wheel_center_height = 0;
   int segments = 20;
   
   // Front left wheel well
   double front_wheel_x = -0.35;
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, -1);
   glVertex3f(front_wheel_x, wheel_center_height, -body_width - 0.01);
   for (int i = 0; i <= segments; i++) {
      double angle = 180.0 * i / segments;
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
