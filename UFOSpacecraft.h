/*
 *  UFOSpacecraft.h
 */
#ifndef UFO_SPACECRAFT_H
#define UFO_SPACECRAFT_H

// Draw UFO spacecraft
// x, y, z: position
// scale: size multiplier
// time: animation time
// rotationSpeed: rotation speed (degrees/second)
// tractorBeamOn: 0=off, 1=on
void UFOSpacecraft(double x, double y, double z, double scale, 
                   double time, double rotationSpeed, int tractorBeamOn);

#endif
