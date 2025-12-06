#ifndef OLDCAR_H
#define OLDCAR_H

#ifdef __cplusplus
extern "C" {
#endif

// Car scale multiplier (can be modified via 'c'/'C' keys)
extern double carScaleUp;

/*
 *  Old abandoned car 
 *     at (x,y,z)
 *     scale s
 */
void old_car(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* OLDCAR_H */
