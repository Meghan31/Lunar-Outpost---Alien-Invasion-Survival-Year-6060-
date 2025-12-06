#include "CSCIx229.h"
#include "Human.h"
#include "Primitives.h"

// External variables
extern int ntex;
extern unsigned int texture[];
extern float shiny;

// Human scale multiplier
double humanScaleUp = 1.0;

/*
 *  Human figure
 *     at (x,y,z)
 *     scale s
 *     Use 'h'/'H' keys to decrease/increase humanScaleUp
 */
void human(double x, double y, double z, double s) {
   // Material properties
   float black[] = {0.0, 0.0, 0.0, 1.0};
   float skin[] = {0.9, 0.7, 0.6, 1.0};      // Skin tone
   float shirt[] = {0.2, 0.3, 0.8, 1.0};     // Blue shirt
   float pants[] = {0.3, 0.3, 0.3, 1.0};     // Dark grey pants
   float shoes[] = {0.4, 0.25, 0.15, 1.0};   // Brown shoes
   float hair[] = {0.35, 0.2, 0.1, 1.0};     // Brown hair for afro   
   float specular[] = {0.1, 0.1, 0.1, 1.0};
   float hair_specular[] = {0.3, 0.3, 0.3, 1.0};
   
   // Dimensions
   double head_radius = 0.15;
   double torso_radius = 0.12;
   double torso_height = 0.5;
   double arm_radius = 0.05;
   double arm_length = 0.4;
   double leg_radius = 0.06;
   double leg_length = 0.5;
   
   // Adjusting the Y position so feet stay on ground when scaling
   double y_adjusted = y + (leg_length * s * (humanScaleUp - 1.0));
   
   glPushMatrix();
   glTranslated(x, y_adjusted, z);
   glScaled(s * humanScaleUp, s * humanScaleUp, s * humanScaleUp);
   double hand_radius = 0.06;
   double foot_radius = 0.07;
   
   double head_y = torso_height + head_radius;
   
   // Afro hair (large sphere around head)
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, hair);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hair_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glColor3f(0.35, 0.2, 0.1);
   
   // Large spherical afro - positioned at head center
   glPushMatrix();
   glTranslated(0, head_y + head_radius - 0.02, -head_radius);
   glScaled(1.8, 1.8, 1.8);
   sphere(0, 0, 0, head_radius);
   glPopMatrix();
   
   // HEAD (face visible at front)
   glColor3f(0.9, 0.7, 0.6);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skin);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   sphere(0, head_y, 0, head_radius);
   
   // TORSO (shirt with texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shirt);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[12]);
   }
   cylinder(0, 0, 0, torso_radius, torso_height, 16);
   glDisable(GL_TEXTURE_2D);
   
   // LEFT ARM (sleeve with shirt texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shirt);
   
   glPushMatrix();
   glTranslated(-torso_radius, torso_height, 0);
   glRotated(-20, 0, 0, 1);
   glRotated(20, 1, 0, 0);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[12]);
   }
   cylinder(0, 0, 0, arm_radius, -arm_length, 12);
   glDisable(GL_TEXTURE_2D);
   
   // Left hand
   glColor3f(0.9, 0.7, 0.6);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skin);
   sphere(0, -arm_length, 0, hand_radius);
   glPopMatrix();
   
   // RIGHT ARM (sleeve with shirt texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shirt);
   
   glPushMatrix();
   glTranslated(torso_radius, torso_height, 0);
   glRotated(20, 0, 0, 1);
   glRotated(20, 1, 0, 0);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[12]);
   }
   cylinder(0, 0, 0, arm_radius, -arm_length, 12);
   glDisable(GL_TEXTURE_2D);
   
   // Right hand
   glColor3f(0.9, 0.7, 0.6);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skin);
   sphere(0, -arm_length, 0, hand_radius);
   glPopMatrix();
   
   // LEFT LEG (pants with texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pants);
   
   glPushMatrix();
   glTranslated(-torso_radius/2, 0, 0);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[13]);
   }
   cylinder(0, 0, 0, leg_radius, -leg_length, 12);
   glDisable(GL_TEXTURE_2D);
   
   // Left foot (shoe with texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shoes);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[14]);
   }
   
   glPushMatrix();
   glTranslated(0, -leg_length, foot_radius);
   glScaled(1.5, 1.0, 2.0);
   sphere(0, 0, 0, foot_radius);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();

   // RIGHT LEG (pants with texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pants);
   
   glPushMatrix();
   glTranslated(torso_radius/2, 0, 0);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[13]);
   }
   cylinder(0, 0, 0, leg_radius, -leg_length, 12);
   glDisable(GL_TEXTURE_2D);
   
   // Right foot (shoe with texture)
   glColor3f(1, 1, 1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shoes);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   
   if (ntex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[14]);
   }
   
   glPushMatrix();
   glTranslated(0, -leg_length, foot_radius);
   glScaled(1.5, 1.0, 2.0);
   sphere(0, 0, 0, foot_radius);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
   
   glPopMatrix();
}
