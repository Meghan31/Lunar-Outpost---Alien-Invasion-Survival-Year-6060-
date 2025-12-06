// /*
//  * Helicopter.h
//  * Header file for realistic helicopter model
//  */
// #ifndef HELICOPTER_H
// #define HELICOPTER_H

// #ifdef __cplusplus
// extern "C" {
// #endif

// /*
//  * Set textures for helicopter (call from main program after loading textures)
//  */
// void SetHelicopterTextures(unsigned int bodyTex, unsigned int backTex, unsigned int metalTex, unsigned int glassTex);

// /*
//  * Draw a realistic helicopter
//  * Parameters:
//  *   x, y, z   - Position
//  *   dx, dy, dz - Scale in each dimension
//  *   th        - Rotation about y-axis (yaw)
//  *   rotorAngle - Main rotor rotation angle
//  *   tailRotorAngle - Tail rotor rotation angle
//  */
// void DrawHelicopter(double x, double y, double z,
//                    double dx, double dy, double dz,
//                    double th,
//                    double rotorAngle,
//                    double tailRotorAngle);

// #ifdef __cplusplus
// }
// #endif

// #endif

/*
 *  Helicopter.h
 *  Realistic Helicopter Model (EC135-style)
 */
#ifndef HELICOPTER_H
#define HELICOPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Draw a realistic helicopter
 * x, y, z: position
 * scale: size multiplier
 * facing: rotation angle (degrees) around Y axis
 * time: animation time (for rotor rotation)
 * rotorSpeed: main rotor rotation speed multiplier
 * engineOn: 0=off (rotors still), 1=on (rotors spinning)
 */
void Helicopter(double x, double y, double z, double scale,
                double facing, double time, double rotorSpeed, int engineOn);

/*
 * Initialize helicopter textures
 * Call this once before rendering helicopters
 */
void InitHelicopterTextures(void);

#ifdef __cplusplus
}
#endif

#endif /* HELICOPTER_H */