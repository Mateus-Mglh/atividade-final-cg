#include "core/GameContext.h"
#include "core/fps.h"
#include <GL/glut.h>
#include <cstdio>

void fpsUpdate(GameContext& ctx)
{
    static int frameCount = 0;
    static int previousTime = 0;
    frameCount++;
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (currentTime - previousTime > 1000)
    {
        ctx.fps = frameCount; 
        frameCount = 0;
        previousTime = currentTime;
    }
}

void fpsRender(GameContext& ctx, const char *baseTitle)
{
    char titulo[128];
    std::snprintf(titulo, sizeof(titulo), "%s (%d FPS)", baseTitle, ctx.fps);
    glutSetWindowTitle(titulo);
}