#ifndef HOUSE_H
#define HOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

// House scale multiplier (can be modified via 'y'/'Y' keys)
extern double houseScaleUp;

/*
 *  House (cabin-type)
 *     at (x,y,z)
 *     scale s
 */
void house(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* HOUSE_H */
