#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glfw3.h>

#include "headers/shader.h"
#include "headers/model.h"

// --- Global var for camera ---
glm::vec3 cameraPos = glm::vec3(-3.0f, 3.0f, 8.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.65f, -0.03f, -0.76f));
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// --- mouse config ---
float lastX = 512.0f / 2, lastY = 512.0f / 2;
float yaw = glm::degrees(atan2(cameraFront.z, cameraFront.x)), pitch = glm::degrees(asin(cameraFront.y));
bool firstMouse = true;
float sensitivity = 0.1f;

const int screenResolution = 720;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    //printf("x: %f\ty: %f\tz: %f\n", front.x, front.y, front.z);
    //printf("x: %f\ty: %f\tz: %f\n", cameraFront.x, cameraFront.y, cameraFront.z);
}

void updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    static float cameraSpeed = .0007f;
    const float rotationSpeed = .01f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

    // --- Camera movement ---
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraRight * cameraSpeed;
}


int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenResolution, screenResolution, "Main window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);

    Shader shaderModel("shaders/shader.vert", "shaders/shader.frag");
    Model modelObj("resources/models/model.obj");

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ---- MAIN ----
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // bg color: 167, 139, 113 (A78B71)
        glClearColor(0.6549f, 0.5451f, 0.4431f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Матрицы проекции
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)screenResolution / (float)screenResolution,
            0.1f,
            100.0f
        );
        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );
        glm::mat4 model = glm::mat4(1.0f);

        // load projection
        shaderModel.setMat4("projection", projection);
        shaderModel.setMat4("view", view);

        // render model 
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shaderModel.setMat4("model", model);
        
        // change color
        float time = glfwGetTime();
        float r = (sin(time) / 2.0f) + .5f;
        float g = (cos(time) / 2.0f) + .5f;
        float b = .5f;
        shaderModel.setVec3("ourColor", r, g, b);

        shaderModel.use();
        modelObj.Draw(shaderModel);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
