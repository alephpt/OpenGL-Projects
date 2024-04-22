#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string shader_path = "/home/persist/z/Ancillary/Big Stick Studios/repos/learning/Cpp/OpenGl/Primary/CaveGen/cavegen/components/base.shader";

// used to store our shader source code
struct ShaderSource {
  std::string VertexSource;
  std::string FragmentSource;
//    std::string ComputeSource;
}; 

enum class ShaderType {
  NONE = -1,
  VERTEX = 0,
  FRAGMENT = 1,
  COMPUTE = 2
};


// function  definitions
static unsigned int compileShader(unsigned int type, const std::string& sourcecode)
  {
      int result;
      unsigned int id = glCreateShader(type);
      const char* src = sourcecode.c_str();
      //printf("Compiling Shader:\n%s\n", src);
      glShaderSource(id, 1, &src, nullptr);
      glCompileShader(id);
      glGetShaderiv(id, GL_COMPILE_STATUS, &result);

      if (result == GL_FALSE)
          {
              int length;
              glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
              char* message = (char*)alloca(length * sizeof(char));
              glGetShaderInfoLog(id, length, &length, message);
              std::cout << "Failed "
                        << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
                        << " Shader Compilation!" << std::endl;
              std::cout << message << std::endl;
              glDeleteShader(id);
              return 0;
          }

      return id;
  }

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader, unsigned int& program)
    {
        //printf("Creating Vertex Shader:\n%s\n", vertexShader.c_str());
        //printf("Creating Fragment Shader:\n%s\n", fragmentShader.c_str());

        int success;
        unsigned int vert = compileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
  //     unsigned int comp = compileShader(GL_COMPUTE_SHADER, fragmentShader);

        glAttachShader(program, vert);
        glAttachShader(program, frag);
  //      glAttachShader(program, comp);
        glLinkProgram(program);
        glValidateProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        char* infoLog = (char*)alloca(success * sizeof(char));

        if (!success) 
            {
                glGetProgramInfoLog(program, success, NULL, infoLog);
                std::cout << "Creating Shader Failed\n" << infoLog << std::endl;
            }
        
        glDeleteShader(vert);
        glDeleteShader(frag);

        return program;
    }
static ShaderSource parseShader(const std::string& filepath)
    {
        std::ifstream stream(filepath);
        
        if(!stream)
            { std::cout << "Failed to open file: " << filepath << std::endl; }

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;

        while (getline(stream, line))
            {
                if (line.find("#Shader") != std::string::npos)
                    {
                        if(line.find("Vertex") != std::string::npos)
                            { type = ShaderType::VERTEX; }
                        else if (line.find("Fragment") != std::string::npos)
                            { type = ShaderType::FRAGMENT; }
                        else if (line.find("Compute") != std::string::npos)
                            { type = ShaderType::COMPUTE; }
                    }
                else
                    { ss[(int)type] << line << '\n'; }
            }

        return{ss[0].str(), ss[1].str()};
    }
