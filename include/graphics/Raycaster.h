#pragma once
#include <vector>
#include <utility>
struct GameContext;
struct RayHit
{
    int   tileX, tileZ;
    float distance;
    bool  hit;
};
RayHit castRay(GameContext& ctx, float angle, std::vector<std::pair<int,int>>& visitedTiles);
std::vector<RayHit> castAllRays(GameContext& ctx);