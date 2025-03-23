#include <iostream>
#include "loadShadersProgram.h"


int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(512, 512, "Main window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glewExperimental = GL_TRUE;
    glfwMakeContextCurrent(window);
    GLenum ret = glewInit();
    if (GLEW_OK != ret) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
        return 1;
    }

    // square points and idxs
    GLfloat points[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // create config
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // fill VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // fill EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // config attr vertuces
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragShader);

    // Create shader's programm 
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    linkErrorCatch(shaderProgram);

    // del shaders
    glDeleteShader(vs);
    glDeleteShader(fs);

    // ---- MAIN ----
    while (!glfwWindowShouldClose(window)) {

        // bg color: 167, 139, 113 (A78B71)
        glClearColor(0.6549f, 0.5451f, 0.4431f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // figure color: 247, 200, 21 (F7C815)
        float time = glfwGetTime();
        GLint colorLoc = glGetUniformLocation(shaderProgram, "ourColor");
        float r = (sin(time) / 2.0f) + .5f;
        float g = (cos(time) / 2.0f) + .5f;
        float b = .5f;
        glUniform4f(colorLoc, r, g, b, 1.0f);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
