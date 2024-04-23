#pragma once

#include "modules/world/map.h"
#include "modules/gui/camera.h"
#include "components/utility/logger.h"

#include <vector>
#include <set>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class CaveGeneration {
    public:
        CaveGeneration();

        const int screenHeight = 1080;
        const int screenWidth = 1640;
        const float verticalFOV = 60.0f;
        const float horizontalFOV = 2.0f * atan(tan(verticalFOV * 0.5f) * (screenWidth / screenHeight));
        const float halfFOV = horizontalFOV * 0.5f;
        const DebugLevel debugLevel = DebugLevel::Verbose;

        bool initGLFW();
        bool initBuffers();
        void render();

        // Return the camera from a static function
        static Camera camera;

    private:
        World world;
        GLFWwindow* window;
        std::set<glm::ivec3, Vec3Compare> current_chunks;
        std::map<glm::ivec3, unsigned int, Vec3Compare> chunk_buffers;
        unsigned int shader;

        void playerControls();
        void MVP();
        void imguiInit();
        void imgui(bool);
        void imguiDestroy();
        void cleanUp();
        void updateWorld();
};