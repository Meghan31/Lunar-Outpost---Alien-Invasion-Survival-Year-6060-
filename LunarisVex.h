/*
 *  LunarisVex.h
 *  Selunari Moon Alien Character
 */
#ifndef LUNARIS_VEX_H
#define LUNARIS_VEX_H

// Emotion states
#define EMOTION_CALM 0
#define EMOTION_CURIOUS 1
#define EMOTION_ALERT 2
#define EMOTION_THINKING 3

// Draw Lunaris Vex character
// x, y, z: position
// facing: rotation angle (degrees)
// time: animation time
// emotion: 0=calm, 1=curious, 2=alert, 3=thinking
// isGliding: 0=standing, 1=gliding animation
void LunarisVex(double x, double y, double z, double facing, 
                double time, int emotion, int isGliding);

#endif
