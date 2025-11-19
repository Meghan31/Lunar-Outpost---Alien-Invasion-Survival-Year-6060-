/*
 *  Alien Scout Drone
 *  Fast reconnaissance drones for alien forces
 */

#include "CSCIx229.h"
#include <math.h>

// Draw octahedral core
static void DrawCore(float size, float time)
{
   float black[] = {0.08, 0.08, 0.12, 1.0};
   float blue[] = {0.0, 0.55, 1.0, 1.0};
   float emission[] = {0.25, 0.65, 1.0, 1.0};
   
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 48.0f);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blue);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   
   // Micro-vibration when active
   float vib = sin(time * 100.0) * 0.002;
   glTranslatef(vib, vib * 0.5, 0);
   
   glScalef(size, size, size);
   
   // Octahedron vertices (beveled)
   float v[6][3] = {
      {0, 0.4, 0}, {0, -0.4, 0},
      {0.4, 0, 0}, {-0.4, 0, 0},
      {0, 0, 0.4}, {0, 0, -0.4}
   };
   
   // 8 faces
   int faces[8][3] = {
      {0,2,4}, {0,4,3}, {0,3,5}, {0,5,2},
      {1,4,2}, {1,3,4}, {1,5,3}, {1,2,5}
   };
   
   for (int i = 0; i < 8; i++) {
      glBegin(GL_TRIANGLES);
      for (int j = 0; j < 3; j++) {
         int idx = faces[i][j];
         glNormal3f(v[idx][0], v[idx][1], v[idx][2]);
         glVertex3f(v[idx][0], v[idx][1], v[idx][2]);
      }
      glEnd();
      
      // Beveled edges with blue accent
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
      glBegin(GL_LINE_LOOP);
      for (int j = 0; j < 3; j++) {
         int idx = faces[i][j];
         glVertex3f(v[idx][0], v[idx][1], v[idx][2]);
      }
      glEnd();
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
   }
}

// Draw triangular sensor wing
static void DrawWing(float size)
{
   float black[] = {0.08, 0.08, 0.12, 1.0};
   float blue[] = {0.0, 0.55, 1.0, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
   
   glBegin(GL_TRIANGLES);
   glNormal3f(0, 0, 1);
   glVertex3f(0, 0, 0);
   glVertex3f(size, 0.15, 0);
   glVertex3f(size, -0.15, 0);
   glEnd();
   
   // Blue edge strobe
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, blue);
   glBegin(GL_LINE_LOOP);
   glVertex3f(0, 0, 0);
   glVertex3f(size, 0.15, 0);
   glVertex3f(size, -0.15, 0);
   glEnd();
   
   float black_em[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black_em);
}

// Draw optical sphere eye
static void DrawEye(float time)
{
   float white[] = {1,1,1,1};
   float scan_pulse = 0.6 + 0.4 * sin(time * 3.0);
   float emission[] = {scan_pulse, scan_pulse, scan_pulse, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
   
   glutSolidSphere(0.08, 16, 16);
}

// Draw laser beam
static void DrawLaser(float length, float intensity)
{
   float red_em[] = {1.0, 0.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, red_em);
   
   glDisable(GL_LIGHTING);
   
   // Core beam
   glColor3f(1, 0, 0);
   glBegin(GL_LINES);
   glVertex3f(0, 0, 0);
   glVertex3f(length, 0, 0);
   glEnd();
   
   // Glow halo
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glBegin(GL_QUAD_STRIP);
   for (float t = 0; t <= length; t += 0.1) {
      float alpha = (1.0 - t/length) * intensity * 0.3;
      glColor4f(1, 0, 0, alpha);
      glVertex3f(t, 0.02, 0);
      glVertex3f(t, -0.02, 0);
   }
   glEnd();
   glDisable(GL_BLEND);
   
   glEnable(GL_LIGHTING);
   
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
}

/*
 *  Main function: AlienScoutDrone
 *  
 *  x,y,z: position
 *  time: animation time
 *  moveDir: movement direction angle (0-360)
 *  speed: movement speed (affects wing tilt)
 *  scanning: 1=laser active, 0=laser off
 */
void AlienScoutDrone(double x, double y, double z, 
                     double time, double moveDir, double speed,
                     int scanning)
{
   glPushMatrix();
   glTranslatef(x, y, z);
   
   // Hover oscillation (1.2 sec period)
   float hover = sin(time * 2.0 * M_PI / 1.2) * 0.15;
   glTranslatef(0, hover, 0);
   
   // Face movement direction
   glRotatef(moveDir, 0, 1, 0);
   
   // Draw core
   glPushMatrix();
   DrawCore(0.45, time);
   glPopMatrix();
   
   // Draw eye
   glPushMatrix();
   glTranslatef(0.35, 0, 0);
   DrawEye(time);
   glPopMatrix();
   
   // Draw laser if scanning
   if (scanning) {
      glPushMatrix();
      glTranslatef(0.35, 0, 0);
      float sweep = sin(time * 30.0 * M_PI / 180.0) * 15.0;
      glRotatef(sweep, 0, 1, 0);
      DrawLaser(5.0, 1.0);
      glPopMatrix();
   }
   
   // Draw four wings with articulation
   float wingTilt = speed * 25.0; // Max ±25° based on speed
   
   for (int i = 0; i < 4; i++) {
      glPushMatrix();
      glRotatef(i * 90, 1, 0, 0);
      glTranslatef(0, 0.25, 0);
      
      // Tilt based on movement
      float individualTilt = wingTilt * cos((i * 90 + moveDir) * M_PI / 180.0);
      glRotatef(individualTilt, 0, 0, 1);
      
      // Flap slightly
      float flap = sin(time * 5.0 + i) * 3.0;
      glRotatef(flap, 0, 0, 1);
      
      DrawWing(0.325);
      glPopMatrix();
   }
   
   glPopMatrix();
}
