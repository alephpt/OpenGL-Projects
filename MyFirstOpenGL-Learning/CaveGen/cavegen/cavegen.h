#pragma once

#include "./modules/world/world.h"
#include "./modules/gui/camera.h"

#include <vector>
#include <set>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class CaveGeneration {
    public:
        CaveGeneration();

        const int screenHeight = 720;
        const int screenWidth = 1260;

        bool initGLFW();
        bool initBuffers();
        void render();

        // Return the camera from a static function
        static Camera camera;

    private:
        World Map;
        GLFWwindow* window;
        std::vector<unsigned int> VAOs;
        std::set<glm::ivec3, Vec3Compare> current_chunks;
        unsigned int shader;
        bool show_window = true;

        void userInput();
        void MVP();
        void imguiInit();
        void imgui(bool);
        void imguiRender();
        void imguiDestroy();
        void cleanUp();
};