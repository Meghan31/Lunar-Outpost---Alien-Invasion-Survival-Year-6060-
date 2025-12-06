#ifndef GRENADE_H
#define GRENADE_H

#ifdef __cplusplus
extern "C" {
#endif

// Grenade scale multiplier (can be modified via 'j'/'J' keys)
extern double grenadeScaleUp;

/*
 *  Draw a grenade
 *     at (x,y,z)
 *     scale s
 */
void grenade(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* GRENADE_H */
