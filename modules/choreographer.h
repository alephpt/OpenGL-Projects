#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../components/instance.h"
#include "../components/receiver.h"
#include "../components/transmission.h"
#include "../components/reflection.h"
#include "../utilities/shader.h"


// The Choreographer is the main application State

/*
    Sights and sounds entwine,
    Echoes dance with colors bold,
    Essence reflected.
        - Richard I. Christopher, GPT3; 2024-03-24
*/

class World {
    public:
        World();
        ~World();

        void persist();
        void initGL();
        void createChannel();
        void logWorkGroupInfo();

        Entities<Orientation> instance;
    
    private:
        void initGLFW();
        void sync();

        GLFWwindow* window;
        const int WIDTH = 1800;
        const int HEIGHT = 1200;

        std::vector<unsigned int> shader_modules;
        unsigned int apparition;

        Receiver* voyager;
        Transmission* manifestation;

        double prev_T, curr_T;
        int n_frames;
        float frame_T;
};