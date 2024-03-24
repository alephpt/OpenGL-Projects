#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gl.h"

unsigned int makeShader( const std::string& filepath, unsigned int mod_type) {
    std::ifstream file;
    std::stringstream buffered_lines;
    std::string line;

    file.open(filepath);
    while (std::getline(file, line)) {
        buffered_lines << line << "\n";
    }

    std::string shader_file = buffered_lines.str();
    const char* shader_source = shader_file.c_str();
    buffered_lines.str("");  // Not Needed but good practice
    file.close();           // Not Needed but good practice

    return createShader(mod_type, shader_source);
}

unsigned int initShaders() {
    std::vector<unsigned int> shader_modules;

    //shader_modules.push_back(makeShader("/home/persist/z/Documents/repos/learning/C/OpenGL/shader/vertex.glsl", GL_VERTEX_SHADER));
    //shader_modules.push_back(makeShader("/home/persist/z/Documents/repos/learning/C/OpenGL/shader/fragment.glsl", GL_FRAGMENT_SHADER));

    return createShaderProgram(shader_modules);
}