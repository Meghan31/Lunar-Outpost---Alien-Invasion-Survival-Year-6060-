#include "CSCIx229.h"
#include "Ground.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

/*
 *  Ground plane
 *     size: dimension of the square ground
 */
void ground(double size)
{
   //  Set material properties - grass green
   float green[] = {0.2, 0.6, 0.2, 1.0};
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float white[] = {0.3, 0.3, 0.3, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glColor3f(1, 1, 1);  // White color so texture shows properly
   
   //  Enable textures
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[0]);
   }
   
   //  Drawing ground as a flat square at y=0
   //  Normal points up (0, 1, 0) 
   glNormal3f(0, 1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3f(-size, 0, -size);
   glTexCoord2f(5, 0); glVertex3f(+size, 0, -size);
   glTexCoord2f(5, 5); glVertex3f(+size, 0, +size);
   glTexCoord2f(0, 5); glVertex3f(-size, 0, +size);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
}
