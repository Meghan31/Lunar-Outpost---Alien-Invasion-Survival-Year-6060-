#ifndef HEALTHKIT_H
#define HEALTHKIT_H

#ifdef __cplusplus
extern "C" {
#endif

// Health-kit scale multiplier (can be modified via 'k'/'K' keys)
extern double healthkitScaleUp;

// Health-kit rotation angle (for spinning effect)
extern double healthkitRotation;

/*
 *  Health-kit (white suitcase with red cross)
 *     at (x,y,z)
 *     scale s
 */
void healthkit(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* HEALTHKIT_H */
