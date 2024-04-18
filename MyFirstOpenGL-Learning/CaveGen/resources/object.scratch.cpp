// Include necessary headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Define the compute shader source code
const char* computeShaderSource = R"(
    #version 430 core

    // Define your compute shader code here
    // Example:
    layout (local_size_x = 16, local_size_y = 16) in;

    uniform int mapSize;
    uniform int noiseThreshold;

    // Define functions for map generation tasks (e.g., noise generation, smoothing)

    void main() {
        // Implement your map generation algorithm here
        // This is where you perform the map generation computations in parallel
        // Use shared memory or global memory for data access
    }
)";

// Function to compile and link the compute shader
GLuint createComputeShader() {
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    return program;
}

// Function to perform map generation using GPU compute shaders
void performMapGeneration(int mapSize, int noiseThreshold, std::vector<float>& chunkData) {
    // Create and compile the compute shader
    GLuint computeProgram = createComputeShader();

    // Use the compute shader
    glUseProgram(computeProgram);

    // Set uniform variables (e.g., map size, noise threshold)
    glUniform1i(glGetUniformLocation(computeProgram, "mapSize"), mapSize);
    glUniform1i(glGetUniformLocation(computeProgram, "noiseThreshold"), noiseThreshold);

    // Create buffer object for storing chunk data
    GLuint chunkBuffer;
    glGenBuffers(1, &chunkBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, chunkData.size() * sizeof(float), chunkData.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, chunkBuffer);

    // Dispatch the compute shader to perform map generation
    glDispatchCompute(mapSize / 16, mapSize / 16, 1);

    // Synchronize the GPU to ensure the compute shader finishes execution
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Retrieve chunk data from GPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkBuffer);
    float* mappedData = static_cast<float*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));
    if (mappedData) {
        std::copy(mappedData, mappedData + chunkData.size(), chunkData.begin());
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    // Cleanup
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glDeleteBuffers(1, &chunkBuffer);
    glDeleteProgram(computeProgram);
}


// UpdateChunks function modified to call performMapGeneration with appropriate parameters
void World::UpdateChunks(glm::vec3 &playerLoc) {
    // Take user input and find current grid squares

    currentChunk[2] = (int)playerLoc.x / chunkSize;
    currentChunk[1] = (int)playerLoc.y / chunkSize;
    currentChunk[0] = (int)playerLoc.z / chunkSize;

    if (previousChunk != currentChunk) {
        // Determine map size and noise threshold based on parameters
        int mapSize = ...; // Calculate map size based on chunkSize and visibleChunks
        int noiseThreshold = ...; // Calculate noise threshold based on parameters

        // Perform map generation using GPU compute shaders
        performMapGeneration(mapSize, noiseThreshold);

        // Update previousChunk
        previousChunk[0] = currentChunk[0];
        previousChunk[1] = currentChunk[1];
        previousChunk[2] = currentChunk[2];
    }
}


