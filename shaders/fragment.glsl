#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main() {
    // Draw circular particles using point coordinates
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    float distance = length(circCoord);
    if (distance > 1.0) {
        discard;
    }
    FragColor = vec4(vertexColor, 1.0);
}
