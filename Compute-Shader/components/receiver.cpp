#include "receiver.h"

Receiver::Receiver(GLFWwindow *window, Entities<Orientation> &instance, unsigned int &shader_program):
    shader_program(shader_program),
    instances(instance)
{
    this->window = window;
    unsigned int shader = shader_program;
    glUseProgram(shader);
    anterior = glGetUniformLocation(shader, "forwards");            // Front
    dextral = glGetUniformLocation(shader, "right");                // Right
    zenith = glGetUniformLocation(shader, "up");                    // Up
    alignment = glGetUniformLocation(shader, "cameraPos");          // Position
}

bool Receiver::update(float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { return true; }

    Orientation& instance = instances.instantiations[0];

    glm::vec3& position = instance.position;
    glm::vec3& direction = instance.direction;

    float theta = glm::radians(direction.z);
    float phi = glm::radians(direction.y);

    glm::vec3& right = instance.right;
    glm::vec3& up = instance.up;
    glm::vec3& forward = instance.forward;

    forward = {
        glm::cos(theta) * glm::cos(phi),
        glm::sin(theta) * glm::cos(phi),
        glm::sin(phi)
    };
    right = glm::normalize(glm::cross(forward, global_up));
    up = glm::normalize(glm::cross(right, forward));

    // Keyboard Input
    glm::vec3 d_pos = { 0.0f, 0.0f, 0.0f };

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { d_pos.y -= 1.0f; }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { d_pos.x += 1.0f; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { d_pos.x -= 1.0f; }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { d_pos.y += 1.0f; }

    if (glm::length(d_pos) > 0.1f) {
        d_pos = glm::normalize(d_pos);

        position += 1.0f * dt * d_pos.x * forward;
        position += 1.0f * dt * d_pos.y * right;
    }

    // Mouse Events
    glm::vec3 d_dir = { 0.0f, 0.0f, 0.0f };
    double mouse_x, mouse_y;
    
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    glfwSetCursorPos(window, 320.0, 240.0);
    glfwPollEvents();

    d_dir.z = -0.1f * static_cast<float>(mouse_x - 320.0);
    d_dir.y = -0.1f * static_cast<float>(mouse_y - 240.0);

    direction.y = fminf(89.0f, fmaxf(-89.0f, direction.y + d_dir.y));

    direction.z += d_dir.z;
    if (direction.z > 360) { direction.z -= 360; }
    else if (direction.z < 0) { direction.z += 360; }

    // Update Camera
    glUseProgram(shader_program);
    glUniform3fv(anterior, 1, glm::value_ptr(forward));
    glUniform3fv(dextral, 1, glm::value_ptr(right));
    glUniform3fv(zenith, 1, glm::value_ptr(up));
    glUniform3fv(alignment, 1, glm::value_ptr(position));

    system("clear");
    std::cout << instance << std::endl;

    return false;
}