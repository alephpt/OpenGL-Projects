#pragma once
#include "../utilities/entities.h"
#include "./instance.h"

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
        unsigned int anterior;              // Front
        unsigned int dextral;               // Right
        unsigned int zenith;                // Up
        unsigned int alignment;             // Position
        float dx, dy, dz;
};