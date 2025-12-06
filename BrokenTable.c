#include "CSCIx229.h"
#include "BrokenTable.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// Table scale multiplier
double tableScaleUp = 1.0;

/*
 *  Broken table (two pieces fallen on ground)
 *     at (x,y,z)
 *     scale s
 */
void broken_table(double x, double y, double z, double s)
{
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float wood[] = {0.4, 0.25, 0.15, 1.0};
   float specular[] = {0.2, 0.2, 0.2, 1.0};
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(s * tableScaleUp, s * tableScaleUp, s * tableScaleUp);
   
   // Dimensions
   double table_thickness = 0.08;
   
   // === LEFT PIECE (larger, fallen on ground tilted left) ===
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wood);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   //  Enable textures for table top
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);
   }
   
   glPushMatrix();
   glTranslated(-0.8, 0.35, 0);
   glRotated(-50, 0, 0, 1);
   
   // Top surface
   glBegin(GL_QUADS);
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.6, table_thickness, -0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, table_thickness, -0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, table_thickness, 0.4);
   glEnd();
   
   // Bottom surface
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.6, 0, -0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, 0, 0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, 0, 0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.4);
   glEnd();
   
   // Front edge
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-0.6, 0, 0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, table_thickness, 0.4);
   glEnd();
   
   // Back edge
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(-0.6, 0, -0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, table_thickness, -0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, -0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.4);
   glEnd();
   
   // Left edge
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.6, 0, -0.4);
   glTexCoord2f(1, 0); glVertex3f(-0.6, 0, 0.4);
   glTexCoord2f(1, 1); glVertex3f(-0.6, table_thickness, 0.4);
   glTexCoord2f(0, 1); glVertex3f(-0.6, table_thickness, -0.4);
   glEnd();
   
   // Right edge (broken/jagged)
   glBegin(GL_QUADS);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(0.5, 0, -0.4);
   glTexCoord2f(0, 1); glVertex3f(0.5, table_thickness, -0.4);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.4);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0.4);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   // Left table leg 1
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wood);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[6]);
   }
   
   double leg1_y_bottom = -0.75;
   
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(-0.3, table_thickness, 0.3);
   glVertex3f(-0.2, table_thickness, 0.3);
   glVertex3f(-0.2, leg1_y_bottom, 0.3);
   glVertex3f(-0.3, leg1_y_bottom, 0.3);
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(-0.3, table_thickness, 0.2);
   glVertex3f(-0.3, leg1_y_bottom, 0.2);
   glVertex3f(-0.2, leg1_y_bottom, 0.2);
   glVertex3f(-0.2, table_thickness, 0.2);
   // Left face
   glNormal3f(-1, 0, 0);
   glVertex3f(-0.3, table_thickness, 0.2);
   glVertex3f(-0.3, table_thickness, 0.3);
   glVertex3f(-0.3, leg1_y_bottom, 0.3);
   glVertex3f(-0.3, leg1_y_bottom, 0.2);
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(-0.2, table_thickness, 0.2);
   glVertex3f(-0.2, leg1_y_bottom, 0.2);
   glVertex3f(-0.2, leg1_y_bottom, 0.3);
   glVertex3f(-0.2, table_thickness, 0.3);
   // Bottom face
   glNormal3f(0, -1, 0);
   glVertex3f(-0.3, leg1_y_bottom, 0.2);
   glVertex3f(-0.3, leg1_y_bottom, 0.3);
   glVertex3f(-0.2, leg1_y_bottom, 0.3);
   glVertex3f(-0.2, leg1_y_bottom, 0.2);
   glEnd();
   
   // Left table leg 2
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(-0.3, table_thickness, -0.2);
   glVertex3f(-0.2, table_thickness, -0.2);
   glVertex3f(-0.2, leg1_y_bottom, -0.2);
   glVertex3f(-0.3, leg1_y_bottom, -0.2);
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(-0.3, table_thickness, -0.3);
   glVertex3f(-0.3, leg1_y_bottom, -0.3);
   glVertex3f(-0.2, leg1_y_bottom, -0.3);
   glVertex3f(-0.2, table_thickness, -0.3);
   // Left face
   glNormal3f(-1, 0, 0);
   glVertex3f(-0.3, table_thickness, -0.3);
   glVertex3f(-0.3, table_thickness, -0.2);
   glVertex3f(-0.3, leg1_y_bottom, -0.2);
   glVertex3f(-0.3, leg1_y_bottom, -0.3);
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(-0.2, table_thickness, -0.3);
   glVertex3f(-0.2, leg1_y_bottom, -0.3);
   glVertex3f(-0.2, leg1_y_bottom, -0.2);
   glVertex3f(-0.2, table_thickness, -0.2);
   // Bottom face
   glNormal3f(0, -1, 0);
   glVertex3f(-0.3, leg1_y_bottom, -0.3);
   glVertex3f(-0.3, leg1_y_bottom, -0.2);
   glVertex3f(-0.2, leg1_y_bottom, -0.2);
   glVertex3f(-0.2, leg1_y_bottom, -0.3);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
   
   // === RIGHT PIECE (smaller, fallen on ground tilted right) ===
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wood);
   
   glPushMatrix();
   glTranslated(0.9, 0.25, 0);
   glRotated(45, 0, 0, 1);
   glRotated(-15, 1, 0, 0);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);
   }
   
   // Top surface
   glBegin(GL_QUADS);
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.4, table_thickness, -0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, table_thickness, -0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, table_thickness, 0.35);
   glEnd();
   
   // Bottom surface
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.4, 0, -0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, 0, 0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, 0, 0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.35);
   glEnd();
   
   // Front edge
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(-0.4, 0, 0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, table_thickness, 0.35);
   glEnd();
   
   // Back edge
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(-0.4, 0, -0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, table_thickness, -0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, -0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.35);
   glEnd();
   
   // Left edge (broken/jagged)
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(-0.4, 0, -0.35);
   glTexCoord2f(1, 0); glVertex3f(-0.4, 0, 0.35);
   glTexCoord2f(1, 1); glVertex3f(-0.4, table_thickness, 0.35);
   glTexCoord2f(0, 1); glVertex3f(-0.4, table_thickness, -0.35);
   glEnd();
   
   // Right edge
   glBegin(GL_QUADS);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0, 0); glVertex3f(0.5, 0, -0.35);
   glTexCoord2f(0, 1); glVertex3f(0.5, table_thickness, -0.35);
   glTexCoord2f(1, 1); glVertex3f(0.5, table_thickness, 0.35);
   glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0.35);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   // Right table leg 1
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wood);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[6]);
   }
   
   double leg2_y_bottom = -0.5;
   
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(0.1, table_thickness, 0.25);
   glVertex3f(0.2, table_thickness, 0.25);
   glVertex3f(0.2, leg2_y_bottom, 0.25);
   glVertex3f(0.1, leg2_y_bottom, 0.25);
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(0.1, table_thickness, 0.15);
   glVertex3f(0.1, leg2_y_bottom, 0.15);
   glVertex3f(0.2, leg2_y_bottom, 0.15);
   glVertex3f(0.2, table_thickness, 0.15);
   // Left face
   glNormal3f(-1, 0, 0);
   glVertex3f(0.1, table_thickness, 0.15);
   glVertex3f(0.1, table_thickness, 0.25);
   glVertex3f(0.1, leg2_y_bottom, 0.25);
   glVertex3f(0.1, leg2_y_bottom, 0.15);
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(0.2, table_thickness, 0.15);
   glVertex3f(0.2, leg2_y_bottom, 0.15);
   glVertex3f(0.2, leg2_y_bottom, 0.25);
   glVertex3f(0.2, table_thickness, 0.25);
   // Bottom face
   glNormal3f(0, -1, 0);
   glVertex3f(0.1, leg2_y_bottom, 0.15);
   glVertex3f(0.1, leg2_y_bottom, 0.25);
   glVertex3f(0.2, leg2_y_bottom, 0.25);
   glVertex3f(0.2, leg2_y_bottom, 0.15);
   glEnd();
   
   // Right table leg 2
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glVertex3f(0.1, table_thickness, -0.15);
   glVertex3f(0.2, table_thickness, -0.15);
   glVertex3f(0.2, leg2_y_bottom, -0.15);
   glVertex3f(0.1, leg2_y_bottom, -0.15);
   // Back face
   glNormal3f(0, 0, -1);
   glVertex3f(0.1, table_thickness, -0.25);
   glVertex3f(0.1, leg2_y_bottom, -0.25);
   glVertex3f(0.2, leg2_y_bottom, -0.25);
   glVertex3f(0.2, table_thickness, -0.25);
   // Left face
   glNormal3f(-1, 0, 0);
   glVertex3f(0.1, table_thickness, -0.25);
   glVertex3f(0.1, table_thickness, -0.15);
   glVertex3f(0.1, leg2_y_bottom, -0.15);
   glVertex3f(0.1, leg2_y_bottom, -0.25);
   // Right face
   glNormal3f(1, 0, 0);
   glVertex3f(0.2, table_thickness, -0.25);
   glVertex3f(0.2, leg2_y_bottom, -0.25);
   glVertex3f(0.2, leg2_y_bottom, -0.15);
   glVertex3f(0.2, table_thickness, -0.15);
   // Bottom face
   glNormal3f(0, -1, 0);
   glVertex3f(0.1, leg2_y_bottom, -0.25);
   glVertex3f(0.1, leg2_y_bottom, -0.15);
   glVertex3f(0.2, leg2_y_bottom, -0.15);
   glVertex3f(0.2, leg2_y_bottom, -0.25);
   glEnd();
   
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}
