#include "CSCIx229.h"
#include "Tree.h"
#include "Primitives.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// Tree scale multiplier
double treeScaleUp = 3.1;

/*
 *  Tree
 *     at (x,y,z)
 *     scale s
 */
void tree(double x, double y, double z, double s)
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
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brown);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[3]);
   }
   cylinder(0, 0, 0, trunk_radius, trunk_height, 16);
   glDisable(GL_TEXTURE_2D);
   
   //  Drawing foliage (green cone with leaves texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[4]);
   }
   //  Position cone at top of trunk
   cone(0, trunk_height, 0, foliage_radius, foliage_height, 16);
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}
