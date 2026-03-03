#include "graphics/Drawlevel.h"
#include "graphics/Texture.h"
#include "graphics/Raycaster.h"
#include "core/GameContext.h"
#include "core/Config.h"
#include "entities/Enemy.h"
#include "level/MapLoader.h"
#include "level/LevelMetrics.h"
#include <GL/glut.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>

static void drawFloorQuad(float x, float z)
{
    float half = GameConfig::TILE * 0.5f;
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - half, GameConfig::EPS_Y, z + half);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + half, GameConfig::EPS_Y, z + half);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + half, GameConfig::EPS_Y, z - half);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - half, GameConfig::EPS_Y, z - half);
    glEnd();
}

static void drawWallCube(float x, float z)
{
    float half = GameConfig::TILE * 0.5f;
    float h    = GameConfig::WALL_H;

    glBegin(GL_QUADS);

    // Frente
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(x - half, 0, z + half);
    glTexCoord2f(1, 0); glVertex3f(x + half, 0, z + half);
    glTexCoord2f(1, 1); glVertex3f(x + half, h, z + half);
    glTexCoord2f(0, 1); glVertex3f(x - half, h, z + half);
    // Trás
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(x + half, 0, z - half);
    glTexCoord2f(1, 0); glVertex3f(x - half, 0, z - half);
    glTexCoord2f(1, 1); glVertex3f(x - half, h, z - half);
    glTexCoord2f(0, 1); glVertex3f(x + half, h, z - half);
    // Direita
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(x + half, 0, z + half);
    glTexCoord2f(1, 0); glVertex3f(x + half, 0, z - half);
    glTexCoord2f(1, 1); glVertex3f(x + half, h, z - half);
    glTexCoord2f(0, 1); glVertex3f(x + half, h, z + half);
    // Esquerda
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(x - half, 0, z - half);
    glTexCoord2f(1, 0); glVertex3f(x - half, 0, z + half);
    glTexCoord2f(1, 1); glVertex3f(x - half, h, z + half);
    glTexCoord2f(0, 1); glVertex3f(x - half, h, z - half);
    // Topo
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(x - half, h, z + half);
    glTexCoord2f(1, 0); glVertex3f(x + half, h, z + half);
    glTexCoord2f(1, 1); glVertex3f(x + half, h, z - half);
    glTexCoord2f(0, 1); glVertex3f(x - half, h, z - half);
    glEnd();

    glEnd();
}

void drawTexturedBox(float x, float y, float z, float w, float h, float d)
{
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    float hd = d * 0.5f;
    glBegin(GL_QUADS);
        // Frente
        glNormal3f(0,0,1);
        glTexCoord2f(0,0); glVertex3f(x-hw, y-hh, z+hd);
        glTexCoord2f(1,0); glVertex3f(x+hw, y-hh, z+hd);
        glTexCoord2f(1,1); glVertex3f(x+hw, y+hh, z+hd);
        glTexCoord2f(0,1); glVertex3f(x-hw, y+hh, z+hd);
        // Trás
        glNormal3f(0,0,-1);
        glTexCoord2f(0,0); glVertex3f(x+hw, y-hh, z-hd);
        glTexCoord2f(1,0); glVertex3f(x-hw, y-hh, z-hd);
        glTexCoord2f(1,1); glVertex3f(x-hw, y+hh, z-hd);
        glTexCoord2f(0,1); glVertex3f(x+hw, y+hh, z-hd);
        // Topo
        glNormal3f(0,1,0);
        glTexCoord2f(0,0); glVertex3f(x-hw, y+hh, z-hd);
        glTexCoord2f(1,0); glVertex3f(x+hw, y+hh, z-hd);
        glTexCoord2f(1,1); glVertex3f(x+hw, y+hh, z+hd);
        glTexCoord2f(0,1); glVertex3f(x-hw, y+hh, z+hd);
        // Base
        glNormal3f(0,-1,0);
        glTexCoord2f(0,0); glVertex3f(x-hw, y-hh, z+hd);
        glTexCoord2f(1,0); glVertex3f(x+hw, y-hh, z+hd);
        glTexCoord2f(1,1); glVertex3f(x+hw, y-hh, z-hd);
        glTexCoord2f(0,1); glVertex3f(x-hw, y-hh, z-hd);
        // Direita
        glNormal3f(1,0,0);
        glTexCoord2f(0,0); glVertex3f(x+hw, y-hh, z+hd);
        glTexCoord2f(1,0); glVertex3f(x+hw, y-hh, z-hd);
        glTexCoord2f(1,1); glVertex3f(x+hw, y+hh, z-hd);
        glTexCoord2f(0,1); glVertex3f(x+hw, y+hh, z+hd);
        // Esquerda
        glNormal3f(-1,0,0);
        glTexCoord2f(0,0); glVertex3f(x-hw, y-hh, z-hd);
        glTexCoord2f(1,0); glVertex3f(x-hw, y-hh, z+hd);
        glTexCoord2f(1,1); glVertex3f(x-hw, y+hh, z+hd);
        glTexCoord2f(0,1); glVertex3f(x-hw, y+hh, z-hd);
    glEnd();
}

