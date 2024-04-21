#include "./cavegen.h"
#include "./modules/gui/gl.h"
#include "./modules/gui/shader.h"
#include "cavegen.h"


Camera CaveGeneration::camera = Camera();

CaveGeneration::CaveGeneration()
    {
        if (!initGLFW())
            { throw std::runtime_error("Failed to initialize GLFW"); }

        if (!initBuffers())
            { throw std::runtime_error("Failed to initialize buffers"); }
    }

bool CaveGeneration::initGLFW()
    {
        printf("Creating Window\n");
        // window initialization and resizing
        glfwSetErrorCallback(glfw_error_callback);
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(screenWidth, screenHeight, "Cave Generation", NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        bool err = gladLoadGL() == 0;
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        return true;
    }

bool CaveGeneration::initBuffers()
    {
        imguiInit();

        // buffer data
        shader = ShaderData();

        for (auto& chunk : world.MapTable)
            {
                unsigned int buffer = 0;
                glGenVertexArrays(1, &buffer);
                chunk_buffers[chunk.first] = buffer;
                bindObjectBuffer(buffer, chunk.second);
            }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DITHER);
        glDisable(GL_CULL_FACE);
        
        current_chunks = world.visible_chunks;

        return true;
    }

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
    {
        float xoffset = xpos - CaveGeneration::camera.lastX;
        float yoffset = CaveGeneration::camera.lastY - ypos;
        CaveGeneration::camera.lastX = xpos;
        CaveGeneration::camera.lastY = ypos;

        const float sensitive = 0.1f;
        xoffset *= sensitive;
        yoffset *= sensitive;

        CaveGeneration::camera.yaw += xoffset;
        CaveGeneration::camera.pitch += yoffset;

        if(CaveGeneration::camera.pitch > 89.0f)
            { CaveGeneration::camera.pitch = 89.0f; }

        if(CaveGeneration::camera.pitch < -89.0f)
            { CaveGeneration::camera.pitch = -89.0f; }

        CaveGeneration::camera.direction.x = cos(glm::radians(CaveGeneration::camera.yaw)) * cos(glm::radians(CaveGeneration::camera.pitch));
        CaveGeneration::camera.direction.y = sin(glm::radians(CaveGeneration::camera.pitch));
        CaveGeneration::camera.direction.z = sin(glm::radians(CaveGeneration::camera.yaw)) * cos(glm::radians(CaveGeneration::camera.pitch));
        CaveGeneration::camera.xstrafe = cos(glm::radians(CaveGeneration::camera.yaw) + glm::radians(90.0f)) * cos(glm::radians(CaveGeneration::camera.pitch));
        CaveGeneration::camera.zstrafe = sin(glm::radians(CaveGeneration::camera.yaw) + glm::radians(90.0f)) * cos(glm::radians(CaveGeneration::camera.pitch));
        CaveGeneration::camera.front = glm::normalize(CaveGeneration::camera.direction);
    }

void CaveGeneration::render()
    {
        while(!glfwWindowShouldClose(window) && !camera.killapp)
            {
                // clear screen data
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // load input data
                glUseProgram(shader);

                // if mouse is free - kill callback - else disable cursor, log mouse and user input
                if (camera.freeMouse) 
                    {
                        glfwSetCursorPosCallback(window, NULL);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    } 
                else if (!camera.freeMouse)
                    {
                        userInput();
                        glfwSetCursorPosCallback(window, mouseCallback);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    }

                // initialize screen data handling
                glfwPollEvents();
                
                // imgui instance and object buffer
                imgui(show_window);

                // TODO: we can abstract and combine this logic with out UPDATECHUNKS function
                // If the map table size changes, reinitialize the object buffer
                if (world.delete_chunks.size() > 0 || world.new_chunks.size() > 0)
                    {
                        printf("Map Table Size Changed\n");

                        // get a list of the delete chunks buffers and delete them
                        for (auto& chunk : world.delete_chunks)
                            {
                                if (chunk_buffers.find(chunk) != chunk_buffers.end())
                                    { 
                                        unsigned int buffer = chunk_buffers[chunk];
                                        glDeleteVertexArrays(1, &buffer);
                                        chunk_buffers.erase(chunk);
                                    }
                            }


                        for (auto& chunk : world.new_chunks)
                            { 
                                if (chunk_buffers.find(chunk) == chunk_buffers.end())
                                    {
                                        unsigned int buffer = 0;
                                        glGenVertexArrays(1, &buffer);
                                        chunk_buffers[chunk] = buffer;
                                        bindObjectBuffer(buffer, world.MapTable[chunk]);
                                    }
                            }
                    }

                for (auto& chunk : world.MapTable)
                    {
                        glBindVertexArray(chunk_buffers[chunk.first]);
                        glDrawElements(GL_TRIANGLES, chunk.second.indices.size(), GL_UNSIGNED_INT, 0);
                    }
                
                // model - view - projection 
                MVP();
                
                // refresh compute data
                imguiRender();

                glfwSwapBuffers(window);
            }

        cleanUp();

        return;
    }

void CaveGeneration::MVP()
    {
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
inline void CaveGeneration::cleanUp()
    {
        imguiDestroy();

        for (auto& buffer : chunk_buffers)
            { glDeleteVertexArrays(1, &buffer.second); }

        glDeleteProgram(shader);
        glfwTerminate();
    }
