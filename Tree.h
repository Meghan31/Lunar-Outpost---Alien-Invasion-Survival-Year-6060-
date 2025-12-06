#ifndef TREE_H
#define TREE_H

#ifdef __cplusplus
extern "C" {
#endif

// Tree scale multiplier (can be modified via 't'/'T' keys)
extern double treeScaleUp;

/*
 *  Tree
 *     at (x,y,z)
 *     scale s
 */
void tree(double x, double y, double z, double s);

#ifdef __cplusplus
}
#endif

#endif /* TREE_H */
