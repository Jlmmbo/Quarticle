#ifndef DRAW_H
#define DRAW_H

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

GLuint VAO, VBO, ShaderProgram;

// Load shader source from file
std::string LoadShaderFile(const char *filePath)
{
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open())
    {
        std::cerr << "Failed to open shader file: " << filePath << '\n';
        return "";
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}

// Compile shader helper
GLuint CompileShader(const std::string &source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << '\n';
    }
    return shader;
}

int InitOpenGL()
{
    // Load and compile shaders from files
    std::string vertexSource = LoadShaderFile("shaders/vertex.glsl");
    std::string fragmentSource = LoadShaderFile("shaders/fragment.glsl");

    if (vertexSource.empty() || fragmentSource.empty())
    {
        std::cerr << "Failed to load shader files\n";
        return -1;
    }

    GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

    ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, vertexShader);
    glAttachShader(ShaderProgram, fragmentShader);
    glLinkProgram(ShaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ShaderProgram, 512, nullptr, infoLog);
        std::cerr << "Program linking failed: " << infoLog << '\n';
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Allocate buffer (6 floats per particle: x, y, r, g, b, mass)
    glBufferData(GL_ARRAY_BUFFER, 1000 * 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Mass attribute (location 2)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_PROGRAM_POINT_SIZE); // Allow vertex shader to set point size
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return 0;
}

void Draw(Particle *particles, int count)
{
    // Pack particle data into vertex buffer
    float *vertexData = new float[count * 6];

    for (int i = 0; i < count; i++)
    {
        vertexData[i * 6 + 0] = particles[i].x / 400.0f - 1.0f;  // Normalize to -1 to 1
        vertexData[i * 6 + 1] = particles[i].y / 300.0f - 1.0f;

        // Map chromo to RGB colors
        // 1: red, 2: green, 3: blue, 0: white
        // -1: cyan (anti-red), -2: magenta (anti-green), -3: yellow (anti-blue)
        float r = 0.0f, g = 0.0f, b = 0.0f;
        switch (particles[i].chromo) {
            case 1:   r = 1.0f; break;                    // Red
            case 2:   g = 1.0f; break;                    // Green
            case 3:   b = 1.0f; break;                    // Blue
            case 0:   r = g = b = 1.0f; break;            // White
            case -1:  g = 1.0f; b = 1.0f; break;          // Cyan (anti-red)
            case -2:  r = 1.0f; b = 1.0f; break;          // Magenta (anti-green)
            case -3:  r = 1.0f; g = 1.0f; break;          // Yellow (anti-blue)
        }

        vertexData[i * 6 + 2] = r;
        vertexData[i * 6 + 3] = g;
        vertexData[i * 6 + 4] = b;
        vertexData[i * 6 + 5] = particles[i].mass;  // Mass for point size
    }

    // Update buffer data
    glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
    glBufferSubData(GL_COPY_WRITE_BUFFER, 0, count * 6 * sizeof(float), vertexData);

    // Draw
    glUseProgram(ShaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, count);

    delete[] vertexData;
}

#endif