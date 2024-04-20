#pragma once

#include "../world/chunk.h"
#include "./shader.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static void glfw_error_callback(int error, const char* description)
    { fprintf(stderr, "GLFW ERR: %d: %s\n", error, description); }

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    { glViewport(0, 0, width, height); }

// generates and binds Object and Vertex Array buffers, and populates buffer data with Object Vertices
static inline void bindObjectBuffer(unsigned int &arrayObject, Chunk &MapChunk)
    {
        unsigned int EBO, VBO[3];

        glGenVertexArrays(1, &arrayObject);
        glGenBuffers(3, VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(arrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, MapChunk.vertices.size() * sizeof(float), &MapChunk.vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MapChunk.indices.size() * sizeof(int), &MapChunk.indices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, MapChunk.normals.size() * sizeof(float), &MapChunk.normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, MapChunk.colors.size() * sizeof(float), &MapChunk.colors[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


// calls the shader source code, parsing and creating shader program
unsigned int ShaderData()
    {
        unsigned int program = glCreateProgram();
        ShaderSource source = parseShader(shader_path);
        unsigned int shader = createShader(source.VertexSource, source.FragmentSource, program);
        return shader;
    }