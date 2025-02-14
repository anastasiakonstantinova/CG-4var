#include <glew.h>
#include <glfw3.h>
#include <iostream>


int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

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
    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.5f, 0.0f);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
