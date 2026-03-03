#ifndef LEVEL_H
#define LEVEL_H
#include "level/MapLoader.h"
#include "level/LevelMetrics.h"
#include "entities/Enemy.h"
#include <vector>

struct Level
{
    MapLoader    map;
    LevelMetrics metrics;
    std::vector<Enemy> enemies;
};

bool loadLevel(Level& lvl, const char* mapPath);
void applySpawn(const Level& lvl, float& camX, float& camZ);
#endif