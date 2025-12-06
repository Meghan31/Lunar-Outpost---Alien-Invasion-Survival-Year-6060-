#ifndef BROKENTABLE_H
#define BROKENTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

// Table scale multiplier (can be modified via 'b'/'B' keys)
extern double tableScaleUp;

/*
 *  Broken table (two pieces fallen on ground)
 *     at (x,y,z)
 *     scale s
 */
void broken_table(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* BROKENTABLE_H */
