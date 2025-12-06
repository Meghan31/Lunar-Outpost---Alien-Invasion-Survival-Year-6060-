/*
 *  Alien Scout Drone - Searches and scans for "FloatingCrystalShards"
 * Took assistance from AI in building the particle system and shader integration
 */

#include "CSCIx229.h"
#include "shader_loader.h"
#include <math.h>
#include <stdlib.h>

// Shader program
static unsigned int droneShaderProgram = 0;
static int shadersInitialized = 0;

// Texture IDs for realistic drone appearance
static unsigned int droneBodyTexture = 0;
static unsigned int dronePanelTexture = 0;
static unsigned int droneMetalTexture = 0;
static int texturesLoaded = 0;

// Particle system for propulsion effects
#define MAX_PROPULSION_PARTICLES 100
typedef struct {
   float x, y, z;
   float vx, vy, vz;
   float life;
   float size;
   float color[4];
} PropulsionParticle;

static PropulsionParticle propulsionParticles[MAX_PROPULSION_PARTICLES];
static int propulsionInitialized = 0;

/*
 * Initializing shader programs
 */
static void InitShaders()
{
   if (shadersInitialized) return;
   
   droneShaderProgram = CreateShaderProgram(
      "shaders/drone.vert",
      "shaders/drone.frag"
   );
   
   if (droneShaderProgram == 0) {
      fprintf(stderr, "Warning: Failed to load drone shaders, using fixed pipeline\n");
   }
   
   shadersInitialized = 1;
}

/*
 * Loading drone textures
 */
static void LoadDroneTextures()
{
   if (texturesLoaded) return;
   
   droneBodyTexture = LoadTexBMP("textures/metals/blue_metal.bmp");
   dronePanelTexture = LoadTexBMP("textures/metals/metal_plate.bmp");
   droneMetalTexture = LoadTexBMP("textures/metals/green_metal_rust.bmp");
   
   texturesLoaded = 1;
}

/*
 * Initializing propulsion particle system
 */
static void InitPropulsion()
{
   if (propulsionInitialized) return;
   
   for (int i = 0; i < MAX_PROPULSION_PARTICLES; i++) {
      propulsionParticles[i].life = 0.0;
   }
   propulsionInitialized = 1;
}

/*
 * Emitting propulsion particle
 */
static void EmitPropulsionParticle(float x, float y, float z, float time)
{
   for (int i = 0; i < MAX_PROPULSION_PARTICLES; i++) {
      if (propulsionParticles[i].life <= 0.0) {
         propulsionParticles[i].x = x;
         propulsionParticles[i].y = y;
         propulsionParticles[i].z = z;
         
         // Creating downward thrust
         float angle = (rand() % 360) * M_PI / 180.0;
         propulsionParticles[i].vx = cos(angle) * 0.02;
         propulsionParticles[i].vy = -0.08 - (rand() % 100) / 500.0;
         propulsionParticles[i].vz = sin(angle) * 0.02;
         
         propulsionParticles[i].life = 0.5 + (rand() % 100) / 200.0;
         propulsionParticles[i].size = 0.03 + (rand() % 100) / 2000.0;
         
         // Setting cyan-blue thrust color
         propulsionParticles[i].color[0] = 0.0;
         propulsionParticles[i].color[1] = 0.5 + (rand() % 50) / 100.0;
         propulsionParticles[i].color[2] = 1.0;
         propulsionParticles[i].color[3] = 1.0;
         
         break;
      }
   }
}

/*
 * Updating propulsion particles
 * Took help from AI to finish this part
 */
static void UpdatePropulsion(float dt)
{
   for (int i = 0; i < MAX_PROPULSION_PARTICLES; i++) {
      if (propulsionParticles[i].life > 0.0) {
         propulsionParticles[i].x += propulsionParticles[i].vx;
         propulsionParticles[i].y += propulsionParticles[i].vy;
         propulsionParticles[i].z += propulsionParticles[i].vz;
         
         propulsionParticles[i].life -= dt;
         propulsionParticles[i].color[3] = propulsionParticles[i].life * 2.0;
      }
   }
}

/*
 * Drawing propulsion particles
 */
