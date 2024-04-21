#include "cavegen.h"
#include "modules/gui/gl.h"
#include "modules/gui/shader.h"

// static camera object
Camera CaveGeneration::camera = Camera();

// initialize glDebugMessageCallback


CaveGeneration::CaveGeneration()
    {
        Logger::SetLevel(debugLevel);
        //glDebugMessageCallback(gl_error_callback, 0);

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
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) || err)
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
                bindObjectBuffer(buffer, chunk.second, shader);
            }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DITHER);
        glDisable(GL_CULL_FACE);
        
        current_chunks = world.visible_chunks;

        return true;
    }

static inline void mouseCallback(GLFWwindow* window, double xpos, double ypos)
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

void CaveGeneration::updateWorld()
    {
        // Update the world chunks
        world.UpdateChunks(CaveGeneration::camera.location);
        
        // If the map table size changes, reinitialize the object buffer
        if (world.delete_chunks.size() > 0 || world.new_chunks.size() > 0)
            {

                for (auto& chunk : world.new_chunks)
                    { 
                        Logger::Verbose("New Chunk: %i %i %i ", chunk.x, chunk.y, chunk.z);
                        unsigned int* buffer = new unsigned int;

                        glGenVertexArrays(1, buffer);
                        bindObjectBuffer(*buffer, world.MapTable[chunk], shader);

                        chunk_buffers[chunk] = *buffer;
                        delete buffer;
                    }
                world.new_chunks.clear();
                
                // get a list of the delete chunks buffers and delete them
                for (auto& chunk : world.delete_chunks)
                    {
                        Logger::Verbose("Delete Chunk: %i %i %i\n", chunk.x, chunk.y, chunk.z);
                        if (chunk_buffers.find(chunk) != chunk_buffers.end())
                            { 
                                unsigned int buffer = chunk_buffers[chunk];

                                glDeleteBuffers(1, &buffer);
                                glDeleteVertexArrays(1, &buffer);

                                chunk_buffers.erase(chunk);
                            }                               
                    }
                world.delete_chunks.clear();
            }

        for (auto& chunk : world.MapTable)
            {
                glBindVertexArray(chunk_buffers[chunk.first]);
                glDrawElements(GL_TRIANGLES, chunk.second.indices.size(), GL_UNSIGNED_INT, 0);
            }

        return;
    }

void CaveGeneration::render()
    {
        bool show_window = true;

        while(!glfwWindowShouldClose(window) && !camera.killapp)
            {

                // clear screen data
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glUseProgram(shader);

                glfwPollEvents();

                // check for mouse input
                if (!camera.freeMouse)
                    { 
                        playerControls(); 
                        glfwSetCursorPosCallback(window, mouseCallback);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    
                    }
                else 
                    {
                        glfwSetCursorPosCallback(window, NULL);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    }

                updateWorld();

                MVP();
                
                imgui(show_window);
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
            { 
                glDeleteVertexArrays(1, &buffer.second); 
                glDeleteBuffers(1, &buffer.second);
            }

        glDeleteProgram(shader);
        glfwTerminate();
    }
