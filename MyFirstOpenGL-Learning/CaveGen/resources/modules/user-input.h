#pragma once

#include "camera.h"

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

void userInput(GLFWwindow *window)
    {
        float directx = camera.speed * camera.direction.x;
        float directy = camera.speed * camera.direction.y;
        float directz = camera.speed * camera.direction.z;
        float directxe = (camera.speed * 1.25) * camera.xstrafe;
        float directze = (camera.speed * 1.25) * camera.zstrafe;

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            { glfwSetWindowShouldClose(window, true); }

        if(glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
            {
                camera.freeMouse = true;
                camera.newmouse = true;
            }

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                camera.location.z -= directz;
                camera.location.x -= directx;
                camera.location.y -= directy;
            }

        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                camera.location.z += directz;
                camera.location.x += directx;
                camera.location.y += directy;
            }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                camera.location.z += directze;
                camera.location.x += directxe;
            }

        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                camera.location.z -= directze;
                camera.location.x -= directxe;
            }

        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            { camera.location.y -= camera.speed * 0.635f; }

        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            { camera.location.y += camera.speed / 2; }

        // if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        //   camera.bPress = true;
        // }
    }

void mouselog(GLFWwindow* window, double xpos, double ypos)
    {
        float xoffset = xpos - camera.lastX;
        float yoffset = camera.lastY - ypos;
        camera.lastX = xpos;
        camera.lastY = ypos;

        const float sensitive = 0.1f;
        xoffset *= sensitive;
        yoffset *= sensitive;

        camera.yaw += xoffset;
        camera.pitch += yoffset;

        if(camera.pitch > 89.0f)
            { camera.pitch = 89.0f; }

        if(camera.pitch < -89.0f)
            { camera.pitch = -89.0f; }

        camera.direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.direction.y = sin(glm::radians(camera.pitch));
        camera.direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.xstrafe = cos(glm::radians(camera.yaw) + glm::radians(90.0f)) * cos(glm::radians(camera.pitch));
        camera.zstrafe = sin(glm::radians(camera.yaw) + glm::radians(90.0f)) * cos(glm::radians(camera.pitch));
        camera.front = glm::normalize(camera.direction);
    }