#include "CSCIx229.h"
#include "Ball.h"

// External variables
extern float shiny;
extern int emission;

/*
 *  Draw a ball for light position
 *     at (x,y,z)
 *     radius (r)
 */
void ball(double x, double y, double z, double r)
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
