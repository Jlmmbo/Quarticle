#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include "particles.h"
#include "draw.h"

#define PARTICLE_COUNT 2
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main() {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Quarticle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    InitOpenGL();

    Particle particles[] = {
        //x ,   y,size,mass,spin,charge,chromo,velocityX, velocityY
        {400, 100, 10, 1000000, 0, 0, 1, 0, 0},
        {600, 100, 10, 0.01, 0, 0, 2, 0, 60},
    };

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        UpdateParticles(particles, PARTICLE_COUNT);
        Draw(particles, PARTICLE_COUNT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
