#ifndef PARTICLES_H
#define PARTICLES_H

#include <cmath>

// Uses planck units for simplicity of formulas

typedef struct Particle {
    double x, y;
    float size;
    float mass;
    float spin;
    float charge;
    int chromo;// 1/2/3: red/green/blue, 0: white, -1/-2/-3: anti-red/anit-green/anti-blue
    double velocityX, velocityY;
    int type;// 0
} Particle;

void UpdateParticles(Particle* particles, int count, double dt) {
    //classical forces
    float force;
    for (int i = 0; i < count; i++) {
        particles[i].x += particles[i].velocityX * dt;
        particles[i].y += particles[i].velocityY * dt;
        for (int j = 0; j < count; j++) {
            if (i == j) continue;

            float dx = particles[j].x - particles[i].x;
            float dy = particles[j].y - particles[i].y;
            float d = sqrt(dx * dx + dy * dy) + 0.0001f; // Avoid division by zero

            //Gravitational force
            force = particles[i].mass * particles[j].mass / (d * d);

            //Electromagnetic force
            force += particles[i].charge * particles[j].charge / (d * d);

            particles[i].velocityX += force * dx / d / particles[i].mass * dt;
            particles[i].velocityY += force * dy / d / particles[i].mass * dt;
        }
    }
}

#endif