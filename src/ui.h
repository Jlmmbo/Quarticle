#ifndef UI_H
#define UI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "particles.h"
#include <cstdio>
#include <cmath>
#include <string>
#include <cstring>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define CHAR_COUNT 95
#define FONT_TEXTURE_SIZE 512

static GLuint uiProgram = 0;
static GLuint uiVAO = 0, uiVBO = 0;
static GLint uiScreenSizeLoc = -1;

static GLuint textProgram = 0;
static GLuint textVAO = 0, textVBO = 0;
static GLint textScreenSizeLoc = -1, textTextureLoc = -1, textColorLoc = -1;

static GLuint fontTexture = 0;
static stbtt_bakedchar fontCDATA[CHAR_COUNT];

static const char* uiVertexSrc =
    "#version 330 core\n"
    "layout(location = 0) in vec2 aPos;\n"
    "layout(location = 1) in vec4 aColor;\n"
    "out vec4 vColor;\n"
    "uniform vec2 uScreenSize;\n"
    "void main() {\n"
    "    vColor = aColor;\n"
    "    vec2 ndc = aPos / uScreenSize * 2.0 - 1.0;\n"
    "    ndc.y = -ndc.y;\n"
    "    gl_Position = vec4(ndc, 0.0, 1.0);\n"
    "}";

static const char* uiFragmentSrc =
    "#version 330 core\n"
    "in vec4 vColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vColor;\n"
    "}";

static const char* textVertexSrc =
    "#version 330 core\n"
    "layout(location = 0) in vec2 aPos;\n"
    "layout(location = 1) in vec2 aUV;\n"
    "out vec2 vUV;\n"
    "uniform vec2 uScreenSize;\n"
    "void main() {\n"
    "    vUV = aUV;\n"
    "    vec2 ndc = aPos / uScreenSize * 2.0 - 1.0;\n"
    "    ndc.y = -ndc.y;\n"
    "    gl_Position = vec4(ndc, 0.0, 1.0);\n"
    "}";

static const char* textFragmentSrc =
    "#version 330 core\n"
    "in vec2 vUV;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D uTexture;\n"
    "uniform vec4 uColor;\n"
    "void main() {\n"
    "    float alpha = texture(uTexture, vUV).r;\n"
    "    if (alpha < 0.01) discard;\n"
    "    FragColor = vec4(uColor.rgb, uColor.a * alpha);\n"
    "}";

static GLuint CompileShaderFromSrc(const char* src, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        fprintf(stderr, "Shader compile error: %s\n", log);
    }
    return shader;
}

