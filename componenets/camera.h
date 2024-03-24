#pragma once
#include <glm/glm.hpp>

struct Orientation {
    glm::vec3 position;
    glm::vec3 eulers;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;
}