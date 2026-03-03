#include "systems/RainSystem.h"
#include "core/GameContext.h"
#include <GL/glut.h>
#include <cstdlib>

void RainSystem::init(int numDrops, float mapSize, float maxHeight)
{
    rainDrops.clear();
    float half = mapSize * 0.5f;
    for (int i = 0; i < numDrops; ++i)
    {
        rainDrops.push_back({
            ((float)rand() / RAND_MAX) * mapSize - half,  // -22.5 até +22.5
            ((float)rand() / RAND_MAX) * maxHeight,
            ((float)rand() / RAND_MAX) * mapSize - half
        });
    }
}

void RainSystem::update(float dt, float maxHeight)
{
    for (auto& drop : rainDrops)
    {
        drop.y -= dt * speed;
        if (drop.y < 0) drop.y = maxHeight;
    }
}

void RainSystem::render(GameContext& ctx)
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.8f, 0.8f, 1.0f, 0.3f);
    glBegin(GL_LINES);
    for (auto& drop : rainDrops)
    {
        glVertex3f(drop.x, drop.y, drop.z);
        glVertex3f(drop.x, drop.y - length, drop.z);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
}