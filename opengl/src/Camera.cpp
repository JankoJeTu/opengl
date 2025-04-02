#include "Camera.h"

Camera2D::Camera2D()
    : position(0.0f, 0.0f), zoom(1.0f), moveSpeed(0.05f), zoomSpeed(0.005f), minZoom(0.5f), maxZoom(2.0f) {

    projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    Update();
}

void Camera2D::Update()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(-position, 0.0f));
    transform = glm::scale(transform, glm::vec3(zoom, zoom, 1.0f));
    viewMatrix = transform;
}

void Camera2D::ProcessInput(GLFWwindow* window)
{
    bool moved = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position.y += moveSpeed; moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position.y -= moveSpeed; moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position.x -= moveSpeed; moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position.x += moveSpeed; moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        zoom += zoomSpeed; moved = true;
        if (zoom >= maxZoom)
            zoom = maxZoom;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        zoom -= zoomSpeed; moved = true;
        if (zoom <= minZoom)
            zoom = minZoom;
    }

    if (moved) Update();
}

glm::mat4 Camera2D::getCameraMatrix()
{
    return projectionMatrix * viewMatrix;
}
