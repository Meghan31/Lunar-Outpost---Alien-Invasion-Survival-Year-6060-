#include "CSCIx229.h"
#include "HealthKit.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// Health-kit scale multiplier
double healthkitScaleUp = 1.0;

// Rotation angle for spinning
double healthkitRotation = 0.0;

/*
 *  Health-kit (white suitcase with red cross)
 *     at (x,y,z)
 *     scale s
 */
void healthkit(double x, double y, double z, double s)
{
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float white[] = {0.95, 0.95, 0.95, 1.0};
   float red[] = {0.9, 0.1, 0.1, 1.0};
   float metal[] = {0.6, 0.6, 0.6, 1.0};
   float specular[] = {0.4, 0.4, 0.4, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(healthkitRotation, 0, 1, 0);
   glScaled(s * healthkitScaleUp, s * healthkitScaleUp, s * healthkitScaleUp);
   
   // Suitcase dimensions
   double width = 0.4;
   double height = 0.3;
   double depth = 0.15;
   
   //  MAIN SUITCASE BODY (white box with texture) 
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 0.8);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[15]);
   }
   
   glBegin(GL_QUADS);
   
   // Front face
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-width, 0, depth);
   glTexCoord2f(1, 0); glVertex3f(width, 0, depth);
   glTexCoord2f(1, 1); glVertex3f(width, height, depth);
   glTexCoord2f(0, 1); glVertex3f(-width, height, depth);
   
   // Back face
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(-width, 0, -depth);
   glTexCoord2f(0, 1); glVertex3f(-width, height, -depth);
   glTexCoord2f(1, 1); glVertex3f(width, height, -depth);
   glTexCoord2f(1, 0); glVertex3f(width, 0, -depth);
   
   // Top face
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-width, height, -depth);
   glTexCoord2f(0, 1); glVertex3f(-width, height, depth);
   glTexCoord2f(1, 1); glVertex3f(width, height, depth);
   glTexCoord2f(1, 0); glVertex3f(width, height, -depth);
   
   // Bottom face
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-width, 0, -depth);
   glTexCoord2f(1, 0); glVertex3f(width, 0, -depth);
   glTexCoord2f(1, 1); glVertex3f(width, 0, depth);
   glTexCoord2f(0, 1); glVertex3f(-width, 0, depth);
   
   // Right face
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(width, 0, -depth);
   glTexCoord2f(0, 1); glVertex3f(width, height, -depth);
   glTexCoord2f(1, 1); glVertex3f(width, height, depth);
   glTexCoord2f(1, 0); glVertex3f(width, 0, depth);
   
   // Left face
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
   double cross_length_v = 0.24;
   double cross_length_h = 0.24;
   double cross_z = depth + 0.002;
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
   
   //  RED CROSS ON BACK
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
   
   // Handle base mounts
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
   
   // Handle arc
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
