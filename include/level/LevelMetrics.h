#ifndef LEVELMETRICS_H
#define LEVELMETRICS_H
#include "level/MapLoader.h"

struct LevelMetrics
{
    float tile    = 0.0f;
    float offsetX = 0.0f;
    float offsetZ = 0.0f;

    static LevelMetrics fromMap(const MapLoader& map, float tile);
    void tileCenter(int tileX, int tileZ, float& outX, float& outZ) const;
    void spawnPos(const MapLoader& map, float& outX, float& outZ) const;
};
#endif