static void DrawPropulsion()
{
   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glDepthMask(GL_FALSE);
   
   for (int i = 0; i < MAX_PROPULSION_PARTICLES; i++) {
      if (propulsionParticles[i].life > 0.0) {
         glPushMatrix();
         glTranslatef(propulsionParticles[i].x, 
                     propulsionParticles[i].y, 
                     propulsionParticles[i].z);
         
         glColor4fv(propulsionParticles[i].color);
         
         // Drawing particle as point sprite
         glPointSize(propulsionParticles[i].size * 100.0);
         glBegin(GL_POINTS);
         glVertex3f(0, 0, 0);
         glEnd();
         
         // Adding glow halo
         float alpha = propulsionParticles[i].color[3] * 0.3;
         glColor4f(propulsionParticles[i].color[0],
                  propulsionParticles[i].color[1],
                  propulsionParticles[i].color[2],
                  alpha);
         glPointSize(propulsionParticles[i].size * 200.0);
         glBegin(GL_POINTS);
         glVertex3f(0, 0, 0);
         glEnd();
         
         glPopMatrix();
      }
   }
   
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
}

// Drawing realistic textured drone body (cylindrical fuselage with sphere ends)
static void DrawRealisticBody(float size, float time, int mode)
{
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, droneBodyTexture);
   
   // Material properties - metallic finish
   float bodyColor[] = {0.75, 0.78, 0.82, 1.0};
   float bodySpec[] = {0.8, 0.82, 0.85, 1.0};
   float emission[] = {0.0, 0.0, 0.0, 1.0};
   
   // Subtle mode indicator glow
   if (mode == 1) {
      emission[0] = 0.0; emission[1] = 0.15; emission[2] = 0.2;  // Scanning cyan glow
   } else if (mode == 2) {
      emission[0] = 0.2; emission[1] = 0.05; emission[2] = 0.0;  // Combat red glow
   }
   
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 85.0f);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bodyColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bodySpec);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   
   glColor3f(1, 1, 1);  // Full white to show texture
   
   // Main cylindrical body
   glPushMatrix();
   glRotatef(90, 0, 1, 0);
   glScalef(size, size, size);
   
   GLUquadric* quad = gluNewQuadric();
   gluQuadricTexture(quad, GL_TRUE);
   gluQuadricNormals(quad, GLU_SMOOTH);
   
   // Central cylinder
   gluCylinder(quad, 0.35, 0.35, 0.8, 20, 1);
   
   // Front cap (sphere)
   glPushMatrix();
   glTranslatef(0, 0, 0.8);
   glutSolidSphere(0.35, 16, 16);
   glPopMatrix();
   
   // Rear cap (sphere)
   glPushMatrix();
   glScalef(1, 1, 0.8);
   glutSolidSphere(0.35, 16, 16);
   glPopMatrix();
   
   gluDeleteQuadric(quad);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
}

// Drawing realistic textured angular panel/wing
// Took help from AI to finish this part
static void DrawRealisticPanel(float size)
{
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, dronePanelTexture);
   
   float panelColor[] = {0.65, 0.68, 0.70, 1.0};
   float panelSpec[] = {0.7, 0.72, 0.75, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, panelColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, panelSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0f);
   
   glColor3f(1, 1, 1);
   
   // Angular panel with thickness
   glBegin(GL_QUADS);
   // Front face
   glNormal3f(0, 0, 1);
   glTexCoord2f(0, 0); glVertex3f(0, 0, 0.02);
   glTexCoord2f(1, 0); glVertex3f(size, 0.12, 0.02);
   glTexCoord2f(1, 1); glVertex3f(size, -0.12, 0.02);
   glTexCoord2f(0, 1); glVertex3f(0, 0, 0.02);
   
   // Back face
   glNormal3f(0, 0, -1);
   glTexCoord2f(0, 0); glVertex3f(0, 0, -0.02);
   glTexCoord2f(0, 1); glVertex3f(size, -0.12, -0.02);
   glTexCoord2f(1, 1); glVertex3f(size, 0.12, -0.02);
   glTexCoord2f(1, 0); glVertex3f(0, 0, -0.02);
   
   // Top edge
   glNormal3f(0, 1, 0);
   glTexCoord2f(0, 0); glVertex3f(0, 0, 0.02);
   glTexCoord2f(1, 0); glVertex3f(size, 0.12, 0.02);
   glTexCoord2f(1, 1); glVertex3f(size, 0.12, -0.02);
   glTexCoord2f(0, 1); glVertex3f(0, 0, -0.02);
   
   // Bottom edge
   glNormal3f(0, -1, 0);
   glTexCoord2f(0, 0); glVertex3f(0, 0, 0.02);
   glTexCoord2f(0, 1); glVertex3f(0, 0, -0.02);
   glTexCoord2f(1, 1); glVertex3f(size, -0.12, -0.02);
   glTexCoord2f(1, 0); glVertex3f(size, -0.12, 0.02);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   // Add subtle edge detail with dark metallic strip
   glDisable(GL_LIGHTING);
   glColor3f(0.15, 0.16, 0.17);
   glLineWidth(2.0);
   glBegin(GL_LINE_LOOP);
   glVertex3f(0, 0, 0.02);
   glVertex3f(size, 0.12, 0.02);
   glVertex3f(size, -0.12, 0.02);
   glEnd();
   glEnable(GL_LIGHTING);
}

// Drawing optical sphere eye (more subtle)
static void DrawRealisticSensor(float time)
{
   // Dark sensor lens
   float sensorColor[] = {0.08, 0.10, 0.12, 1.0};
   float sensorSpec[] = {0.9, 0.92, 0.95, 1.0};
   float scan_pulse = 0.3 + 0.2 * sin(time * 3.0);
   float emission[] = {scan_pulse * 0.1, scan_pulse * 0.15, scan_pulse * 0.2, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sensorColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sensorSpec);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120.0f);
   
   glutSolidSphere(0.08, 16, 16);
   
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
}

// Drawing laser beam
static void DrawLaser(float length, float intensity)
{
   float red_em[] = {1.0, 0.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, red_em);
   
   glDisable(GL_LIGHTING);
   
   // Rendering core beam
   glColor3f(1, 0, 0);
   glBegin(GL_LINES);
   glVertex3f(0, 0, 0);
   glVertex3f(length, 0, 0);
   glEnd();
   
   // Creating glow halo
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


//   AlienScoutDrone

void AlienScoutDrone(double x, double y, double z, 
                     double time, double moveDir, double speed,
                     int scanning)
{
   // Initializing systems
   InitShaders();
   LoadDroneTextures();
   InitPropulsion();
   
   // Determining mode: 0=idle, 1=scanning, 2=combat
   int mode = scanning ? (speed > 0.3 ? 2 : 1) : 0;
   
   glPushMatrix();
   glTranslatef(x, y, z);
   
   // Creating subtle hover oscillation (less cartoonish)
   float hover = sin(time * 2.0 * M_PI / 1.5) * 0.08;
   glTranslatef(0, hover, 0);
   
   // Facing movement direction
   glRotatef(moveDir, 0, 1, 0);
   
   // Drawing realistic body
   glPushMatrix();
   DrawRealisticBody(0.50, time, mode);
   glPopMatrix();
   
   // Drawing sensor
   glPushMatrix();
   glTranslatef(0.40, 0, 0);
   DrawRealisticSensor(time);
   glPopMatrix();
   
   // Drawing laser if scanning (keep but make less bright)
   if (scanning) {
      glPushMatrix();
      glTranslatef(0.40, 0, 0);
      float sweep = sin(time * 30.0 * M_PI / 180.0) * 12.0;
      glRotatef(sweep, 0, 1, 0);
      DrawLaser(5.0, 0.6);  // Reduced intensity
      glPopMatrix();
   }
   
   // Drawing four realistic panels (sensor/stabilizer arrays)
   float panelTilt = speed * 15.0;  // Less dramatic than before
   
   for (int i = 0; i < 4; i++) {
      glPushMatrix();
      glRotatef(i * 90, 1, 0, 0);
      glTranslatef(0, 0.28, 0);
      
      float individualTilt = panelTilt * cos((i * 90 + moveDir) * M_PI / 180.0);
      glRotatef(individualTilt, 0, 0, 1);
      
      // Subtle articulation (less flapping)
      float flap = sin(time * 3.5 + i) * 1.5;
      glRotatef(flap, 0, 0, 1);
      
      DrawRealisticPanel(0.35);
      glPopMatrix();
   }
   
   // Emitting and updating propulsion particles (more subtle)
   if ((int)(time * 60.0) % 3 == 0) {  // Less frequent
      EmitPropulsionParticle(0, -0.35, 0, time);
   }
   UpdatePropulsion(1.0/60.0);
   
   // Drawing propulsion
   DrawPropulsion();
   
   glPopMatrix();
}

