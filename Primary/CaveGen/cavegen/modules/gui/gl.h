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

static void gl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    { fprintf(stderr, "GL ERR: %s\n", message); }

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    { glViewport(0, 0, width, height); }

static inline void bindObjectBuffer(unsigned int &arrayObject, Chunk &MapChunk, unsigned int shader)
    {
        Logger::Verbose(" - Binding Object Buffer - VAO: %d\n", arrayObject);
        //MapChunk.log();
        unsigned int EBO, VBO;

        glGenVertexArrays(1, &arrayObject);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(arrayObject);

        // bind vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, MapChunk.vertices.size() * sizeof(Vertex), MapChunk.vertices.data(), GL_STATIC_DRAW);

        // bind index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MapChunk.indices.size() * sizeof(unsigned int), MapChunk.indices.data(), GL_STATIC_DRAW);

        // position attribute
        const static GLint posAttr = glGetAttribLocation(shader, "aPos");
        glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(posAttr);

        // color attribute
        const static GLint colAttr = glGetAttribLocation(shader, "aCol");
        glVertexAttribPointer(colAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(colAttr);

        // normal attribute
        const static GLint normAttr = glGetAttribLocation(shader, "aNorm");
        glVertexAttribPointer(normAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(normAttr);

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