#pragma once
#include "../utilities/entities.h"
#include "./instance.h"

#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


// The Receiver is the Camera Controller
class Receiver {
    public:
        Receiver(GLFWwindow* window, Entities<Orientation>& instance, unsigned int &shader_program);
        bool update(float dt);

    private:
        GLFWwindow* window;
        Entities<Orientation>& instances;       
        unsigned int &shader_program;
        const glm::vec3 global_up = {0.0f, 0.0f, 1.0f}; // Z is up so we are oriented for 2D purposes. "Top Down"
        std::vector<unsigned int> position;
        std::vector<unsigned int> vantage;
        unsigned int anterior;  // Front
        unsigned int dextral;   // Right
        unsigned int zenith;    // Up
        float dx, dy, dz;
};