#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

const int WIDTH = 1800;
const int HEIGHT = 1200;

int main() {
    // Initializing GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to Initialize GLFW. Wtf yu durn fam?!\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(WIDTH, HEIGHT, "Back At the OpenGL", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW Window. Why isn't this working?!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to Initialize GLEW. Fuck!\n");
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Working with Shaders
    std::ifstream file;
    std::stringstream bufferedLines;
    std::string line;

    file.open("/home/persist/z/Documents/repos/learning/C/OpenGL/shader/vertex.glsl");
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    // Main OpenGL Render Loop
    do {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0
    );

    glfwTerminate();
    return 0;
}