#ifndef TARGET_H
#define TARGET_H

#ifdef __cplusplus
extern "C" {
#endif

// Target scale multiplier (can be modified via 'g'/'G' keys)
extern double targetScaleUp;

/*
 *  Archery target
 *     at (x,y,z)
 *     scale s
 */
void target(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* TARGET_H */
