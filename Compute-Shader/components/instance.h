#pragma once
#include <glm/glm.hpp>
#include <ostream>

// Can turn this into a class and do the logic here
struct Orientation {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    friend std::ostream& operator<<(std::ostream& out, const Orientation& instance);
};

inline std::ostream& operator<<(std::ostream& out, const Orientation& instance) {
    out << "Position: " << instance.position.x << ", " << instance.position.x << ", " << instance.position.x << ", " << std::endl;
    out << "Direction: " << instance.direction.x << ", " << instance.direction.x << ", " << instance.direction.x << ", " << std::endl;
    out << "Forward: " << instance.forward.x << ", " << instance.forward.x << ", " << instance.forward.x << ", " << std::endl;
    out << "Right: " << instance.right.x << ", " << instance.right.x << ", " << instance.right.x << ", " << std::endl;
    out << "Up: " << instance.up.x << ", " << instance.up.x << ", " << instance.up.x << ", " << std::endl;
    return out;
}