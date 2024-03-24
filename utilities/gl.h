#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

void printWorkGroupInfo() {
    int workGroupSizes[3] = { 0 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSizes[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSizes[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSizes[2]);
    int workGroupCounts[3] = { 0 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);

    std::cout << "Max Compute Workgroup Invocatios: " << GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "Workgroup Size" << workGroupSizes[i] << std::endl;
        std::cout << "Workgroup Count" << workGroupCounts[i] << std::endl;
    }
}

// Initializing GLFW
int init(GLFWwindow* window, const int WIDTH, const int HEIGHT) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to Initialize GLFW. Wtf yu durn fam?!\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return 1;
}

// Checking the state of window creation
int checkWindow(GLFWwindow* window) {
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW Window. Why isn't this working?!\n");
        glfwTerminate();
        return -1;
    }
    return 1;
}

// Initialize Glew or bust
int initGlew() {
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to Initialize GLEW. Fuck!\n");
        return -1;
    }
    return 1;
}

// Create Shader Module
unsigned int createShader(unsigned int mod_type, const char* shader) {
    unsigned int shader_mod = glCreateShader(mod_type);
    glShaderSource(shader_mod, 1, &shader, NULL);
    glCompileShader(shader_mod);

    int shader_created;
    glGetShaderiv(shader_mod, GL_COMPILE_STATUS, &shader_created);

    if (!shader_created) {
        char err_log[1024];
        glGetShaderInfoLog(shader_mod, 1024, NULL, err_log);
        std::cout << "HEY!! - Your Shader Module Failed to Compile:\n" << err_log << std::endl;
    }

    return shader_mod;
}

// Create Shader Program
unsigned int createShaderProgram(const std::vector<unsigned int>& shader_modules) {
    unsigned int shader_program = glCreateProgram();

    for (unsigned int mod : shader_modules) {
        glAttachShader(shader_program, mod);
    }

    glLinkProgram(shader_program);

    int program_created;
    glGetProgramiv(shader_program, GL_COMPILE_STATUS, &program_created);

    if (!program_created) {
        char err_log[1024];
        glGetProgramInfoLog(shader_program, 1024, NULL, err_log);
        std::cout << "HEY!! - Your Shader Program Failed to Build:\n" << err_log << std::endl;
    }

    for (unsigned int mod : shader_modules) {
        glDeleteShader(mod);
    }

    return shader_program;
}