#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Cylinder (for tree trunk, table legs, etc)
 *     at (x,y,z)
 *     radius r, height h
 *     with n segments around circumference
 */
void cylinder(double x, double y, double z, double r, double h, int n);

/*
 *  Cone (for tree foliage)
 *     at (x,y,z)
 *     base radius r, height h
 *     with n segments around base
 */
void cone(double x, double y, double z, double r, double h, int n);

/*
 *  Sphere (for human head, hands, feet)
 *     at (x,y,z)
 *     radius r
 */
void sphere(double x, double y, double z, double r);

#ifdef __cplusplus
}
#endif

#endif /* PRIMITIVES_H */
