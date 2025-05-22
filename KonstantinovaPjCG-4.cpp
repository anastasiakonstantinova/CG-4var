#include <iostream>
#include <chrono>

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

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --- mouse config ---
float lastX = 512.0f / 2, lastY = 512.0f / 2;
float yaw = glm::degrees(atan2(cameraFront.z, cameraFront.x)), pitch = glm::degrees(asin(cameraFront.y));
bool firstMouse = true;
float sensitivity = 0.1f;

const int screenResolution = 720;

struct ObjectTransform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 pivotPoint = glm::vec3(0.0f);

    struct {
        float min = -90.0f;
        float max = 90.0f;
    } rotationLimit;
};

std::vector<ObjectTransform> objectTransforms;

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
}

glm::mat4 calculateModelMatrix(int index) {
    glm::mat4 model = glm::mat4(1.0f);

    if (index == 0) {
        return model;
    }

    if (index == 1) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);
        return model;
    }

    if (index == 2) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);

        model = glm::translate(model, objectTransforms[2].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[2].rotation.z), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[2].pivotPoint);
        return model;
    }

    if (index == 3) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);

        model = glm::translate(model, objectTransforms[2].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[2].rotation.z), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[2].pivotPoint);

        model = glm::translate(model, objectTransforms[3].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[3].rotation.z), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[3].pivotPoint);
        return model;
    }
    return model;
}

void processInput(GLFWwindow* window) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float cameraSpeed = 2.5f * deltaTime;
    float rotateSpeed = 50.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraRight * cameraSpeed;

    // Model rotation controls
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        objectTransforms[1].rotation.z += rotateSpeed;
        objectTransforms[1].rotation.z = glm::clamp(objectTransforms[1].rotation.z,
            objectTransforms[1].rotationLimit.min,
            objectTransforms[1].rotationLimit.max);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        objectTransforms[1].rotation.z -= rotateSpeed;
        objectTransforms[1].rotation.z = glm::clamp(objectTransforms[1].rotation.z,
            objectTransforms[1].rotationLimit.min,
            objectTransforms[1].rotationLimit.max);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        objectTransforms[2].rotation.z += rotateSpeed;
        objectTransforms[2].rotation.z = glm::clamp(objectTransforms[2].rotation.z,
            objectTransforms[2].rotationLimit.min,
            objectTransforms[2].rotationLimit.max);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        objectTransforms[2].rotation.z -= rotateSpeed;
        objectTransforms[2].rotation.z = glm::clamp(objectTransforms[2].rotation.z,
            objectTransforms[2].rotationLimit.min,
            objectTransforms[2].rotationLimit.max);
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        objectTransforms[3].rotation.z += rotateSpeed;
        objectTransforms[3].rotation.z = glm::clamp(objectTransforms[3].rotation.z,
            objectTransforms[3].rotationLimit.min,
            objectTransforms[3].rotationLimit.max);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        objectTransforms[3].rotation.z -= rotateSpeed;
        objectTransforms[3].rotation.z = glm::clamp(objectTransforms[3].rotation.z,
            objectTransforms[3].rotationLimit.min,
            objectTransforms[3].rotationLimit.max);
    }
}

void drawModel(Shader& shader, Model& modelObj, glm::mat4 projection, glm::mat4 view) {
    shader.use();

    // Настройка материалов
    shader.setVec3("material.ambient", 0.24725f, 0.1995f, 0.0745f);
    shader.setVec3("material.diffuse", 0.75164f, 0.60648f, 0.22648f);
    shader.setVec3("material.specular", 0.628281f, 0.555802f, 0.366065f);
    shader.setFloat("material.shininess", 51.2f);

    // Настройка освещения
    glm::vec3 lightPos = glm::vec3(5.0f, 3.0f, 5.0f);
    shader.setVec3("light.position", lightPos);
    shader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
    shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("light.intensity", 2.0f);

    // Позиция камеры
    shader.setVec3("viewPos", cameraPos);

    // Матрицы проекции и вида
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    // Обновляем трансформации для всех мешей
    for (size_t i = 0; i < modelObj.meshTransforms.size(); ++i) {
        modelObj.meshTransforms[i] = calculateModelMatrix(i);
    }

    // Рендерим модель с уже обновленными трансформациями
    modelObj.Draw(shader);
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

    // Загружаем шейдер
    Shader modelShader("shaders/shader.vert", "shaders/shader.frag");

    // Загружаем модель
    Model modelObj("resources/models/model.obj");

    // Инициализация трансформаций
    objectTransforms.resize(4);
    objectTransforms[1].pivotPoint = glm::vec3(1.8f, 0.0f, 1.7f);
    objectTransforms[2].pivotPoint = glm::vec3(0.0f, 1.9f, 2.55f);
    objectTransforms[3].pivotPoint = glm::vec3(0.0f, 3.746f, 2.545f);

    objectTransforms[1].rotationLimit = { -90.0f, 90.0f };
    objectTransforms[2].rotationLimit = { -25.0f, 60.0f };
    objectTransforms[3].rotationLimit = { -90.0f, 90.0f };

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Главный цикл рендеринга
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Очистка экрана
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Настройка матриц проекции и вида
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

        // Рендеринг модели
        drawModel(modelShader, modelObj, projection, view);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}