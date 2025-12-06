#ifndef HUMAN_H
#define HUMAN_H

#ifdef __cplusplus
extern "C" {
#endif

// Human scale multiplier (can be modified via 'h'/'H' keys)
extern double humanScaleUp;

/*
 *  Human figure
 *     at (x,y,z)
 *     scale s
 */
void human(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* HUMAN_H */
