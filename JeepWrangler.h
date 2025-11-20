/*
 *  JeepWrangler.h
 */
#ifndef JEEP_WRANGLER_H
#define JEEP_WRANGLER_H

// Draw realistic Jeep Wrangler vehicle
// x, y, z: position
// scale: size multiplier
// facing: rotation angle (degrees)
// time: animation time (for wheel rotation)
void JeepWrangler(double x, double y, double z, double scale,
                 double facing, double time);

#endif