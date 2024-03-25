#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

unsigned int createShader(unsigned int mod_type, const char* shader);
unsigned int createShaderProgram(const std::vector<unsigned int>& shader_modules);
unsigned int makeShader( const std::string& filepath, unsigned int mod_type);
unsigned int initShaders();