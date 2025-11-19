#include "CSCIx229.h"

/*
 *  Crashed airplane wreckage on ground
 *     at (x,y,z)
 *     scale s
 */

double airplaneScaleUp = 1.5;
double airplaneGroundAdjust = -0.0;
double airplaneRollDeg = 5.0;
double airplanePitchDeg = -60.0;

static void crashed_airplane(double x, double y, double z, double s)
{
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float plane_body[] = {0.65, 0.65, 0.70, 1.0};
   float damaged[] = {0.35, 0.30, 0.25, 1.0};
   float wing[] = {0.55, 0.55, 0.60, 1.0};
   float interior[] = {0.25, 0.20, 0.15, 1.0};
   float rust[] = {0.55, 0.27, 0.07, 1.0};
   float specular[] = {0.2, 0.2, 0.2, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * airplaneScaleUp, s * airplaneScaleUp, s * airplaneScaleUp);
   
   glRotated(airplaneRollDeg, 0, 0, 1);
   glRotated(airplanePitchDeg, 1, 0, 0);
   glTranslated(0, airplaneGroundAdjust, 0);
   
   double plane_body_length = 1.8;
   double plane_body_radius = 0.25;
   double break_point = 0.55;
   int segments = 20;
   
   //   FRONT FUSELAGE SECTION  
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, plane_body);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0 * 0.3);
   
   //  Enable textures for plane
   extern int ntex;
   extern unsigned int texture[];
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[10]);  // rusty plane texture
   }
   
   // Front fuselage with tapered nose - NORMALS
   for (int i = 0; i < (int)(break_point * segments); i++)
   {
      double t1 = (double)i / segments;
      double t2 = (double)(i+1) / segments;
      
      double r1 = plane_body_radius * (0.05 + 0.95 * pow(t1, 1.5));
      double r2 = plane_body_radius * (0.05 + 0.95 * pow(t2, 1.5));
      
      double x1 = t1 * plane_body_length;
      double x2 = t2 * plane_body_length;
      
      // Calculating taper angle
      double dx = x2 - x1;
      double dr = r2 - r1;
      double slope_angle = atan2(dr, dx);
      
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= segments; j++)
      {
         double angle = 360.0 * j / segments;
         double ca = Cos(angle);
         double sa = Sin(angle);
         
         double weather = 1.0 - 0.1 * Sin(angle * 3);
         glColor3f(0.65 * weather, 0.65 * weather, 0.70 * weather);
         
         //  : For cylinder along X-axis with vertices at (x, r*cos(a), r*sin(a))
         // Normal must point radially: (0, cos(a), sin(a)) for straight cylinder
         // For tapered cylinder: normal tilts based on slope
         double nx = sin(slope_angle);    // X component from taper
         double ny = ca * cos(slope_angle); // Y component (radial)
         double nz = sa * cos(slope_angle); // Z component (radial)
         
         double len = sqrt(nx*nx + ny*ny + nz*nz);
         if (len > 0.001) {
            nx /= len; ny /= len; nz /= len;
         }
         
         glNormal3d(nx, ny, nz);
         glTexCoord2d((double)j/segments, t1);
         glVertex3d(x1, r1 * ca, r1 * sa);
         glTexCoord2d((double)j/segments, t2);
         glVertex3d(x2, r2 * ca, r2 * sa);
      }
      glEnd();
   }
   
   // Nose cap
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(-1, 0, 0);
   glVertex3d(0, 0, 0);
   
   double nose_cap_r = plane_body_radius * 0.05;
   for (int j = 0; j <= segments; j++)
   {
      double angle = 360.0 * j / segments;
      glVertex3d(0.02, nose_cap_r * Cos(angle), nose_cap_r * Sin(angle));
   }
   glEnd();
   
   //   DAMAGE AT BREAK  
   glColor3f(0.35, 0.30, 0.25);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, damaged);
   
   double break_x = break_point * plane_body_length;
   double break_r = plane_body_radius * (0.05 + 0.95 * pow(break_point, 1.5));
   
   glBegin(GL_TRIANGLES);
   for (int i = 0; i < 16; i++)
   {
      double a1 = 360.0 * i / 16;
      double a2 = 360.0 * (i + 1) / 16;
      
      double damage1 = 0.6 + 0.4 * Sin(5 * a1);
      double damage2 = 0.6 + 0.4 * Sin(5 * a2);
      
      double y1 = break_r * damage1 * Cos(a1);
      double z1 = break_r * damage1 * Sin(a1);
      double y2 = break_r * damage2 * Cos(a2);
      double z2 = break_r * damage2 * Sin(a2);
      
      // Edge vectors
      double e1y = y1 - 0;
      double e1z = z1 - 0;
      double e2y = y2 - 0;
      double e2z = z2 - 0;
      
      // Cross product (edges in YZ plane, normal along X)
      double nx = e1y * e2z - e1z * e2y;
      double ny = 0;
      double nz = 0;
      
      double len = sqrt(nx*nx + ny*ny + nz*nz);
      if (len > 0.001) {
         nx /= len;
      } else {
         nx = 1;
      }
      
      glNormal3d(nx, 0, 0);
      glVertex3d(break_x, 0, 0);
      glVertex3d(break_x, y1, z1);
      glVertex3d(break_x, y2, z2);
   }
   glEnd();
   
   //   INTERIOR BULKHEAD  
   glColor3f(0.25, 0.20, 0.15);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, interior);
   
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(-1, 0, 0);
   glVertex3d(break_x - 0.01, 0, 0);
   for (int j = 0; j <= segments; j++)
   {
      double angle = 360.0 * j / segments;
      double inner_r = break_r * 0.7;
      glVertex3d(break_x - 0.01, inner_r * Cos(angle), inner_r * Sin(angle));
   }
   glEnd();
   
   // Interior structure rings
   for (int ring = 0; ring < 3; ring++)
   {
      double ring_x = 0.2 + ring * 0.15;
      double ring_r = plane_body_radius * (0.05 + 0.95 * pow(ring_x / plane_body_length, 1.5)) * 0.75;
      
      glBegin(GL_LINE_LOOP);
      for (int j = 0; j < segments; j++)
      {
         double angle = 360.0 * j / segments;
         glVertex3d(ring_x, ring_r * Cos(angle), ring_r * Sin(angle));
      }
      glEnd();
   }
   
   //   REAR FUSELAGE SECTION  
   glPushMatrix();
   glTranslated(break_x + 0.55, -0.05, 0.25);
   glRotated(-12, 0, 1, 0);
   glRotated(-8, 1, 0, 0);
   glRotated(-40, 0, 0, 1);
   glScaled(1.0, 0.85, 1.05);

   double rear_length = (1.0 - break_point) * plane_body_length * 0.95;
   int rear_slices = 9;

   // Torn front edge
   glColor3f(0.32, 0.28, 0.24);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, damaged);
   
   glBegin(GL_TRIANGLES);
   for (int i = 0; i < segments; ++i)
   {
      double a1 = 360.0 * i / segments;
      double a2 = 360.0 * (i+1) / segments;
      double jag1 = 0.78 + 0.25 * Sin(5*a1) + 0.08 * Cos(3*a1);
      double jag2 = 0.78 + 0.25 * Sin(5*a2) + 0.08 * Cos(3*a2);
      double rj1 = break_r * jag1;
      double rj2 = break_r * jag2;
      double y1 = rj1 * Cos(a1);
      double z1 = rj1 * Sin(a1);
      double y2 = rj2 * Cos(a2);
      double z2 = rj2 * Sin(a2);
      
      double fold = -0.07 - 0.03 * Sin(2*a1);
      
      // Edges from fold point to ring points
      double e1y = y1 - 0.55*y1;
      double e1z = z1 - 0.55*z1;
      double e2y = y2 - 0.55*y1;
      double e2z = z2 - 0.55*z1;
      
      // Cross product
      double nx = -(e1y * e2z - e1z * e2y);
      
      if (nx > 0) nx = -1; else if (nx < 0) nx = 1; else nx = -1;
      
      glNormal3d(nx, 0, 0);
      glVertex3d(0, y1, z1);
      glVertex3d(0, y2, z2);
      glVertex3d(fold, 0.55*y1, 0.55*z1);
   }
   glEnd();

   // Rear cylinder body 
   for (int i = 0; i < rear_slices; ++i)
   {
      double t1 = (double)i / rear_slices;
      double t2 = (double)(i+1) / rear_slices;
      double r1 = break_r * (0.95 - 0.45 * t1);
      double r2 = break_r * (0.95 - 0.45 * t2);
      double x1 = 0.02 + t1 * rear_length;
      double x2 = 0.02 + t2 * rear_length;
      
      double scorch1 = 0.55 - 0.30 * (1.0 - t1) + 0.05*Sin(11*t1);
      double scorch2 = 0.55 - 0.30 * (1.0 - t2) + 0.05*Sin(11*t2);
      
      // Calculating taper angle
      double dx = x2 - x1;
      double dr = r2 - r1;
      double taper_angle = atan2(dr, dx);
      
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= segments; ++j)
      {
         double ang = 360.0 * j / segments;
         double ca = Cos(ang);
         double sa = Sin(ang);
         
         double dent = 1.0 - 0.06*Sin(3*ang + 210*t1) - 0.04*Sin(7*ang + 150*t1);
         double rr1 = r1 * dent;
         double rr2 = r2 * dent;
         
         glColor3f(scorch1, scorch1, scorch1 + 0.02);
         
         //  Normal for tapered cylinder along X-axis
         double nx = sin(taper_angle);
         double ny = ca * cos(taper_angle);
         double nz = sa * cos(taper_angle);
         
         double len = sqrt(nx*nx + ny*ny + nz*nz);
         if (len > 0.001) {
            nx /= len; ny /= len; nz /= len;
         }
         
         glNormal3d(nx, ny, nz);
         glVertex3d(x1, rr1 * ca, rr1 * sa);
         
         glColor3f(scorch2, scorch2, scorch2 + 0.02);
         glVertex3d(x2, rr2 * ca, rr2 * sa);
      }
      glEnd();
   }

   // Interior ribs
   glColor3f(0.22,0.18,0.14);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, interior);
   for (int r = 0; r < 2; ++r)
   {
      double rx = 0.02 + r * 0.18;
      double ring_r = break_r * (0.70 - 0.15 * r);
      glBegin(GL_LINE_LOOP);
      for (int j=0; j<segments; ++j)
      {
         double ang = 360.0 * j / segments;
         glVertex3d(rx, ring_r * Cos(ang), ring_r * Sin(ang));
      }
      glEnd();
   }

   // Rust patches 
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rust);
   for (int sc=0; sc<4; ++sc)
   {
      double sx = 0.15 + rear_length * 0.6 * sc / 4.0;
      double angCenter = 25 + 70*sc;
      double base_r = break_r * (0.85 - 0.40 * (sx/rear_length));
      
      // Normal points radially outward from X-axis
      double patch_nx = 0;
      double patch_ny = Cos(angCenter);
      double patch_nz = Sin(angCenter);
      
      glBegin(GL_TRIANGLE_FAN);
      glNormal3d(patch_nx, patch_ny, patch_nz);
      glColor3f(0.55,0.30,0.10);
      glVertex3d(sx, base_r * Cos(angCenter), base_r * Sin(angCenter));
      
      for (int k=0; k<=6; ++k)
      {
         double da = 40.0 * (k/6.0 - 0.5);
         double rr = base_r + 0.04*Sin(3*da) + 0.02*Cos(5*da);
         glColor3f(0.50 + 0.05*Sin(k), 0.28, 0.09);
         glVertex3d(sx + 0.03*Cos(da), rr * Cos(angCenter + da), rr * Sin(angCenter + da));
      }
      glEnd();
   }

   glPopMatrix();
   
   //   LEFT WING  
   glColor3f(0.55, 0.55, 0.60);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wing);
   
   double wing_x = 0.6;
   double wing_span = 1.2;
   double wing_chord = 0.35;
   double wing_thick = 0.04;
   
   glPushMatrix();
   glTranslated(wing_x, 0, 0);
   glRotated(-30, 1, 0, 0);
   
   // Wing upper surface
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= 10; i++)
   {
      double s = (double)i / 10.0 * wing_span;
      double chord_s = wing_chord * (1.0 - 0.4 * s / wing_span);
      
      double damage_fade = (i > 6) ? 0.5 + 0.5 * Sin(s * 5) : 1.0;
      glColor3f(0.55 * damage_fade, 0.55 * damage_fade, 0.60 * damage_fade);
      
      // Wing extends in Y direction, normal points in Z
      glNormal3d(0, 0, 1);
      glVertex3d(-0.2 * chord_s, s, wing_thick);
      glVertex3d(0.8 * chord_s, s, wing_thick);
   }
   glEnd();
   
   // Wing lower surface
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= 10; i++)
   {
      double s = (double)i / 10.0 * wing_span;
      double chord_s = wing_chord * (1.0 - 0.4 * s / wing_span);
      
      glNormal3d(0, 0, -1);
      glVertex3d(-0.2 * chord_s, s, -wing_thick);
      glVertex3d(0.8 * chord_s, s, -wing_thick);
   }
   glEnd();
   
   // Leading edge
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= 10; i++)
   {
      double s = (double)i / 10.0 * wing_span;
      double chord_s = wing_chord * (1.0 - 0.4 * s / wing_span);
      
      double nx = -0.707;
      double ny = 0;
      double nz = 0.707;
      glNormal3d(nx, ny, nz);
      glVertex3d(-0.2 * chord_s, s, wing_thick);
      
      glNormal3d(nx, ny, -nz);
      glVertex3d(-0.2 * chord_s, s, -wing_thick);
   }
   glEnd();
   
   // Trailing edge
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= 10; i++)
   {
      double s = (double)i / 10.0 * wing_span;
      double chord_s = wing_chord * (1.0 - 0.4 * s / wing_span);
      
      double nx = 0.707;
      double ny = 0;
      double nz = 0.707;
      glNormal3d(nx, ny, nz);
      glVertex3d(0.8 * chord_s, s, wing_thick);
      
      glNormal3d(nx, ny, -nz);
      glVertex3d(0.8 * chord_s, s, -wing_thick);
   }
   glEnd();
   
   // Torn wing tip
   glColor3f(0.35, 0.30, 0.25);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, damaged);
   
   double tip_s = wing_span * 0.8;
   double chord_tip = wing_chord * 0.7;
   
   glBegin(GL_TRIANGLES);
   for (int i = 0; i < 8; i++)
   {
      double angle = 45.0 * i;
      double tear_x = chord_tip * 0.3 * Cos(angle);
      double tear_z = wing_thick * 2 * Sin(angle);
      
      double v1x = -0.1 * chord_tip;
      double v2x = 0.5 * chord_tip;
      double v3x = tear_x;
      double v1z = 0;
      double v2z = wing_thick;
      double v3z = tear_z;
      
      // Edges in XZ plane, normal along Y
      double e1x = v2x - v1x;
      double e1z = v2z - v1z;
      double e2x = v3x - v1x;
      double e2z = v3z - v1z;
      
      double ny = e1x * e2z - e1z * e2x;
      
      if (ny == 0) ny = 1;
      else ny = (ny > 0) ? 1 : -1;
      
      glNormal3d(0, ny, 0);
      glVertex3d(v1x, tip_s, v1z);
      glVertex3d(v2x, tip_s, v2z);
      glVertex3d(v3x, tip_s + 0.1, v3z);
   }
   glEnd();
   
   glPopMatrix();
   
   // //   TAIL SECTION  
   // glColor3f(0.55, 0.55, 0.60);
   // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wing);
   
   // double tail_x = plane_body_length * 0.85;
   // double tail_height = 0.5;
   // double tail_chord = 0.25;
   
   // glBegin(GL_QUADS);
   
   // // Right side (positive Y)
   // glNormal3d(0, 1, 0);
   // glVertex3d(tail_x, 0, 0);
   // glVertex3d(tail_x - tail_chord, 0, 0);
   // glVertex3d(tail_x - tail_chord * 0.6, 0, tail_height * 0.4);
   // glVertex3d(tail_x, 0, tail_height * 0.5);
   
   // // Left side (negative Y)
   // glNormal3d(0, -1, 0);
   // glVertex3d(tail_x, 0, 0);
   // glVertex3d(tail_x, 0, tail_height * 0.5);
   // glVertex3d(tail_x - tail_chord * 0.6, 0, tail_height * 0.4);
   // glVertex3d(tail_x - tail_chord, 0, 0);
   // glEnd();
   
   // //   RUST PATCHES ON FUSELAGE  
   // glColor3f(0.55, 0.27, 0.07);
   // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rust);
   
   // for (int p = 0; p < 8; p++)
   // {
   //    double patch_x = 0.2 + p * 0.1;
   //    double patch_angle = 45 * p;
   //    double patch_r = plane_body_radius * (0.05 + 0.95 * pow(patch_x / plane_body_length, 1.5));
      
   //    // Normal points radially from X-axis
   //    double patch_nx = 0;
   //    double patch_ny = Cos(patch_angle);
   //    double patch_nz = Sin(patch_angle);
      
   //    glBegin(GL_TRIANGLE_FAN);
   //    glNormal3d(patch_nx, patch_ny, patch_nz);
   //    glVertex3d(patch_x, patch_r * Cos(patch_angle), patch_r * Sin(patch_angle));
      
   //    for (int j = 0; j <= 8; j++)
   //    {
   //       double a = 360.0 * j / 8;
   //       double offset = 0.05 + 0.02 * Sin(a * 2);
   //       glVertex3d(patch_x + offset * Cos(a) * 0.5,
   //                  patch_r * Cos(patch_angle) + offset * Cos(a),
   //                  patch_r * Sin(patch_angle) + offset * Sin(a));
   //    }
   //    glEnd();
   // }
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}