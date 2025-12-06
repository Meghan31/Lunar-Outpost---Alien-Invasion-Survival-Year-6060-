#include "CSCIx229.h"
#include "Boulder.h"
#include <math.h>

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// Boulder scale multiplier
double boulderScaleUp = 1.0;

/*
 *  Stone boulder
 *     at (x,y,z)
 *     scale s
 *    
 *  Use 'r'/'R' keys to decrease/increase boulderScaleUp
 */
void boulder(double x, double y, double z, double s)
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
   const double base_radius = 0.5;
   const int inc = 10;
   const double amp = 0.18;
   const double mossHeight = base_radius * 0.4;

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stone_base);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, stone_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 0.08);

   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[8]);
   }

   const double cy = base_radius;

   // Rock-like noise pattern
   #define ROCK_NOISE(th,ph) (0.5*Sin(3*(th)) + 0.3*Sin(5*(ph)) + 0.2*Sin(2*(th)+(ph)))

   // Build latitude bands with quad strips
   for (int ph = -90; ph < 90; ph += inc)
   {
      int ph2 = ph + inc;
      if (ph2 > 90) ph2 = 90;
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += inc)
      {
         for (int pass = 0; pass < 2; ++pass)
         {
            int p = pass ? ph : ph2;
            double noise = ROCK_NOISE(th, p);
            if (noise > 1.0) noise = 1.0; else if (noise < -1.0) noise = -1.0;
            double radius = base_radius * (1.0 + amp * noise);

            double cp = Cos(p);
            double sp = Sin(p);
            double ct = Cos(th);
            double st = Sin(th);

            double vx = radius * ct * cp;
            double vy = radius * sp + cy;
            if (vy < 0.0) vy = 0.0;
            double vz = radius * st * cp;

            double nx = vx;
            double ny = vy - cy;
            double nz = vz;
            double len = sqrt(nx*nx + ny*ny + nz*nz);
            if (len == 0) len = 1;
            nx /= len; ny /= len; nz /= len;

            double sunDir[3] = {0.3, 0.9, 0.2};
            double sunLen = sqrt(sunDir[0]*sunDir[0]+sunDir[1]*sunDir[1]+sunDir[2]*sunDir[2]);
            sunDir[0]/=sunLen; sunDir[1]/=sunLen; sunDir[2]/=sunLen;
            double sunDot = nx*sunDir[0] + ny*sunDir[1] + nz*sunDir[2];
            if (sunDot < 0) sunDot = 0;

            double shade = -0.07 * noise;
            double r = stone_base[0] + shade;
            double g = stone_base[1] + shade;
            double b = stone_base[2] + shade;

            double highlight = 0.15 * pow(sunDot, 1.5);
            r += highlight; g += highlight; b += highlight;

            if (ny < 0.0)
            {
               double f = (-ny); if (f > 1.0) f = 1.0;
               r -= 0.15 * f; g -= 0.15 * f; b -= 0.15 * f;
            }

            double localHeight = vy;
            if (localHeight < mossHeight && sunDot < 0.4)
            {
               double mossFactor = (mossHeight - localHeight)/mossHeight;
               if (mossFactor > 1.0) mossFactor = 1.0;
               r = r*(1.0 - mossFactor) + moss_col[0]*mossFactor;
               g = g*(1.0 - mossFactor) + moss_col[1]*mossFactor;
               b = b*(1.0 - mossFactor) + moss_col[2]*mossFactor;
            }

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
      double th = 30 + i*47;
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
