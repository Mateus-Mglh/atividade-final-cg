#include "systems/AmuletSystem.h"
#include "core/GameContext.h"
#include "core/Config.h"
#include <GL/glut.h>
#include <cmath>

void initAmulets(GameContext& ctx)
{
    ctx.amulets.clear();
    ctx.amuletsCollected = 0;

    const auto& data = ctx.level.map.data();
    const LevelMetrics& m = ctx.level.metrics;

    for (int z = 0; z < (int)data.size(); z++)
    {
        for (int x = 0; x < (int)data[z].size(); x++)
        {
            if (data[z][x] == 'A')
            {
                float wx, wz;
                m.tileCenter(x, z, wx, wz);
                ctx.amulets.push_back({ wx, wz });
            }
        }
    }
}

void updateAmulets(GameContext& ctx)
{
    for (auto& amulet : ctx.amulets)
    {
        if (amulet.collected) continue;
        float dx   = ctx.camX - amulet.x;
        float dz   = ctx.camZ - amulet.z;
        if (std::sqrt(dx*dx + dz*dz) < 1.0f)
        {
            amulet.collected = true;
            ctx.amuletsCollected++;
            ctx.audio.playAmuletPickup();
        }
    }
}
