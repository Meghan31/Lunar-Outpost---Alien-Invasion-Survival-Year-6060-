/*
 *  GeometricAlienFlora.h
 */
#ifndef GEOMETRIC_ALIEN_FLORA_H
#define GEOMETRIC_ALIEN_FLORA_H

// Draw a single alien plant
void GeometricAlienFlora(double x, double y, double z, double scale, 
                        double time, int colorVariant, int seed);

// Draw a cluster of plants
void GeometricAlienFloraCluster(double centerX, double centerY, double centerZ,
                               double clusterRadius, int count, double time);

#endif
