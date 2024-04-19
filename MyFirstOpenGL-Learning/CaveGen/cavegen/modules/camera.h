#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

struct Camera {
    glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.7f, -0.15f, 0.70f);
    glm::vec3 location = glm::vec3(180.8f, -366.87f, 146.8f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 direction;
    const float speed = 1.3f;
    float xstrafe;
    float zstrafe;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX;
    float lastY;
    bool newmouse = true;
    bool bPress = false;
    bool freeMouse = true;
    bool killapp = false;
};
#endif