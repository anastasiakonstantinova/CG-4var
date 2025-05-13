#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glfw3.h>

#include "headers/shader.h"
#include "headers/model.h"

// --- Global var for camera ---
glm::vec3 cameraPos = glm::vec3(-3.6f, 3.0f, 10.4f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.65f, -0.03f, -0.76f));
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightPos = glm::vec3(5.0f, 3.0f, 5.0f);

// main model light reflections config
const glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
const glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);   // Bright white light
const glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
const float intensity = 7.0f;                            // intensity

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

void drawMainModel(Shader shader, Model modelObj, glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
    glm::vec3 lightPosView = glm::vec3(view * glm::vec4(lightPos, 1.0f));

    // load projection
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);
    shader.setMat4("normalMatrix", normalMatrix);

    // material set
    shader.setVec3("material.ambient", 0.24725f, 0.1995f, 0.0745f);
    shader.setVec3("material.diffuse", 0.75164f, 0.60648f, 0.22648f);
    shader.setVec3("material.specular", 0.628281f, 0.555802f, 0.366065f);
    shader.setFloat("material.shininess", 51.2f);

    // light set
    shader.setVec3("light.position", lightPos);
    shader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
    shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);  // Яркий белый свет
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("light.intensity", 7.0f);  // Увеличиваем интенсивность

    // Set view pos
    shader.setVec3("viewPos", cameraPos);

    // render model
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMat4("model", model);

    modelObj.Draw(shader);
}

void drawLightCube(Shader shader, Model model, glm::mat4 projection, glm::mat4 view) {
    // light Cube
    shader.use();
    glm::mat4 cubeLightModel = glm::mat4(1.0f);
    cubeLightModel = glm::translate(cubeLightModel, lightPos);
    cubeLightModel = glm::scale(cubeLightModel, glm::vec3(0.2f));

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", cubeLightModel);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    model.Draw(shader);
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

    Shader lightCubeShader("shaders/lightCubeShader.vert", "shaders/lightCubeShader.frag");
    Model lightCubeModel("resources/models/cube.obj");

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ---- MAIN ----
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        printf("x: %f, y: %f, z: %f\n", cameraPos.x, cameraPos.y, cameraPos.z);
        //printf("Light pos: %.2f, %.2f, %.2f\n", lightPos.x, lightPos.y, lightPos.z);=

        // bg color: 167, 139, 113 (A78B71)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

        drawMainModel(shaderModel, modelObj, projection, view, model);

        drawLightCube(lightCubeShader, lightCubeModel, projection, view);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
