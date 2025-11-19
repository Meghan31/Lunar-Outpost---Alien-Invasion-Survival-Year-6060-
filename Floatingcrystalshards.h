/*
 *  FloatingCrystalShards.h
 */

#ifndef FLOATING_CRYSTAL_SHARDS_H
#define FLOATING_CRYSTAL_SHARDS_H


void FloatingCrystalShard(double x, double y, double z,
                          double scale, double time,
                          int seed, int drawTrails,
                          double facingCamera);


void FloatingCrystalCluster(double centerX, double centerY, double centerZ,
                           double clusterRadius, int numCrystals,
                           double time, int drawTrails, double facingCamera);



#endif // FLOATING_CRYSTAL_SHARDS_H