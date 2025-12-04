#ifndef ASTRONAUT_H
#define ASTRONAUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Main astronaut rendering function
 */
void Astronaut(double x, double y, double z, double scale, double rotation, double time);

/*
 * Initialize astronaut textures
 */
// void InitAstronautTextures(void);

#ifdef __cplusplus
}
#endif

#endif /* ASTRONAUT_H */