static GLuint LinkProgram(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    int success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        fprintf(stderr, "Program link error: %s\n", log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

bool InitUI() {
    GLuint vs, fs;

    vs = CompileShaderFromSrc(uiVertexSrc, GL_VERTEX_SHADER);
    fs = CompileShaderFromSrc(uiFragmentSrc, GL_FRAGMENT_SHADER);
    uiProgram = LinkProgram(vs, fs);
    uiScreenSizeLoc = glGetUniformLocation(uiProgram, "uScreenSize");

    glGenVertexArrays(1, &uiVAO);
    glGenBuffers(1, &uiVBO);

    vs = CompileShaderFromSrc(textVertexSrc, GL_VERTEX_SHADER);
    fs = CompileShaderFromSrc(textFragmentSrc, GL_FRAGMENT_SHADER);
    textProgram = LinkProgram(vs, fs);
    textScreenSizeLoc = glGetUniformLocation(textProgram, "uScreenSize");
    textTextureLoc = glGetUniformLocation(textProgram, "uTexture");
    textColorLoc = glGetUniformLocation(textProgram, "uColor");

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    const char* fontPaths[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
        nullptr
    };

    FILE* fontFile = nullptr;
    for (int i = 0; fontPaths[i]; i++) {
        fontFile = fopen(fontPaths[i], "rb");
        if (fontFile) break;
    }

    if (!fontFile) {
        fprintf(stderr, "No font file found\n");
        return false;
    }

    fseek(fontFile, 0, SEEK_END);
    long fontSize = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    unsigned char* fontBuffer = new unsigned char[fontSize];
    fread(fontBuffer, 1, fontSize, fontFile);
    fclose(fontFile);

    unsigned char* bitmap = new unsigned char[FONT_TEXTURE_SIZE * FONT_TEXTURE_SIZE];
    stbtt_BakeFontBitmap(fontBuffer, 0, 16.0f, bitmap, FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE,
                         32, CHAR_COUNT, fontCDATA);
    delete[] fontBuffer;

    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE,
                 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    delete[] bitmap;

    return true;
}

static void DrawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
    float verts[] = {
        x,   y,   r, g, b, a,
        x+w, y,   r, g, b, a,
        x,   y+h, r, g, b, a,
        x+w, y+h, r, g, b, a,
    };

    glUseProgram(uiProgram);
    glUniform2f(uiScreenSizeLoc, 1920.0f, 1080.0f);

    glBindVertexArray(uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void DrawString(float x, float y, const char* text, float r, float g, float b, float a, float scale) {
    if (!text || !*text) return;

    glUseProgram(textProgram);
    glUniform2f(textScreenSizeLoc, 1920.0f, 1080.0f);
    glUniform1i(textTextureLoc, 0);
    glUniform4f(textColorLoc, r, g, b, a);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    const int batchSize = 256;
    float vertBuf[batchSize * 6 * 4];

    int charCount = 0;
    float curX = x;
    float curY = y;

    stbtt_aligned_quad q;
    for (const char* p = text; *p; p++) {
        if (*p == '\n') {
            curX = x;
            curY += 20.0f * scale;
            continue;
        }

        int charIndex = (int)(unsigned char)(*p) - 32;
        if (charIndex < 0 || charIndex >= CHAR_COUNT) continue;

        stbtt_GetBakedQuad(fontCDATA, FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE, charIndex,
                           &curX, &curY, &q, 1);

        float* v = &vertBuf[charCount * 6 * 4];

        // Triangle 1: bl, br, tl
        v[0] = q.x0; v[1] = q.y1; v[2] = q.s0; v[3] = q.t1;
        v[4] = q.x1; v[5] = q.y1; v[6] = q.s1; v[7] = q.t1;
        v[8] = q.x0; v[9] = q.y0; v[10] = q.s0; v[11] = q.t0;
        // Triangle 2: tl, br, tr
        v[12] = q.x0; v[13] = q.y0; v[14] = q.s0; v[15] = q.t0;
        v[16] = q.x1; v[17] = q.y1; v[18] = q.s1; v[19] = q.t1;
        v[20] = q.x1; v[21] = q.y0; v[22] = q.s1; v[23] = q.t0;

        charCount++;

        if (charCount >= batchSize) {
            int vertCount = charCount * 6;
            glBufferData(GL_ARRAY_BUFFER, vertCount * 4 * sizeof(float), vertBuf, GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, vertCount);
            charCount = 0;
        }
    }

    if (charCount > 0) {
        int vertCount = charCount * 6;
        glBufferData(GL_ARRAY_BUFFER, vertCount * 4 * sizeof(float), vertBuf, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, vertCount);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void DrawParticlePanel(Particle* p, int index, float screenW, float screenH) {
    float panelX = 15;
    float panelY = 15;
    float panelW = 260;
    float panelH = 220;

    DrawRect(panelX, panelY, panelW, panelH, 0.1f, 0.1f, 0.15f, 0.85f);

    DrawRect(panelX, panelY, panelW, 1.5f, 0.4f, 0.6f, 1.0f, 1.0f);
    DrawRect(panelX, panelY + panelH - 1.5f, panelW, 1.5f, 0.4f, 0.6f, 1.0f, 1.0f);
    DrawRect(panelX, panelY, 1.5f, panelH, 0.4f, 0.6f, 1.0f, 1.0f);
    DrawRect(panelX + panelW - 1.5f, panelY, 1.5f, panelH, 0.4f, 0.6f, 1.0f, 1.0f);

    float tx = panelX + 10;
    float ty = panelY + 10;
    float ls = 19.0f;

    char buf[128];

    snprintf(buf, sizeof(buf), "Particle #%d", index + 1);
    DrawString(tx, ty, buf, 0.4f, 0.7f, 1.0f, 1.0f, 1.0f);
    ty += ls;

    snprintf(buf, sizeof(buf), "Pos: (%.1f, %.1f)", p->x, p->y);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    snprintf(buf, sizeof(buf), "Mom: mag=%.2f ang=%.1f deg", p->p_mag, p->p_angle * 180.0 / M_PI);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    double mC = p->mass * C;
    double E = sqrt(mC * mC + p->p_mag * p->p_mag);
    double v_mag = (E > 0.0) ? p->p_mag * C / E : 0.0;

    snprintf(buf, sizeof(buf), "Vel: mag=%.2f ang=%.1f deg", v_mag, p->p_angle * 180.0 / M_PI);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    snprintf(buf, sizeof(buf), "Speed: %.2f (%.3f c)", v_mag, v_mag / C);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    double dir = p->p_angle * 180.0 / M_PI;
    snprintf(buf, sizeof(buf), "Dir: %.1f deg", dir);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    double gamma = (mC > 0.0 && E > 0.0) ? E / mC : 1.0;
    snprintf(buf, sizeof(buf), "Gamma: %.4f", gamma);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    snprintf(buf, sizeof(buf), "Mass: %.2f", p->mass);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    snprintf(buf, sizeof(buf), "Charge: %.2f", p->charge);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    snprintf(buf, sizeof(buf), "Size: %.1f", p->size);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    const char* chromoNames[] = {
        "White (0)", "Red (1)", "Green (2)", "Blue (3)",
        "Anti-Red (-1)", "Anti-Green (-2)", "Anti-Blue (-3)"
    };
    int chromoIdx = p->chromo >= -3 && p->chromo <= 3 ? p->chromo + 3 : 0;
    snprintf(buf, sizeof(buf), "Chromo: %s", chromoNames[chromoIdx]);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    ty += ls;

    snprintf(buf, sizeof(buf), "Spin: %.2f", p->spin);
    DrawString(tx, ty, buf, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
}

int PickParticle(Particle* particles, int count, double mouseX, double mouseY, float screenW, float screenH) {
    double mx = mouseX / screenW * 2.0 - 1.0;
    double my = 1.0 - mouseY / screenH * 2.0;

    int best = -1;
    double bestDist = 0.1;

    for (int i = 0; i < count; i++) {
        double px = particles[i].x / 400.0 - 1.0;
        double py = particles[i].y / 300.0 - 1.0;
        double dx = mx - px;
        double dy = my - py;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist < bestDist) {
            bestDist = dist;
            best = i;
        }
    }

    return best;
}

void DrawSelectionHighlight(Particle* p, float screenW, float screenH) {
    float ndcX = p->x / 400.0f - 1.0f;
    float ndcY = p->y / 300.0f - 1.0f;

    float cx = (ndcX + 1.0f) * 0.5f * screenW;
    float cy = (1.0f - ndcY) * 0.5f * screenH;

    float radius = p->size * 8.0f;
    float innerRadius = p->size * 6.0f;
    int segments = 32;

    std::vector<float> verts;
    verts.reserve((segments + 1) * 2 * 6);

    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * (float)M_PI;
        float ca = cosf(angle), sa = sinf(angle);

        verts.push_back(cx + innerRadius * ca);
        verts.push_back(cy + innerRadius * sa);
        verts.push_back(0.3f);
        verts.push_back(0.8f);
        verts.push_back(1.0f);
        verts.push_back(0.6f);

        verts.push_back(cx + radius * ca);
        verts.push_back(cy + radius * sa);
        verts.push_back(0.3f);
        verts.push_back(0.8f);
        verts.push_back(1.0f);
        verts.push_back(0.6f);
    }

    glUseProgram(uiProgram);
    glUniform2f(uiScreenSizeLoc, screenW, screenH);

    glBindVertexArray(uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (segments + 1) * 2);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

#endif
