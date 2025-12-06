#ifndef BOULDER_H
#define BOULDER_H

#ifdef __cplusplus
extern "C" {
#endif

// Boulder scale multiplier (can be modified via 'r'/'R' keys)
extern double boulderScaleUp;

/*
 *  Stone boulder
 *     at (x,y,z)
 *     scale s
 */
void boulder(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* BOULDER_H */
