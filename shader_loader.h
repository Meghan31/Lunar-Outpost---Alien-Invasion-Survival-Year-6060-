/*
 * Shader Loader Utility
 * Handles loading, compiling, and linking GLSL shaders
 */

#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

// Function prototypes

// Load shader source code from file
char* LoadShaderFile(const char* filename);

// Compile a shader from source
unsigned int CompileShader(const char* source, unsigned int shaderType);

// Create a complete shader program from vertex and fragment shaders
unsigned int CreateShaderProgram(const char* vertexPath, const char* fragmentPath);

// Delete shader program
void DeleteShaderProgram(unsigned int program);

// Print shader compilation errors
void PrintShaderLog(unsigned int shader);

// Print program linking errors
void PrintProgramLog(unsigned int program);

#endif
