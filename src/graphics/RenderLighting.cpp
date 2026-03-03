#include "graphics/RenderLighting.h"
#include "core/GameContext.h"
#include <GL/glew.h>

void setupLightingPhase1(GameContext& ctx)
{
    GLfloat ambientLight[] = { 0.02f, 0.02f, 0.05f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    GLfloat lightDiffuse[] = { 0.05f, 0.06f, 0.12f, 1.0f };
    GLfloat lightAmbient[] = { 0.0f,  0.0f,  0.02f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glFogf(GL_FOG_START, 5.0f);
    glFogf(GL_FOG_END,   20.0f);

    glUniform3f(glGetUniformLocation(ctx.progLighting, "uSunDir"),
                0.3f, 1.0f, 0.2f);
    glUniform3f(glGetUniformLocation(ctx.progLighting, "uSunColor"),
                0.15f, 0.18f, 0.28f);
    glUniform1f(glGetUniformLocation(ctx.progLighting, "uSunIntensity"), 1.0f);
    glUniform3f(glGetUniformLocation(ctx.progLighting, "uAmbient"),
                0.02f, 0.02f, 0.05f);
}

void setupLightingPhase2(GameContext& ctx)
{
    GLfloat ambientLight[] = { 0.1f, 0.1f, 0.15f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    GLfloat lightDiffuse[] = { 0.3f, 0.3f, 0.4f, 1.0f };
    GLfloat lightAmbient[] = { 0.05f, 0.05f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glFogf(GL_FOG_START, 10.0f);
    glFogf(GL_FOG_END,   35.0f);

    glUniform3f(glGetUniformLocation(ctx.progLighting, "uSunDir"),
                0.3f, 1.0f, 0.2f);
    glUniform3f(glGetUniformLocation(ctx.progLighting, "uSunColor"),
                0.3f, 0.3f, 0.4f);
    glUniform1f(glGetUniformLocation(ctx.progLighting, "uSunIntensity"), 1.0f);
    glUniform3f(glGetUniformLocation(ctx.progLighting, "uAmbient"),
                0.10f, 0.10f, 0.15f);
}

void setupLightingPhase3(GameContext& ctx)
{
    GLfloat ambientLight[] = { 0.25f, 0.20f, 0.15f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    GLfloat lightDiffuse[] = { 1.0f, 0.7f, 0.4f, 1.0f };
    GLfloat lightAmbient[] = { 0.4f, 0.25f, 0.15f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    GLfloat lightPos[] = { -0.8f, 0.3f, -0.2f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // ← passa direção e cor do sol para o shader
    glUniform3f(glGetUniformLocation(ctx.progLighting, "uSunDir"),
                -0.8f, 0.3f, -0.2f);          // sol baixo, vindo da esquerda
    glUniform3f(glGetUniformLocation(ctx.progLighting, "uSunColor"),
                1.0f, 0.65f, 0.3f);            // cor laranja/quente
    glUniform1f(glGetUniformLocation(ctx.progLighting, "uSunIntensity"),
                1.2f);                          // intensidade
    glUniform3f(glGetUniformLocation(ctx.progLighting, "uAmbient"),
                0.45f, 0.35f, 0.25f);
}