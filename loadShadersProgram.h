#pragma once
#include <glew.h>
#include <glfw3.h>

const char* vertShader =
"#version 460 core\n"
"layout (location = 0) in vec3 vp;"
"void main() {"
"   gl_Position = vec4(vp, 1.0);"
"}\0";

const char* fragShader =
"#version 460 core\n"
"out vec4 FragColor;"
"uniform vec4 ourColor;"
"void main() {"
"   FragColor = ourColor;"
"}\0";


// funcs for compile shaders
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
