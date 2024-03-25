#include "./reflection.h"

unsigned int Reflection::coalesce(GLFWwindow* window) {
    unsigned int color_buffer;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glGenTextures(1, &color_buffer);
    glBindTexture(GL_TEXTURE_2D, color_buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    return color_buffer;
}