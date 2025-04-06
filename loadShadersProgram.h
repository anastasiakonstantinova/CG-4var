#pragma once
#include <fstream>
#include <sstream>
#include <glew.h>
#include <glfw3.h>

// func for load shaders from file
std::string loadShader(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        fprintf(stderr, "ERROR: error load sheder file:\n %s", filePath);
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// func for compile shaders
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: error comile shader:\n %s", infoLog);
    }
    return shader;
}

// link errors funcs 
void linkErrorCatch(GLuint shaderProgram) {
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: error link programm:\n %s", infoLog);
    }
}
