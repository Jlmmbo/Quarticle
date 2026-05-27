#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include "particles.h"
#include "draw.h"

int main() {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Quarticle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    InitOpenGL();

    Particle particles[10] = {
        //x,y,mass,spin,charge,chromo,velocityX, velocityY
        {200, 150, 10, .5f, 1, 1, 0, 0},
        {400, 150, 20, 1, 1, 2, 0, 0},
        {600, 150, 30, 1, 1, 3, 0, 0},
        {200, 300, 10, 1, 1, -1, 0, 0},
        {400, 300, 10, 1, 1, -2, 0, 0},
        {600, 300, 5, 1, 1, -3, 0, 0},
        {200, 450, 5, 1, 1, 0, 0, 0},
        {400, 450, 0.01, 1, 1, 3, 0, 0},
        {600, 450, 1, 1, 1, -1, 0, 0},
        {400, 75, 0.01, 1, 1, 0, 0, 0},
    };

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        UpdateParticles(particles, 10);
        Draw(particles, 10);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
