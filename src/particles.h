#ifndef PARTICLES_H
#define PARTICLES_H

#include <cmath>

static const double C = 100.0;

typedef struct Particle {
    double x, y;
    float size;
    float mass;
    float spin;
    float charge;
    int chromo;
    double p_mag, p_angle;
    int type;
} Particle;

void UpdateParticles(Particle* particles, int count, double dt) {
    for (int i = 0; i < count; i++) {
        double m = particles[i].mass;
        double p_mag = particles[i].p_mag;
        double p_angle = particles[i].p_angle;

        double ca = cos(p_angle);
        double sa = sin(p_angle);

        double denom = sqrt(m * m * C * C + p_mag * p_mag);
        double v_mag = (denom > 0.0) ? p_mag * C / denom : 0.0;

        particles[i].x += v_mag * ca * dt;
        particles[i].y += v_mag * sa * dt;

        float fx = 0.0f, fy = 0.0f;
        for (int j = 0; j < count; j++) {
            if (i == j) continue;

            float dx = particles[j].x - particles[i].x;
            float dy = particles[j].y - particles[i].y;
            float d = sqrt(dx * dx + dy * dy) + 0.0001f;

            float f = particles[i].mass * particles[j].mass / (d * d);
            f += particles[i].charge * particles[j].charge / (d * d);

            fx += f * dx / d;
            fy += f * dy / d;
        }

        double f_parallel = fx * ca + fy * sa;
        double f_perp = -fx * sa + fy * ca;

        p_mag += f_parallel * dt;

        if (p_mag < 0.0) {
            p_mag = -p_mag;
            p_angle += M_PI;
            f_perp = -f_perp;
        }

        if (p_mag > 0.0) {
            p_angle += (f_perp / p_mag) * dt;
        } else {
            double f_mag = sqrt((double)fx * fx + (double)fy * fy);
            if (f_mag > 0.0) {
                p_angle = atan2(fy, fx);
            }
        }

        particles[i].p_mag = p_mag;
        particles[i].p_angle = p_angle;
    }
}

#endif