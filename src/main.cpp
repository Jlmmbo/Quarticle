#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include "particles.h"
#include "draw.h"
#include "ui.h"

#define PARTICLE_COUNT 2
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define FIXED_TIMESTEP (1.0 / 60.0)
#define MAX_FRAME_TIME 0.1

struct AppState {
    Particle* particles;
    int count;
    int selected;
};

static void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        AppState* state = (AppState*)glfwGetWindowUserPointer(window);
        if (!state) return;
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        state->selected = PickParticle(state->particles, state->count, x, y, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
}

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

    if (!InitUI()) {
        std::cerr << "Failed to initialize UI\n";
    }

    AppState state = { particles, PARTICLE_COUNT, -1 };
    glfwSetWindowUserPointer(window, &state);
    glfwSetMouseButtonCallback(window, MouseCallback);

    double lastTime = glfwGetTime();
    double accumulator = 0.0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double frameTime = currentTime - lastTime;
        lastTime = currentTime;

        if (frameTime > MAX_FRAME_TIME)
            frameTime = MAX_FRAME_TIME;

        accumulator += frameTime;

        while (accumulator >= FIXED_TIMESTEP) {
            UpdateParticles(particles, PARTICLE_COUNT, FIXED_TIMESTEP);
            accumulator -= FIXED_TIMESTEP;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        Draw(particles, PARTICLE_COUNT);

        if (state.selected >= 0 && state.selected < state.count) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            DrawSelectionHighlight(&particles[state.selected], SCREEN_WIDTH, SCREEN_HEIGHT);
            DrawParticlePanel(&particles[state.selected], state.selected, SCREEN_WIDTH, SCREEN_HEIGHT);
            glDisable(GL_BLEND);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
