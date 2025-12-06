#include "CSCIx229.h"
#include "House.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// House scale multiplier
double houseScaleUp = 2.0;

/*
 *  House (cabin-type)
 *     at (x,y,z)
 *     scale s
 */
void house(double x, double y, double z, double s)
{
   //  Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float grey[] = {0.5, 0.5, 0.5, 1.0};
   float dark_grey[] = {0.3, 0.3, 0.3, 1.0};
   float brown[] = {0.4, 0.2, 0.0, 1.0};
   float specular[] = {0.2, 0.2, 0.2, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * houseScaleUp, s * houseScaleUp, s * houseScaleUp);
   
   //  WALLS 
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   double wall_width = 0.8;
   double wall_height = 1.0;
   double wall_depth = 0.6;
   
   //  Enable textures for walls
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[1]);
   }
   
   glBegin(GL_QUADS);
   
   //  Front face
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-wall_width, 0, wall_depth);
   glTexCoord2f(1, 0); glVertex3f(+wall_width, 0, wall_depth);
   glTexCoord2f(1, 1); glVertex3f(+wall_width, wall_height, wall_depth);
   glTexCoord2f(0, 1); glVertex3f(-wall_width, wall_height, wall_depth);
   
   //  Back face
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(+wall_width, 0, -wall_depth);
   glTexCoord2f(1, 0); glVertex3f(-wall_width, 0, -wall_depth);
   glTexCoord2f(1, 1); glVertex3f(-wall_width, wall_height+0.2, -wall_depth);
   glTexCoord2f(0, 1); glVertex3f(+wall_width, wall_height+0.2, -wall_depth);
   
   //  Right face
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(+wall_width, 0, +wall_depth);
   glTexCoord2f(1, 0); glVertex3f(+wall_width, 0, -wall_depth);
   glTexCoord2f(1, 1); glVertex3f(+wall_width, wall_height+0.2, -wall_depth);
   glTexCoord2f(0, 1); glVertex3f(+wall_width, wall_height, +wall_depth);
   
   //  Left face
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-wall_width, 0, -wall_depth);
   glTexCoord2f(1, 0); glVertex3f(-wall_width, 0, +wall_depth);
   glTexCoord2f(1, 1); glVertex3f(-wall_width, wall_height, +wall_depth);
   glTexCoord2f(0, 1); glVertex3f(-wall_width, wall_height+0.2, -wall_depth);
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   //  FLAT SLANTED ROOF 
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dark_grey);
   
   double roof_width = wall_width + 0.1;
   double roof_depth = wall_depth + 0.1;
   double roof_height = 0.15;
   double roof_y = wall_height;
   
   //  Enable textures for roof
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[2]);
   }
   
   glBegin(GL_QUADS);
   
   //  Roof top
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-roof_width, roof_y + roof_height+0.2, -roof_depth);
   glTexCoord2f(1, 0); glVertex3f(+roof_width, roof_y + roof_height+0.2, -roof_depth);
   glTexCoord2f(1, 1); glVertex3f(+roof_width, roof_y + roof_height, +roof_depth);
   glTexCoord2f(0, 1); glVertex3f(-roof_width, roof_y + roof_height, +roof_depth);
   
   //  Roof bottom
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(0, 1); glVertex3f(-roof_width, roof_y, +roof_depth);
   glTexCoord2f(1, 1); glVertex3f(+roof_width, roof_y, +roof_depth);
   glTexCoord2f(1, 0); glVertex3f(+roof_width, roof_y+0.2, -roof_depth);
   
   //  Roof front edge
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-roof_width, roof_y, roof_depth);
   glTexCoord2f(1, 0); glVertex3f(+roof_width, roof_y, roof_depth);
   glTexCoord2f(1, 1); glVertex3f(+roof_width, roof_y + roof_height, roof_depth);
   glTexCoord2f(0, 1); glVertex3f(-roof_width, roof_y + roof_height, roof_depth);
   
   //  Roof back edge
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(+roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(1, 0); glVertex3f(-roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(1, 1); glVertex3f(-roof_width, roof_y + roof_height+0.2, -roof_depth);
   glTexCoord2f(0, 1); glVertex3f(+roof_width, roof_y + roof_height+0.2, -roof_depth);
   
   //  Roof right edge
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(+roof_width, roof_y, +roof_depth);
   glTexCoord2f(1, 0); glVertex3f(+roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(1, 1); glVertex3f(+roof_width, roof_y + roof_height+0.2, -roof_depth);
   glTexCoord2f(0, 1); glVertex3f(+roof_width, roof_y + roof_height, +roof_depth);
   
   //  Roof left edge
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-roof_width, roof_y+0.2, -roof_depth);
   glTexCoord2f(1, 0); glVertex3f(-roof_width, roof_y, +roof_depth);
   glTexCoord2f(1, 1); glVertex3f(-roof_width, roof_y + roof_height, +roof_depth);
   glTexCoord2f(0, 1); glVertex3f(-roof_width, roof_y + roof_height+0.2, -roof_depth);
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   //  DOOR (with texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brown);
   
   double door_width = 0.25;
   double door_height = 0.5;
   double door_z = wall_depth + 0.01;
   
   //  Enable textures for door
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);
   }
   
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-door_width, 0, door_z);
   glTexCoord2f(1, 0); glVertex3f(+door_width, 0, door_z);
   glTexCoord2f(1, 1); glVertex3f(+door_width, door_height, door_z);
   glTexCoord2f(0, 1); glVertex3f(-door_width, door_height, door_z);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}
