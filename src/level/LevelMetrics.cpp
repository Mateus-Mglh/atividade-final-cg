#include "level/LevelMetrics.h"
#include "level/MapLoader.h"

LevelMetrics LevelMetrics::fromMap(const MapLoader& map, float tile)
{
    LevelMetrics m;
    m.tile    = tile;
    m.offsetX = -(map.getWidth()  * tile) * 0.5f;
    m.offsetZ = -(map.getHeight() * tile) * 0.5f;
    return m;
}

void LevelMetrics::tileCenter(int tileX, int tileZ, float& outX, float& outZ) const
{
    outX = offsetX + (tileX + 0.5f) * tile;
    outZ = offsetZ + (tileZ + 0.5f) * tile;
}

void LevelMetrics::spawnPos(const MapLoader& map, float& outX, float& outZ) const
{
    int sx = (int)map.getPlayerStartX();
    int sz = (int)map.getPlayerStartZ();
    tileCenter(sx, sz, outX, outZ);
}