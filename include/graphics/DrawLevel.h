#ifndef DRAWLEVEL_H
#define DRAWLEVEL_H
#include <GL/glew.h>
#include <vector>

struct GameContext;
class MapLoader;
class Enemy;

struct TileRenderInfo
{
    float wx, wz;
    char  tipo;
    int   x, z;
};

std::vector<TileRenderInfo> buildLevelRenderInfo(const MapLoader& map);
void renderLevel(GameContext& ctx, const MapLoader& map, bool useRaycast);
void drawLevelTrees(GameContext& ctx, const MapLoader& map);
void drawTree(float x, float z, GLuint texture, float camX, float camZ);
void drawTexturedBox(float x, float y, float z, float w, float h, float d);

// Sprites
void drawSpriteAt(float x, float y, float z, float hw, float h2, unsigned int tex, float camX, float camZ);
void drawEnemies(GameContext& ctx, const std::vector<Enemy>& enemies);
#endif