#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../components/instance.h"
#include "../components/receiver.h"
#include "../components/transmission.h"
#include "../utilities/shader.h"
#include "./reflection.h"


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

        unsigned int shader_program;
        unsigned int apparition;

        Receiver* witness;
        Transmission* manifestation;

        double prev_T, curr_T;
        int n_frames;
        float frame_T;
};