void drawSpriteAt(float x, float y, float z, float hw, float h2,
                          unsigned int tex, float camX, float camZ)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPushMatrix();
    glTranslatef(x, y, z);
    float dx    = camX - x;
    float dz    = camZ - z;
    float angle = std::atan2(dx, dz) * 180.0f / 3.14159f;
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-hw, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( hw, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( hw,  h2,  0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-hw,  h2,  0.0f);
    glEnd();
    glPopMatrix();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}

static void drawCeilQuad(float x, float z)
{
    float half = GameConfig::TILE * 0.5f;
    float h    = GameConfig::WALL_H;
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0,0); glVertex3f(x - half, h, z - half);
    glTexCoord2f(1,0); glVertex3f(x + half, h, z - half);
    glTexCoord2f(1,1); glVertex3f(x + half, h, z + half);
    glTexCoord2f(0,1); glVertex3f(x - half, h, z + half);
    glEnd();
}

void drawEnemies(GameContext& ctx, const std::vector<Enemy>& enemies)
{
    for (const auto& enemy : enemies)
    {
        GLuint tex = enemy.isStunned() 
            ? ctx.enemyStunnedTexture 
            : ctx.enemyTexture;

        drawSpriteAt(
            enemy.getX(), enemy.getY(), enemy.getZ(),
            3.0f, 2.0f,
            tex,
            ctx.camX, ctx.camZ
        );
    }
}

