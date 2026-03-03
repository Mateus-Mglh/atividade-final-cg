#include "level/Level.h"
#include "core/Config.h"
#include <cstdio>

bool loadLevel(Level& lvl, const char* mapPath)
{
    if (!lvl.map.load(mapPath))
    {
        std::printf("ERRO: nao foi possivel carregar o mapa: %s\n", mapPath);
        return false;
    }

    lvl.metrics = LevelMetrics::fromMap(lvl.map, GameConfig::TILE);
    lvl.enemies.clear();

    const auto& data = lvl.map.data();
    for (int z = 0; z < (int)data.size(); z++)
    {
        for (int x = 0; x < (int)data[z].size(); x++)
        {
            if (data[z][x] == 'E')
            {
                float wx, wz;
                lvl.metrics.tileCenter(x, z, wx, wz);
                lvl.enemies.emplace_back(wx, wz);
            }
        }
    }
    return true;
}

void applySpawn(const Level& lvl, float& camX, float& camZ)
{
    lvl.metrics.spawnPos(lvl.map, camX, camZ);
}