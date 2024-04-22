#include "../../cavegen.h"

void CaveGeneration::playerControls()
    {
        float directx = CaveGeneration::camera.speed * CaveGeneration::camera.direction.x;
        float directy = CaveGeneration::camera.speed * CaveGeneration::camera.direction.y;
        float directz = CaveGeneration::camera.speed * CaveGeneration::camera.direction.z;
        float directxe = (CaveGeneration::camera.speed * 1.25) * CaveGeneration::camera.xstrafe;
        float directze = (CaveGeneration::camera.speed * 1.25) * CaveGeneration::camera.zstrafe;

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            { 
                if (!CaveGeneration::camera.freeMouse)
                    { CaveGeneration::camera.freeMouse = true; }
                else
                    { 
                        CaveGeneration::camera.killapp = true; 
                        Logger::Info("Exiting Application");
                    }
            }

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                CaveGeneration::camera.location.z -= directz;
                CaveGeneration::camera.location.x -= directx;
                CaveGeneration::camera.location.y -= directy;
            }

        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                CaveGeneration::camera.location.z += directz;
                CaveGeneration::camera.location.x += directx;
                CaveGeneration::camera.location.y += directy;
            }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                CaveGeneration::camera.location.z += directze;
                CaveGeneration::camera.location.x += directxe;
            }

        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                CaveGeneration::camera.location.z -= directze;
                CaveGeneration::camera.location.x -= directxe;
            }

        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // UP
            { CaveGeneration::camera.location.y -= CaveGeneration::camera.speed * 0.635f; }

        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) // DOWN
            { CaveGeneration::camera.location.y += CaveGeneration::camera.speed * 0.635f; }
    }