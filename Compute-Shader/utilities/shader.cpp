#include "shader.h"

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

// Create Shader Program
unsigned int createShaderProgram(const std::vector<unsigned int>& shader_modules) {
    unsigned int shader_program = glCreateProgram();

    for (unsigned int mod : shader_modules) {
        glAttachShader(shader_program, mod);
    }

    glLinkProgram(shader_program);

    int program_created;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &program_created);

    if (!program_created) {
        char err_log[1024];
        glGetProgramInfoLog(shader_program, 1024, NULL, err_log);
        std::cout << "HEY!! - Your Shader Program Failed to Build, Creating Program:\n" << err_log << std::endl;
    }

    for (unsigned int mod : shader_modules) {
        glDeleteShader(mod);
    }

    return shader_program;
}

unsigned int initShaders() {
    std::vector<unsigned int> shader_modules;

    shader_modules.push_back(makeShader("/home/persist/z/Documents/repos/learning/C/OpenGL/shaders/vertex.glsl", GL_VERTEX_SHADER));
    shader_modules.push_back(makeShader("/home/persist/z/Documents/repos/learning/C/OpenGL/shaders/fragment.glsl", GL_FRAGMENT_SHADER));

    return createShaderProgram(shader_modules);
}