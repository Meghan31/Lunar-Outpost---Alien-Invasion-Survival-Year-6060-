/*
 *  BioluminescentSporeCloud.h
 *  Header for bioluminescent spore cloud object
 */
#ifndef BIOLUMINESCENT_SPORE_CLOUD_H
#define BIOLUMINESCENT_SPORE_CLOUD_H

// Opaque cloud structure (implementation in .c file)
typedef struct SporeCloud SporeCloud;

// Create a new spore cloud
SporeCloud* CreateSporeCloud(float radius, int sporeCount);

// Draw the spore cloud
void BioluminescentSporeCloud(double x, double y, double z, 
                              double size, 
                              SporeCloud* cloud,
                              double time);

// Free spore cloud memory
void FreeSporeCloud(SporeCloud* cloud);

#endif