void drawTree(float x, float z, GLuint texture, float camX, float camZ)
{
    float half = GameConfig::TILE * 0.5f;
    float h    = GameConfig::WALL_H * 1.0f;

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);

    float base = GameConfig::EPS_Y;

    glBegin(GL_QUADS);

    // Frente
    glNormal3f(0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(x - half, base,   z + half);
    glTexCoord2f(1,0); glVertex3f(x + half, base,   z + half);
    glTexCoord2f(1,1); glVertex3f(x + half, base+h, z + half);
    glTexCoord2f(0,1); glVertex3f(x - half, base+h, z + half);

    // Trás
    glNormal3f(0, 0, -1);
    glTexCoord2f(0,0); glVertex3f(x + half, base,   z - half);
    glTexCoord2f(1,0); glVertex3f(x - half, base,   z - half);
    glTexCoord2f(1,1); glVertex3f(x - half, base+h, z - half);
    glTexCoord2f(0,1); glVertex3f(x + half, base+h, z - half);

    // Direita
    glNormal3f(1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(x + half, base,   z + half);
    glTexCoord2f(1,0); glVertex3f(x + half, base,   z - half);
    glTexCoord2f(1,1); glVertex3f(x + half, base+h, z - half);
    glTexCoord2f(0,1); glVertex3f(x + half, base+h, z + half);

    // Esquerda
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(x - half, base,   z - half);
    glTexCoord2f(1,0); glVertex3f(x - half, base,   z + half);
    glTexCoord2f(1,1); glVertex3f(x - half, base+h, z + half);
    glTexCoord2f(0,1); glVertex3f(x - half, base+h, z - half);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

std::vector<TileRenderInfo> buildLevelRenderInfo(const MapLoader& map)
{
    std::vector<TileRenderInfo> tiles;
    LevelMetrics m = LevelMetrics::fromMap(map, GameConfig::TILE);
    int H = map.getHeight();
    for (int z = 0; z < H; z++)
    {
        for (int x = 0; x < (int)map.data()[z].size(); x++)
        {
            float wx, wz;
            m.tileCenter(x, z, wx, wz);
            char c = map.data()[z][x];
            tiles.push_back({wx, wz, c, x, z});
        }
    }
    return tiles;
}

void renderLevel(GameContext& ctx, const MapLoader& map, bool useRaycast)
{
    auto tiles = buildLevelRenderInfo(map);

    if (useRaycast)
        castAllRays(ctx);
    
    for (const auto& tile : tiles)
    {
        if (useRaycast && ctx.visibleTiles.find({tile.x, tile.z}) == ctx.visibleTiles.end())
            continue;
        switch (tile.tipo)
        {
            case '0': case 'E': case 'T': case 'A':
            case 'B': case 'O': case 'W': case 'J':
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, ctx.floorTexture);
                glColor3f(1.0f, 1.0f, 1.0f);
                drawFloorQuad(tile.wx, tile.wz);
                break;
            case '1':
                glDisable(GL_TEXTURE_2D);
                glColor3f(0.5f, 0.35f, 0.2f);
                drawWallCube(tile.wx, tile.wz);
                break;
            case 'M':
                glUseProgram(0);
                glDisable(GL_TEXTURE_2D);
                glColor3f(0.0f, 0.3f, 0.7f);
                drawFloorQuad(tile.wx, tile.wz);
                glUseProgram(ctx.progLighting); // reativa
                break;
            case 'P':
            {
                glUseProgram(0);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, ctx.texWall);
                glColor3f(1.0f, 1.0f, 1.0f);
                drawWallCube(tile.wx, tile.wz);
                glUseProgram(ctx.progLighting);
                // Teto
                glBindTexture(GL_TEXTURE_2D, ctx.floorTexture);
                glColor3f(0.5f, 0.4f, 0.3f);
                drawCeilQuad(tile.wx, tile.wz);
                break;
            }
            case 'C':
            {
                // Chão embaixo
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, ctx.floorTexture);
                glColor3f(1.0f, 1.0f, 1.0f);
                drawFloorQuad(tile.wx, tile.wz);
                // Teto
                glBindTexture(GL_TEXTURE_2D, ctx.texCeiling);
                glColor3f(1.0f, 1.0f, 1.0f);
                drawCeilQuad(tile.wx, tile.wz);
                break;
            }
            case 'G':
                // Chão normal
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, ctx.floorTexture);
                glColor3f(1.0f, 1.0f, 1.0f);
                drawFloorQuad(tile.wx, tile.wz);
                // Teto acima
                glBindTexture(GL_TEXTURE_2D, ctx.texCeiling);
                glColor3f(1.0f, 1.0f, 1.0f);
                drawCeilQuad(tile.wx, tile.wz);
                break;
        }
    }
}

void drawLevelTrees(GameContext& ctx, const MapLoader& map)
{
    auto tiles = buildLevelRenderInfo(map);
    for (const auto& tile : tiles)
        if (tile.tipo == 'T')
            drawTree(tile.wx, tile.wz, ctx.treeTexture, ctx.camX, ctx.camZ);
}