#include "utils/Collision.h"
#include "level/LevelMetrics.h"
#include "core/GameContext.h"
#include <cmath>
#include <algorithm>

bool isPositionWalkable(GameContext& ctx, float nx, float nz, float radius)
{
    if (ctx.level.map.getHeight() == 0)
        return true;

    const LevelMetrics& m = ctx.level.metrics;
    float tile   = m.tile;
    float localX = nx - m.offsetX;
    float localZ = nz - m.offsetZ;

    int tx = (int)std::floor(localX / tile);
    int tz = (int)std::floor(localZ / tile);

    for (int dz = -1; dz <= 1; ++dz)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            int ntx = tx + dx;
            int ntz = tz + dz;

            const auto& data = ctx.level.map.data();
            if (ntz < 0 || ntx < 0 ||
                ntz >= (int)data.size() ||
                ntx >= (int)data[ntz].size())
                return false;

            char c = data[ntz][ntx];
            if (c != '1' && c != 'T' && c != 'P')
                continue;

            float wx, wz;
            m.tileCenter(ntx, ntz, wx, wz);

            float half     = tile * 0.5f;
            float closestX = std::fmax(wx - half, std::fmin(nx, wx + half));
            float closestZ = std::fmax(wz - half, std::fmin(nz, wz + half));
            float dxTile   = nx - closestX;
            float dzTile   = nz - closestZ;

            if ((dxTile*dxTile + dzTile*dzTile) < (radius*radius))
                return false;
        }
    }
    return true;
}