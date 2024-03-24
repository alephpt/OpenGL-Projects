#include <stdio.h>
#include <stdlib.h>
#include "./utilities/shader.h"

int main() {
    GLFWwindow* window;
    const int WIDTH = 1800;
    const int HEIGHT = 1200;

    if(init(window, WIDTH, HEIGHT) == -1) { return -1; }
    window = glfwCreateWindow(WIDTH, HEIGHT, "Back At the OpenGL", NULL, NULL);
    if (checkWindow(window) == -1) { return -1; }
    glfwMakeContextCurrent(window);
    if (initGlew() == -1) { return -1; }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    initShaders();

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