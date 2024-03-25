#include "transmission.h"

#include <GL/glew.h>

Transmission::Transmission(unsigned int apparition, unsigned int& shader_program):
    shader_program(shader_program),
    apparition(apparition)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

Transmission::~Transmission() { glDeleteVertexArrays(1, &VAO); }

void Transmission::update() {
    glUseProgram(shader_program);
    glBindTexture(GL_TEXTURE_2D, apparition);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}