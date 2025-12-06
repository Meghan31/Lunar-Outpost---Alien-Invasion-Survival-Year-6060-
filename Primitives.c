#include "CSCIx229.h"
#include "Primitives.h"

/*
 *  Cylinder (for tree trunk, table legs, etc)
 *     at (x,y,z)
 *     radius r, height h
 *     with n segments around circumference
 */
void cylinder(double x, double y, double z,
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
void cone(double x, double y, double z,
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
void sphere(double x, double y, double z, double r)
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
