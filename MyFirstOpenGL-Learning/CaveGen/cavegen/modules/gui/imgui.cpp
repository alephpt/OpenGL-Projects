#include "../../cavegen.h"
#include "../../components/imgui/imgui.h"
#include "../../components/imgui/imgui_impl_glfw.h"
#include "../../components/imgui/imgui_impl_opengl3.h"

void CaveGeneration::imguiInit()
    {
        // imGUI initialization
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 420 core");
        ImGui::StyleColorsDark();
    }

void CaveGeneration::imgui(bool show_window)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

          // imGui context
        if (show_window)
            {
                ImGui::Begin("Controls", &show_window);
                ImGui::Text("Menu:");
                
                if(ImGui::Button("Camera Controls") && camera.freeMouse)
                    {
                        CaveGeneration::camera.freeMouse = false;
                        CaveGeneration::camera.location = CaveGeneration::camera.location;
                        CaveGeneration::camera.direction = CaveGeneration::camera.direction;
                        CaveGeneration::camera.position = CaveGeneration::camera.position;
                        CaveGeneration::camera.front = CaveGeneration::camera.front;
                    }
                
                ImGui::SameLine();
                if(ImGui::Button("Reset Camera"))
                    {
                        CaveGeneration::camera.front = glm::vec3(0.7f, -0.15f, 0.70f);
                        CaveGeneration::camera.location = glm::vec3(180.8f, -366.87f, 146.8f);
                    }

                ImGui::SameLine();
                if(ImGui::Button(" Exit "))
                    { CaveGeneration::camera.killapp = true; }
                
                ImGui::SliderInt(" - Scale", &Map.scalar, 0, 25);
                ImGui::SliderInt(" - Smoothing Level ", &Map.howSmooth, 0, 10);
                ImGui::SliderFloat(" - Noise Thresh ", &Map.noiseThreshold, 0.0f, 100.0f);
                ImGui::SliderFloat(" - Fill Cutoff", &Map.fillCutOff, 0.0f, 100.0f);

                if(ImGui::Button("Regen"))
                    { Map.MapTable.clear(); }

                ImGui::SameLine();
                if(ImGui::Button("Solid"))
                    {
                        Map.noiseThreshold = 72.44f;
                        Map.fillCutOff = 27.37f;
                        Map.MapTable.clear();
                    }

                ImGui::SameLine();
                if(ImGui::Button("Edges"))
                    {
                        Map.noiseThreshold = 21.29f;
                        Map.fillCutOff = 85.05f;
                        Map.MapTable.clear();
                    }

                ImGui::SameLine();
                if(ImGui::Button("Tunnels"))
                    {
                        Map.noiseThreshold = 77.18f;
                        Map.fillCutOff = 17.69f;
                        Map.MapTable.clear();
                    }

                ImGui::SameLine();
                if(ImGui::Button("Cells"))
                  {
                      Map.noiseThreshold = 81.36f;
                      Map.fillCutOff = 13.31f;
                      Map.MapTable.clear();
                  }

                Map.UpdateChunks(CaveGeneration::camera.location);

                ImGui::End();
            }

        if (show_window)
            {
                ImGui::Begin("Data", &show_window);
                ImGui::Text("Application average %.2f ms/frame (%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                int vertexCount = 0;
                int triangleCount = 0;

                for (auto& chunk : Map.MapTable)
                    {
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

void CaveGeneration::imguiRender()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

void CaveGeneration::imguiDestroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }