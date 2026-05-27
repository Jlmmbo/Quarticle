#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;
layout (location = 2) in float mass;

out vec3 vertexColor;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    vertexColor = color;
    gl_PointSize = mass * 10.0;  // Scale point size by mass
}
