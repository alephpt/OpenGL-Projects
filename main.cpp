#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const int WIDTH = 1800;
const int HEIGHT = 1200;

int main() {
    glewExperimental = true;  // Do we need this here?

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
    glewExperimental = true; // Do we need this again?

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to Initialize GLEW. Fuck!\n");
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0
    );

    return 0;
}