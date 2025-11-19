#include "CSCIx229.h"

/*
 *  Crashed airplane (front section)
 *     at (x,y,z)
 *     scale s
 */

double airplaneFrontScaleUp = 1.0;  // Airplane scale multiplier

static void crashed_front_airplane(double x, double y, double z, double s)
{
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float plane_body_grey[] = {0.7, 0.7, 0.75, 1.0};     // Light grey metal
   float dark_grey[] = {0.4, 0.4, 0.45, 1.0};         // Darker grey for damage
   float interior_dark[] = {0.3, 0.3, 0.35, 1.0};     // Dark interior
   float specular[] = {0.3, 0.3, 0.3, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   // Rotate to lay flat on ground - rotate 90 degrees around Z axis
   glRotated(90, 0, 0, 1);  // Lay it flat on XZ plane
   glRotated(10, 0, 1, 0);   // Slight yaw for realism
   glScaled(s * airplaneFrontScaleUp, s * airplaneFrontScaleUp, s * airplaneFrontScaleUp);
   
   // plane_body dimensions
   double plane_body_length = 1.8;
   double plane_body_radius = 0.35;
   double nose_length = 0.5;
   double wall_thickness = 0.03;  // Thickness of plane_body walls
   int segments = 24;
   
   // OUTER Body (cylindrical body) 
   glColor3f(1, 1, 1);  // White for texture
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, plane_body_grey);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0 * 0.6);
   
   //  Enable textures for front plane
   extern int ntex;
   extern unsigned int texture[];
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[11]);  // rusty plane texture
   }
   
   // Define window cutout positions
   double window_positions[] = {0.3, 0.6, 0.9, 1.2};
   double side_window_height = 0.12;
   
   // Outer cylindrical plane_body body with window cutouts
   for (int i = 0; i < segments; i++)
   {
      double angle1 = 360.0 * i / segments;
      double angle2 = 360.0 * (i + 1) / segments;
      
      // Check if this segment should have windows cut out
      for (int seg_y = 0; seg_y < 20; seg_y++)
      {
         double y1 = (plane_body_length / 20.0) * seg_y;
         double y2 = (plane_body_length / 20.0) * (seg_y + 1);
         
         int hasWindow = 0;
         
         // Check for side windows (only on sides, not top/bottom)
         if (angle1 > 45 && angle1 < 135)  // Right side
         {
            for (int w = 0; w < 4; w++)
            {
               if (y1 < window_positions[w] + side_window_height/2 && 
                   y2 > window_positions[w] - side_window_height/2)
               {
                  hasWindow = 1;
                  break;
               }
            }
         }
         else if (angle1 > 225 && angle1 < 315)  // Left side
         {
            for (int w = 0; w < 4; w++)
            {
               if (y1 < window_positions[w] + side_window_height/2 && 
                   y2 > window_positions[w] - side_window_height/2)
               {
                  hasWindow = 1;
                  break;
               }
            }
         }
         
         if (hasWindow != 1)
         {
            // normals for cylinder
            double nx1 = Cos(angle1);
            double nz1 = Sin(angle1);
            double nx2 = Cos(angle2);
            double nz2 = Sin(angle2);
            
            glBegin(GL_QUADS);
            // Each vertex gets its own normal (smooth shading)
            glNormal3d(nx1, 0, nz1);
            glTexCoord2d((double)i/segments, y1/plane_body_length);
            glVertex3d(plane_body_radius * nx1, y1, plane_body_radius * nz1);
            
            glNormal3d(nx1, 0, nz1);
            glTexCoord2d((double)i/segments, y2/plane_body_length);
            glVertex3d(plane_body_radius * nx1, y2, plane_body_radius * nz1);
            
            glNormal3d(nx2, 0, nz2);
            glTexCoord2d((double)(i+1)/segments, y2/plane_body_length);
            glVertex3d(plane_body_radius * nx2, y2, plane_body_radius * nz2);
            
            glNormal3d(nx2, 0, nz2);
            glTexCoord2d((double)(i+1)/segments, y1/plane_body_length);
            glVertex3d(plane_body_radius * nx2, y1, plane_body_radius * nz2);
            glEnd();
         }
      }
   }
   
   // INNER plane_body (hollow interior) 
   glColor3f(0.3, 0.3, 0.35);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, interior_dark);
   
   double inner_radius = plane_body_radius - wall_thickness;
   
   // Inner cylinder (reversed normals pointing inward)
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= segments; i++)
   {
      double angle = 360.0 * i / segments;
      // Normals point inward for interior surface
      double nx = -Cos(angle);
      double nz = -Sin(angle);
      double vx = inner_radius * Cos(angle);
      double vz = inner_radius * Sin(angle);
      
      glNormal3d(nx, 0, nz);
      glVertex3d(vx, 0, vz);
      glVertex3d(vx, plane_body_length, vz);
   }
   glEnd();
   
   // WINDOW EDGES (connecting outer to inner) 
   glColor3f(0.5, 0.5, 0.55);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, plane_body_grey);
   
   // Side window frames
   for (int w = 0; w < 4; w++)
   {
      double win_y = window_positions[w];
      
      // Right side window frame (bottom edge)
      double right_angle = 90;
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= 8; i++)
      {
         double angle = right_angle - 20 + i * 5;
         double nx = Cos(angle);
         double nz = Sin(angle);
         
         // Normal points down for bottom edge
         glNormal3d(0, -1, 0);
         glVertex3d(plane_body_radius * nx, win_y - side_window_height/2, plane_body_radius * nz);
         glVertex3d(inner_radius * nx, win_y - side_window_height/2, inner_radius * nz);
      }
      glEnd();
      
      // Right side window frame (top edge)
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= 8; i++)
      {
         double angle = right_angle - 20 + i * 5;
         double nx = Cos(angle);
         double nz = Sin(angle);
         
         // Normal points up for top edge
         glNormal3d(0, 1, 0);
         glVertex3d(plane_body_radius * nx, win_y + side_window_height/2, plane_body_radius * nz);
         glVertex3d(inner_radius * nx, win_y + side_window_height/2, inner_radius * nz);
      }
      glEnd();
      
      // Left side window frame (bottom edge)
      double left_angle = 270;
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= 8; i++)
      {
         double angle = left_angle - 20 + i * 5;
         double nx = Cos(angle);
         double nz = Sin(angle);
         
         glNormal3d(0, -1, 0);
         glVertex3d(plane_body_radius * nx, win_y - side_window_height/2, plane_body_radius * nz);
         glVertex3d(inner_radius * nx, win_y - side_window_height/2, inner_radius * nz);
      }
      glEnd();
      
      // Left side window frame (top edge)
      glBegin(GL_QUAD_STRIP);
      for (int i = 0; i <= 8; i++)
      {
         double angle = left_angle - 20 + i * 5;
         double nx = Cos(angle);
         double nz = Sin(angle);
         
         glNormal3d(0, 1, 0);
         glVertex3d(plane_body_radius * nx, win_y + side_window_height/2, plane_body_radius * nz);
         glVertex3d(inner_radius * nx, win_y + side_window_height/2, inner_radius * nz);
      }
      glEnd();
   }
   
   // NOSE CONE (tapered front with hollow interior) 
   glColor3f(0.7, 0.7, 0.75);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, plane_body_grey);
   
   // Outer nose
   for (int slice = 0; slice < segments; slice++)
   {
      double angle1 = 360.0 * slice / segments;
      double angle2 = 360.0 * (slice + 1) / segments;
      
      // Skip front section for cockpit window
      if (angle1 > 30 && angle1 < 150)
         continue;
      
      double x1 = plane_body_radius * Cos(angle1);
      double z1 = plane_body_radius * Sin(angle1);
      double x2 = plane_body_radius * Cos(angle2);
      double z2 = plane_body_radius * Sin(angle2);
      
      double tip_y = plane_body_length + nose_length;
      
      // normal calculation for cone surface
      // Vector from base to tip
      double edge1_x = 0 - x1;
      double edge1_y = tip_y - plane_body_length;
      double edge1_z = 0 - z1;
      
      // Tangent around the cone
      double edge2_x = x2 - x1;
      double edge2_y = 0;
      double edge2_z = z2 - z1;
      
      // Cross product for normal (outward)
      double nx = edge1_y * edge2_z - edge1_z * edge2_y;
      double ny = edge1_z * edge2_x - edge1_x * edge2_z;
      double nz = edge1_x * edge2_y - edge1_y * edge2_x;
      
      // Normalize
      double len = sqrt(nx*nx + ny*ny + nz*nz);
      if (len > 0.001) {
         nx /= len;
         ny /= len;
         nz /= len;
      }
      
      glBegin(GL_TRIANGLES);
      glNormal3d(nx, ny, nz);
      glVertex3d(x1, plane_body_length, z1);
      glVertex3d(x2, plane_body_length, z2);
      glVertex3d(0, tip_y, 0);
      glEnd();
   }
   
   // COCKPIT WINDOW OPENING (hollow cutout in nose)  
   double cockpit_y1 = plane_body_length + 0.1;
   
   // Cockpit window frame edges
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i <= 10; i++)
   {
      double t = i / 10.0;
      double angle = 30 + t * 120;
      double nx = Cos(angle);
      double nz = Sin(angle);
      
      // normal for window frame
      // The normal should point somewhat forward and outward
      double ny_component = 0.3;
      double norm_len = sqrt(nx*nx + ny_component*ny_component + nz*nz);
      
      glNormal3d(nx/norm_len, ny_component/norm_len, nz/norm_len);
      glVertex3d(plane_body_radius * nx * 0.9, cockpit_y1, plane_body_radius * nz * 0.9);
      glVertex3d(inner_radius * nx * 0.9, cockpit_y1, inner_radius * nz * 0.9);
   }
   glEnd();
   
   // DAMAGE SECTION (torn metal on one side) 
   glColor3f(0.4, 0.4, 0.45);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dark_grey);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0 * 0.3);
   
   double damage_start_y = 0.1;
   double damage_angles[] = {180, 195, 210, 225, 240, 200, 185};
   double damage_radii[] = {0.38, 0.42, 0.40, 0.45, 0.41, 0.44, 0.39};
   
   // Calculate average normal for damaged area
   double avg_nx = -0.7, avg_ny = -0.5, avg_nz = -0.5;
   double avg_len = sqrt(avg_nx*avg_nx + avg_ny*avg_ny + avg_nz*avg_nz);
   avg_nx /= avg_len; avg_ny /= avg_len; avg_nz /= avg_len;
   
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(avg_nx, avg_ny, avg_nz);
   glVertex3d(-plane_body_radius * 0.6, damage_start_y + 0.2, -plane_body_radius * 0.3);
   
   for (int i = 0; i < 7; i++)
   {
      double angle = damage_angles[i];
      double r = damage_radii[i];
      double dx = r * Cos(angle);
      double dz = r * Sin(angle);
      
      // normal for each vertex
      double nx = Cos(angle);
      double ny = -0.3;
      double nz = Sin(angle);
      double nlen = sqrt(nx*nx + ny*ny + nz*nz);
      nx /= nlen; ny /= nlen; nz /= nlen;
      
      glNormal3d(nx, ny, nz);
      glVertex3d(dx, damage_start_y, dz);
   }
   glEnd();
   
   // Bent metal panels
   for (int panel = 0; panel < 3; panel++)
   {
      double py = 0.15 + panel * 0.15;
      double pangle = 200 + panel * 15;
      double pr = plane_body_radius * 0.95;
      
      glBegin(GL_TRIANGLES);
      for (int detail = 0; detail < 2; detail++)
      {
         double a1 = pangle + detail * 8;
         double a2 = pangle + (detail + 1) * 8;
         
         double x1 = pr * Cos(a1);
         double z1 = pr * Sin(a1);
         double x2 = pr * Cos(a2);
         double z2 = pr * Sin(a2);
         double x3 = (x1 + x2) / 2.0 * 1.1;
         double z3 = (z1 + z2) / 2.0 * 1.1;
         
         // cross product for triangle normal
         double edge1_x = x2 - x1;
         double edge1_y = 0;
         double edge1_z = z2 - z1;
         
         double edge2_x = x3 - x1;
         double edge2_y = 0.08;
         double edge2_z = z3 - z1;
         
         double nx = edge1_y * edge2_z - edge1_z * edge2_y;
         double ny = edge1_z * edge2_x - edge1_x * edge2_z;
         double nz = edge1_x * edge2_y - edge1_y * edge2_x;
         
         double nlen = sqrt(nx*nx + ny*ny + nz*nz);
         if (nlen > 0.001) {
            nx /= nlen;
            ny /= nlen;
            nz /= nlen;
         } else {
            nx = 0; ny = 1; nz = 0;
         }
         
         glNormal3d(nx, ny, nz);
         glVertex3d(x1, py, z1);
         glVertex3d(x2, py, z2);
         glVertex3d(x3, py + 0.08, z3);
      }
      glEnd();
   }
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}