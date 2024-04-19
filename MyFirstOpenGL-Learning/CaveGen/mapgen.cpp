
#include "resources/modules/world.h"
#include "resources/modules/user-input.h"
#include "resources/modules/shader.h"

#include "resources/components/imgui/imgui.h"
#include "resources/components/imgui/imgui_impl_glfw.h"
#include "resources/components/imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <map>

#if defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>


void cleanUp(std::vector<unsigned int> arrayObjects, unsigned int program);
unsigned int ShaderData();
void ObjectBuffer(unsigned int &arrayObject, ChunkData &MapChunkData);
void imgui(bool show_window, World &Map);
void MVP(unsigned int shader);

static void glfw_error_callback(int error, const char* description){
  fprintf(stderr, "GLFW ERR: %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}


// MAIN FUNCTION //
int main (){
  printf("Starting Application\n");
  World Map;
  //ChunkData *Test = new ChunkData;
  //std::vector<int> offset = {0, 0, 0};
  //ChunkData MapChunkData = Map.MapGen.MapGeneration(Test, offset);
  //delete Test;

  const int screenHeight = 720;
  const int screenWidth = 1260;

  printf("Creating Window\n");
  // window initialization and resizing
  glfwSetErrorCallback(glfw_error_callback);
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Cave Generation", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

    bool err = gladLoadGL() == 0;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
    }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  
  printf("Creating Buffers\n");
  // imGUI initialization
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 420 core");
  ImGui::StyleColorsDark();
  bool show_window = true;

  // buffer data
  std::vector<unsigned int> VAOs;
  unsigned int shader = ShaderData();
  unsigned int map_table_size = Map.MapTable.size();
  VAOs.resize(map_table_size);
  int VAO_index = 0;
  for (auto& chunk : Map.MapTable){
    ObjectBuffer(VAOs[VAO_index++], chunk.second);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DITHER);
  glDisable(GL_CULL_FACE);
  
  int prev_map_table_size = map_table_size;
  printf("Starting Render Loop\n");
  while(!glfwWindowShouldClose(window) && !camera.killapp){
   
    // clear screen data
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // load input data
    glUseProgram(shader);

    // if mouse is free - kill callback - else disable cursor, log mouse and user input
    if (camera.freeMouse) {
      glfwSetCursorPosCallback(window, NULL);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else if (!camera.freeMouse){
      userInput(window);
      glfwSetCursorPosCallback(window, mouselog);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    // initialize screen data handling
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // imgui instance and object buffer
    imgui(show_window, Map);

    // If the map table size changes, reinitialize the object buffer
    if (prev_map_table_size != Map.MapTable.size()){
      printf("Map Table Size Changed\n");
      VAOs.clear();
      VAOs.resize(Map.MapTable.size());
      VAO_index = 0;
      for (auto& chunk : Map.MapTable){ ObjectBuffer(VAOs[VAO_index++], chunk.second); }
      prev_map_table_size = Map.MapTable.size();
    }

    VAO_index = 0;
    for (auto& chunk : Map.MapTable){
      glBindVertexArray(VAOs[VAO_index++]);
      glDrawElements(GL_TRIANGLES, chunk.second.indices.size(), GL_UNSIGNED_INT, 0);
    }
    

   // render objects
    // glDrawArrays(GL_POINTS, 0, Map.vertices.size());

    // model - view - projection 
    MVP(shader);
    
    // refresh compute data
    // Map.MapGeneration();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  cleanUp(VAOs, shader);
  return 0;
}

// generates and binds Object and Vertex Array buffers, and populates buffer data with Object Vertices
void ObjectBuffer(unsigned int &arrayObject, ChunkData &MapChunkData){
  printf("Creating Object Buffer for Chunk: %d, %d, %d\n", MapChunkData.offset.x, MapChunkData.offset.y, MapChunkData.offset.z);
  unsigned int EBO, VBO[3];

  glGenVertexArrays(1, &arrayObject);
  glGenBuffers(3, VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(arrayObject);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, MapChunkData.vertices.size() * sizeof(float), &MapChunkData.vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, MapChunkData.indices.size() * sizeof(int), &MapChunkData.indices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_ARRAY_BUFFER, MapChunkData.normals.size() * sizeof(float), &MapChunkData.normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
  glBufferData(GL_ARRAY_BUFFER, MapChunkData.colors.size() * sizeof(float), &MapChunkData.colors[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

// calls the shader source code, parsing and creating shader program
unsigned int ShaderData(){
  unsigned int program = glCreateProgram();
  ShaderSource source = parseShader("/home/persist/z/Ancillary/Big Stick Studios/repos/learning/Cpp/OpenGl/MyFirstOpenGL-Learning/CaveGen/resources/components/base.shader");
  unsigned int shader = createShader(source.VertexSource, source.FragmentSource, program);
  return shader;
}

void imgui(bool show_window, World &Map){
      // imGui context
    if (show_window){
      ImGui::Begin("Controls", &show_window);
      ImGui::Text("Menu:");
      
      if(ImGui::Button("Camera Controls") && camera.freeMouse){
        camera.freeMouse = false;
        camera.location = camera.location;
        camera.direction = camera.direction;
        camera.position = camera.position;
        camera.front = camera.front;
      }
      ImGui::SameLine();
      if(ImGui::Button("Reset Camera")){
        camera.front = glm::vec3(0.7f, -0.15f, 0.70f);
        camera.location = glm::vec3(180.8f, -366.87f, 146.8f);
      }
      ImGui::SameLine();
      if(ImGui::Button(" Exit ")){
        camera.killapp = true;
      }
      
      ImGui::SliderInt(" - Scale", &Map.scalar, 0, 25);
      ImGui::SliderInt(" - Smoothing Level ", &Map.howSmooth, 0, 10);
      ImGui::SliderFloat(" - Noise Thresh ", &Map.noiseThreshold, 0.0f, 100.0f);
      ImGui::SliderFloat(" - Fill Cutoff", &Map.fillCutOff, 0.0f, 100.0f);

      if(ImGui::Button("Regen")){
        Map.MapTable.clear();
      }
      ImGui::SameLine();
      if(ImGui::Button("Solid")){
        Map.noiseThreshold = 72.44f;
        Map.fillCutOff = 27.37f;
        Map.MapTable.clear();
      }
      ImGui::SameLine();
      if(ImGui::Button("Edges")){
        Map.noiseThreshold = 21.29f;
        Map.fillCutOff = 85.05f;
        Map.MapTable.clear();
      }
      ImGui::SameLine();
      if(ImGui::Button("Tunnels")){
        Map.noiseThreshold = 77.18f;
        Map.fillCutOff = 17.69f;
        Map.MapTable.clear();
      }
      ImGui::SameLine();
      if(ImGui::Button("Cells")){
        Map.noiseThreshold = 81.36f;
        Map.fillCutOff = 13.31f;
        Map.MapTable.clear();
      }

      Map.UpdateChunks(camera.location);


      ImGui::End();
    }

    if (show_window){
      ImGui::Begin("Data", &show_window);
      ImGui::Text("Application average %.2f ms/frame (%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      int vertexCount = 0;
      int triangleCount = 0;
      for (auto& chunk : Map.MapTable){
        vertexCount += chunk.second.vertices.size();
        triangleCount += chunk.second.indices.size();
      }
      ImGui::Text("Vertex Count : %i - Triangles : %i \n" , vertexCount, triangleCount);
      ImGui::Text(" Camera Coords     X       Y       Z");
      ImGui::Text(" - Grid Pos  -    %i    %i    %i", Map.currentChunk[0], Map.currentChunk[1], Map.currentChunk[2]);
      ImGui::Text(" - Location  -    %.2f    %.2f    %.2f", camera.location.x, camera.location.y, camera.location.z);
      ImGui::Text(" - Direction -    %.2f    %.2f    %.2f\n", camera.direction.x, camera.direction.y, camera.direction.z);
      
      ImGui::End();
    }
};

void MVP(unsigned int shader){
   // view matrix - projection, view, model 
    glm::mat4 projection;
    projection = glm::perspective(55.0f, 1.0f, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
    view = glm::translate(view, camera.location);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);

    glm::mat4 model = glm::mat4(2.0f);
    // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
}

// deletes shader program and buffers
void cleanUp(std::vector<unsigned int> arrayObjects, unsigned int program){
  for (auto& arrayObject : arrayObjects){
    glDeleteVertexArrays(1, &arrayObject);
  }
  glDeleteProgram(program);
  glfwTerminate();
}
