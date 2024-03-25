#include "transmission.h"

#include <GL/glew.h>

Transmission::Transmission(unsigned int apparition, std::vector<unsigned int> &shader_modules):
    shader_modules(shader_modules),
    apparition(apparition)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

Transmission::~Transmission() { glDeleteVertexArrays(1, &VAO); }

void Transmission::update() {
    glUseProgram(shader_modules[0]);
    glBindTexture(GL_TEXTURE_2D, apparition);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}