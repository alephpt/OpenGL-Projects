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

void CaveGeneration:: imgui(bool show_window)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int top_window_height = 0;

        // Data Status Box
        if (show_window)
            {
                // Box Logic
                int chunkCount = world.MapTable.size();
                int vertexCount = 0;
                int indexCount = 0;

                for (auto& chunk : world.MapTable)
                    {
                        vertexCount += chunk.second.vertices.size();
                        indexCount += chunk.second.indices.size();
                    }

                int triangleCount = indexCount / 3;

                // Box Format
                ImGui::Begin("Data", &show_window);
                ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - ImGui::GetWindowWidth() - 10, 10));
                top_window_height = ImGui::GetWindowHeight();
                // Align text to the right
                ImGui::Separator();
                // Make Font Bold
                ImGui::Text("World Data"); 
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("0.00f ms/frame (000.0f FPS)").x - 10);
                ImGui::Text("%.2f ms/frame (%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::Separator();
                ImGui::Columns(2, "worlddata");
                ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * .38);
                ImGui::SetColumnWidth(1, ImGui::GetWindowWidth() * .62);
                ImGui::Text("Total Chunks"); ImGui::NextColumn(); 
                // Align text to the right
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * .68);
                ImGui::Text("%i", chunkCount); ImGui::NextColumn();

                ImGui::Text("Total Vertices"); ImGui::NextColumn(); 
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * .68 - 5);
                ImGui::Text("%i", vertexCount); ImGui::NextColumn();

                ImGui::Text("Total Indices"); ImGui::NextColumn(); 
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * .68 - 5);
                ImGui::Text("%i", indexCount); ImGui::NextColumn();

                ImGui::Text("Total Triangles"); ImGui::NextColumn(); 
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * .68 - 5);
                ImGui::Text("%i", triangleCount); ImGui::NextColumn();
                ImGui::Columns(1);

                ImGui::Separator();
                ImGui::NewLine();
                ImGui::Separator();

                ImGui::Columns(4, "userdata");

                ImGui::Text("Cordinate"); ImGui::NextColumn(); 
                ImGui::Text("X"); ImGui::NextColumn(); ImGui::Text("Y"); ImGui::NextColumn(); ImGui::Text("Z"); ImGui::NextColumn();
                ImGui::Separator();
                                
                ImGui::Text("Grid Position"); ImGui::NextColumn();
                glm::vec3 current_position = CaveGeneration::camera.location / (float)world.chunkSize;
                ImGui::Text("%i", current_position.x); ImGui::NextColumn();
                ImGui::Text("%i", current_position.y); ImGui::NextColumn();
                ImGui::Text("%i", current_position.z); ImGui::NextColumn();

                ImGui::Text("Location"); ImGui::NextColumn();
                ImGui::Text("%.2f", camera.location.x); ImGui::NextColumn();
                ImGui::Text("%.2f", camera.location.y); ImGui::NextColumn();
                ImGui::Text("%.2f", camera.location.z); ImGui::NextColumn();

                ImGui::Text("Direction"); ImGui::NextColumn();
                ImGui::Text("%.2f", camera.direction.x); ImGui::NextColumn();
                ImGui::Text("%.2f", camera.direction.y); ImGui::NextColumn();
                ImGui::Text("%.2f", camera.direction.z); ImGui::NextColumn();
                ImGui::Columns(1);

                ImGui::Separator();
                ImGui::End();
            }

        // User Controls Box
        if (show_window)
            {
                ImGui::Begin("Controls", &show_window);
                ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - ImGui::GetWindowWidth() - 10, top_window_height + 20));
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
                
                if (ImGui::SliderInt(" - Scale", &world.config.scalar, 0, 25) ||
                    ImGui::SliderInt(" - Smoothing Level ", &world.config.howSmooth, 0, 10) ||
                    ImGui::SliderFloat(" - Noise Thresh ", &world.config.noiseThreshold, 0.0f, 100.0f) ||
                    ImGui::SliderFloat(" - Fill Cutoff", &world.config.fillCutOff, 0.0f, 100.0f))
                    { 
                        world.fillMode = FillMode::Custom;
                    }

                ImGui::SliderInt(" - Visible Area", &world.depthOfView, 0, 25);

                if(ImGui::Button("Regen"))
                    { 
                        world.reset(); 
                    
                        for (auto& chunk : chunk_buffers)
                            {
                                GLuint buffer = chunk.second;
                                glDeleteBuffers(1, &buffer);
                                glDeleteVertexArrays(1, &buffer);
                            }

                        chunk_buffers.clear();
                    }

                ImGui::SameLine();
                if(ImGui::Button("Solid"))
                    { world.solidFill(); }

                ImGui::SameLine();
                if(ImGui::Button("Edges"))
                    { world.edgeFill(); }

                ImGui::SameLine();
                if(ImGui::Button("Tunnels"))
                    { world.tunnelFill();
                    }

                ImGui::SameLine();
                if(ImGui::Button("Cells"))
                    { world.cellFill(); }

                ImGui::End();
            }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    };

void CaveGeneration::imguiDestroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }