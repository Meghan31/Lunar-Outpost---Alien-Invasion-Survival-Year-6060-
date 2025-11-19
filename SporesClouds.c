#include "CSCIx229.h"
#include <math.h>

// Spore cloud configuration
#define MAX_SPORES 50
#define MAX_CLOUDS 5

typedef struct {
    float x, y, z;           // Position
    float vx, vy, vz;        // Velocity (drift)
    float phase;             // Animation phase
    float size;              // Base size
    float pulse_speed;       // Individual pulse rate
} Spore;

typedef struct {
    float cx, cy, cz;        // Cloud center
    float radius;            // Cloud radius
    int num_spores;          // Number of spores
    Spore spores[MAX_SPORES];
    float master_phase;      // Master animation phase
    int active;              // Is cloud active
} SporeCloud;

static SporeCloud clouds[MAX_CLOUDS];
static float global_time = 0.0;

// Random float helper
static float randf(float min, float max) {
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

/*
 * Initialize spore clouds
 */
void InitSporeClouds() {
    srand(42); // Consistent randomization
    
    for (int c = 0; c < MAX_CLOUDS; c++) {
        clouds[c].cx = randf(-2.0, 2.0);
        clouds[c].cy = randf(0.2, 1.5);
        clouds[c].cz = randf(-2.0, 2.0);
        clouds[c].radius = randf(0.4, 0.7);
        clouds[c].num_spores = (int)randf(20, 50);
        clouds[c].master_phase = randf(0, 2*M_PI);
        clouds[c].active = 1;
        
        // Initialize individual spores
        for (int i = 0; i < clouds[c].num_spores; i++) {
            Spore *s = &clouds[c].spores[i];
            
            // Random position within cloud sphere
            float theta = randf(0, 2*M_PI);
            float phi = randf(-M_PI/2, M_PI/2);
            float r = randf(0, clouds[c].radius);
            
            s->x = clouds[c].cx + r * cos(theta) * cos(phi);
            s->y = clouds[c].cy + r * sin(phi);
            s->z = clouds[c].cz + r * sin(theta) * cos(phi);
            
            // Random drift velocity
            s->vx = randf(-0.02, 0.02);
            s->vy = randf(-0.01, 0.01);
            s->vz = randf(-0.02, 0.02);
            
            // Random animation parameters
            s->phase = randf(0, 2*M_PI);
            s->size = randf(0.02, 0.05);
            s->pulse_speed = randf(0.8, 1.2);
        }
    }
}

/*
 * Update spore cloud animations
 */
void UpdateSporeClouds(float dt) {
    global_time += dt;
    
    for (int c = 0; c < MAX_CLOUDS; c++) {
        if (!clouds[c].active) continue;
        
        clouds[c].master_phase += dt * 0.5;
        
        for (int i = 0; i < clouds[c].num_spores; i++) {
            Spore *s = &clouds[c].spores[i];
            
            // Update position with drift
            s->x += s->vx * dt;
            s->y += s->vy * dt;
            s->z += s->vz * dt;
            
            // Keep spores within cloud bounds (soft boundary)
            float dx = s->x - clouds[c].cx;
            float dy = s->y - clouds[c].cy;
            float dz = s->z - clouds[c].cz;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            
            if (dist > clouds[c].radius) {
                float factor = 0.95 * clouds[c].radius / dist;
                s->x = clouds[c].cx + dx * factor;
                s->y = clouds[c].cy + dy * factor;
                s->z = clouds[c].cz + dz * factor;
                
                // Reverse velocity component
                s->vx *= -0.5;
                s->vy *= -0.5;
                s->vz *= -0.5;
            }
            
            // Update animation phase
            s->phase += dt * s->pulse_speed * 3.5; // ~1.8 sec cycle
        }
    }
}

/*
 * Draw single spore as billboard quad
 */
static void DrawSpore(float x, float y, float z, float size, float intensity) {
    // Billboard quad facing camera
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Billboard rotation (simplified - faces viewer)
    // In a real implementation, you'd calculate this from camera orientation
    
    float s = size * (0.8 + 0.45 * intensity);
    
    // Glow color with pulsation
    float base_r = 0.25;
    float base_g = 1.0;
    float base_b = 0.22;
    
    // Add golden tint at pulse peaks
    float peak_factor = intensity * 0.5;
    float r = base_r + peak_factor * 0.65;
    float g = base_g - peak_factor * 0.05;
    float b = base_b + peak_factor * 0.18;
    
    // Emission intensity
    float emit_intensity = 0.85 + 0.15 * intensity;
    
    // Set material with emission
    float emission[] = {r * emit_intensity, g * emit_intensity, b * emit_intensity, 1.0};
    float color[] = {r, g, b, 0.35 + 0.25 * intensity};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    
    glColor4f(r, g, b, color[3]);
    
    // Draw billboarded quad
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(-s, -s, 0);
    glVertex3f( s, -s, 0);
    glVertex3f( s,  s, 0);
    glVertex3f(-s,  s, 0);
    glEnd();
    
    glPopMatrix();
}

/*
 * Draw volumetric halo around cloud
 */
static void DrawCloudHalo(float cx, float cy, float cz, float radius, float intensity) {
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    
    float halo_size = radius * 1.3;
    float alpha = 0.08 * (0.7 + 0.3 * intensity);
    
    float r = 0.4 + 0.3 * intensity;
    float g = 0.9;
    float b = 0.3 + 0.2 * intensity;
    
    glColor4f(r, g, b, alpha);
    
    // Draw nested semi-transparent spheres
    for (int layer = 0; layer < 3; layer++) {
        float scale = halo_size * (0.8 + layer * 0.2);
        glPushMatrix();
        glScalef(scale, scale, scale);
        
        // Low-poly sphere for performance
        for (int ph = -90; ph < 90; ph += 30) {
            glBegin(GL_QUAD_STRIP);
            for (int th = 0; th <= 360; th += 30) {
                float x1 = Sin(th) * Cos(ph);
                float y1 = Cos(th) * Cos(ph);
                float z1 = Sin(ph);
                float x2 = Sin(th) * Cos(ph+30);
                float y2 = Cos(th) * Cos(ph+30);
                float z2 = Sin(ph+30);
                
                glNormal3f(x1, y1, z1);
                glVertex3f(x1, y1, z1);
                glNormal3f(x2, y2, z2);
                glVertex3f(x2, y2, z2);
            }
            glEnd();
        }
        glPopMatrix();
    }
    
    glPopMatrix();
}

/*
 * Draw all spore clouds
 */
void DrawSporeClouds(int lighting_enabled) {
    // Enable blending for translucency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending
    glDepthMask(GL_FALSE); // Don't write to depth buffer
    
    // Disable lighting for pure emissive glow (or keep it for subtle interaction)
    int restore_lighting = 0;
    if (!lighting_enabled) {
        glEnable(GL_LIGHTING);
        restore_lighting = 1;
    }
    
    for (int c = 0; c < MAX_CLOUDS; c++) {
        if (!clouds[c].active) continue;
        
        // Calculate master intensity pulse
        float master_intensity = 0.5 + 0.5 * sin(clouds[c].master_phase);
        
        // Draw volumetric halo first (back to front)
        DrawCloudHalo(clouds[c].cx, clouds[c].cy, clouds[c].cz, 
                     clouds[c].radius, master_intensity);
        
        // Draw individual spores
        for (int i = 0; i < clouds[c].num_spores; i++) {
            Spore *s = &clouds[c].spores[i];
            
            // Calculate individual pulse (breathing effect)
            float pulse = sin(s->phase);
            float intensity = 0.5 + 0.5 * pulse;
            
            // Overlap enhancement (color saturation spike)
            float overlap_factor = 1.0;
            for (int j = 0; j < clouds[c].num_spores; j++) {
                if (i == j) continue;
                Spore *other = &clouds[c].spores[j];
                float dx = s->x - other->x;
                float dy = s->y - other->y;
                float dz = s->z - other->z;
                float dist = sqrt(dx*dx + dy*dy + dz*dz);
                if (dist < 0.15) {
                    overlap_factor += 0.3 * (1.0 - dist/0.15);
                }
            }
            intensity *= fmin(overlap_factor, 2.0);
            
            DrawSpore(s->x, s->y, s->z, s->size, intensity);
        }
    }
    
    // Restore state
    if (restore_lighting) {
        glDisable(GL_LIGHTING);
    }
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    
    // Reset emission
    float no_emission[] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_emission);
}

/*
 * Cleanup
 */
void CleanupSporeClouds() {
    // Nothing to cleanup (no dynamic allocation)
}