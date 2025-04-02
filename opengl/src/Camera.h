#pragma once

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class Camera2D {

private:
    glm::mat4 projectionMatrix;

public:
    glm::vec2 position;
    float zoom;
    glm::mat4 viewMatrix;

    float moveSpeed;
    float zoomSpeed;
    float minZoom;
    float maxZoom;

    Camera2D();

    void Update();

    void ProcessInput(GLFWwindow* window);

    glm::mat4 getCameraMatrix();
};
