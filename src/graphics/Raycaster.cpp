#include "graphics/Raycaster.h"
#include "core/GameContext.h"
#include "level/LevelMetrics.h"
#include <cmath>

RayHit castRay(GameContext& ctx, float angle, std::vector<std::pair<int,int>>& visitedTiles)
{
    const LevelMetrics& m = ctx.level.metrics;
    const auto& data = ctx.level.map.data();

    float rayDirX = std::sin(angle);
    float rayDirZ = -std::cos(angle);

    float posX = (ctx.camX - m.offsetX) / m.tile;
    float posZ = (ctx.camZ - m.offsetZ) / m.tile;

    int mapX = (int)posX;
    int mapZ = (int)posZ;

    float deltaDistX = std::abs(1.0f / (rayDirX == 0 ? 1e-6f : rayDirX));
    float deltaDistZ = std::abs(1.0f / (rayDirZ == 0 ? 1e-6f : rayDirZ));

    float sideDistX, sideDistZ;
    int stepX, stepZ;

    if (rayDirX < 0) { stepX = -1; sideDistX = (posX - mapX) * deltaDistX; }
    else             { stepX =  1; sideDistX = (mapX + 1.0f - posX) * deltaDistX; }
    if (rayDirZ < 0) { stepZ = -1; sideDistZ = (posZ - mapZ) * deltaDistZ; }
    else             { stepZ =  1; sideDistZ = (mapZ + 1.0f - posZ) * deltaDistZ; }

    bool hit  = false;
    bool side = false;
    int maxSteps = 64;

    // Adiciona tile inicial (posição do jogador)
    visitedTiles.push_back({mapX, mapZ});

    while (!hit && maxSteps-- > 0)
    {
        if (sideDistX < sideDistZ) { sideDistX += deltaDistX; mapX += stepX; side = false; }
        else                       { sideDistZ += deltaDistZ; mapZ += stepZ; side = true;  }

        if (mapZ < 0 || mapZ >= (int)data.size()) break;
        if (mapX < 0 || mapX >= (int)data[mapZ].size()) break;

        visitedTiles.push_back({mapX, mapZ}); // ← guarda cada tile visitado

        char c = data[mapZ][mapX];
        if (c == '1' || c == 'T') hit = true;
    }

    float dist = side
        ? std::abs((mapZ - posZ + (1 - stepZ) / 2.0f) / rayDirZ)
        : std::abs((mapX - posX + (1 - stepX) / 2.0f) / rayDirX);

    return { mapX, mapZ, dist * m.tile, hit };
}

std::vector<RayHit> castAllRays(GameContext& ctx)
{
    float yawRad = ctx.yaw * 3.14159f / 180.0f;
    float fov    = 180.0f  * 3.14159f / 180.0f;
    int   nRays  = 300;

    std::vector<RayHit> hits;
    hits.reserve(nRays);

    ctx.visibleTiles.clear(); // ← guarda no contexto

    for (int i = 0; i < nRays; i++)
    {
        float angle = yawRad - fov / 2.0f + fov * ((float)i / nRays);
        std::vector<std::pair<int,int>> visited;
        hits.push_back(castRay(ctx, angle, visited));
        for (auto& t : visited)
            ctx.visibleTiles.insert(t);
    }

    return hits;
}