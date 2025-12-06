#include "CSCIx229.h"
#include "Grenade.h"
#include "Primitives.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// Grenade scale multiplier
double grenadeScaleUp = 1.0;

/*
 *  Draw a grenade
 *     at (x,y,z)
 *     scale s
 */
void grenade(double x, double y, double z, double s)
{
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float dark_green[] = {0.2, 0.3, 0.2, 1.0};
   float metal[] = {0.4, 0.4, 0.4, 1.0};
   float specular[] = {0.3, 0.3, 0.3, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * grenadeScaleUp, s * grenadeScaleUp, s * grenadeScaleUp);
   
   //  GRENADE BODY (oval sphere - stretched vertically) 
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dark_green);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 0.5);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[9]);
   }
   
   int inc = 10;
   double body_radius = 0.15;
   double body_height_scale = 1.4;
   
   glPushMatrix();
   glScaled(1.0, body_height_scale, 1.0);
   
   // Draw body sphere
   for (int ph = -90; ph < 90; ph += inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += 2*inc)
      {
         double x = Sin(th)*Cos(ph);
         double y = Cos(th)*Cos(ph);
         double z = Sin(ph);
         glNormal3d(x, y/body_height_scale, z);
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
   
   //  Grooves/lines
   glColor3f(0.15, 0.2, 0.15);
   float darker_green[] = {0.15, 0.2, 0.15, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, darker_green);
   
   double groove_positions[] = {-0.08, 0.0, 0.08};
   for (int g = 0; g < 3; g++)
   {
      double groove_y = groove_positions[g];
      double groove_r = body_radius * 1.01;
      
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
   
   // Metal fuse cylinder
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
   
   // Ring as torus
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= 16; i++)
   {
      double angle = 360.0 * i / 16;
      double cx = (fuse_radius + ring_radius) * Cos(angle);
      double cz = (fuse_radius + ring_radius) * Sin(angle);
      
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
