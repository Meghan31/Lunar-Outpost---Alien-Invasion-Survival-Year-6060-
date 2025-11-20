/*
 * Shader Loader Utility Implementation
 * Handles loading, compiling, and linking GLSL shaders
 * Took help from AI
 */

#include "shader_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load shader source code from file
char* LoadShaderFile(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open shader file %s\n", filename);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Cannot allocate memory for shader\n");
        fclose(file);
        return NULL;
    }
    
    // Read file
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';
    
    fclose(file);
    return buffer;
}

// Print shader compilation errors
void PrintShaderLog(unsigned int shader)
{
    int logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    
    if (logLength > 1) {
        char* log = (char*)malloc(logLength);
        glGetShaderInfoLog(shader, logLength, NULL, log);
        fprintf(stderr, "Shader compilation error:\n%s\n", log);
        free(log);
    }
}

// Print program linking errors
void PrintProgramLog(unsigned int program)
{
    int logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    
    if (logLength > 1) {
        char* log = (char*)malloc(logLength);
        glGetProgramInfoLog(program, logLength, NULL, log);
        fprintf(stderr, "Program linking error:\n%s\n", log);
        free(log);
    }
}

// Compile a shader from source
unsigned int CompileShader(const char* source, unsigned int shaderType)
{
    unsigned int shader = glCreateShader(shaderType);
    if (shader == 0) {
        fprintf(stderr, "Error: Cannot create shader\n");
        return 0;
    }
    
    // Compile shader
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    // Check compilation status
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        const char* typeStr = (shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        fprintf(stderr, "Error: Failed to compile %s shader\n", typeStr);
        PrintShaderLog(shader);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

// Create a complete shader program from vertex and fragment shaders
unsigned int CreateShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    // Load shader sources
    char* vertexSource = LoadShaderFile(vertexPath);
    char* fragmentSource = LoadShaderFile(fragmentPath);
    
    if (!vertexSource || !fragmentSource) {
        fprintf(stderr, "Error: Failed to load shader files\n");
        if (vertexSource) free(vertexSource);
        if (fragmentSource) free(fragmentSource);
        return 0;
    }
    
    // Compile shaders
    unsigned int vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
    
    free(vertexSource);
    free(fragmentSource);
    
    if (vertexShader == 0 || fragmentShader == 0) {
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        return 0;
    }
    
    // Create program and link shaders
    unsigned int program = glCreateProgram();
    if (program == 0) {
        fprintf(stderr, "Error: Cannot create shader program\n");
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check linking status
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        fprintf(stderr, "Error: Failed to link shader program\n");
        PrintProgramLog(program);
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    // Clean up individual shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    printf("Successfully created shader program from:\n");
    printf("  Vertex: %s\n", vertexPath);
    printf("  Fragment: %s\n", fragmentPath);
    
    return program;
}

// Delete shader program
void DeleteShaderProgram(unsigned int program)
{
    if (program != 0) {
        glDeleteProgram(program);
    }
}
