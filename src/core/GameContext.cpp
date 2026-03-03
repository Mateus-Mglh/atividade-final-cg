#include "core/GameContext.h"
#include "core/Camera.h"
#include "core/Config.h"
#include "level/Level.h"
#include "graphics/Texture.h"
#include "utils/Assets.h"
#include <GL/glut.h>
#include <cmath>

void initGameContext(GameContext& ctx)
{
    ctx.janelaW = GameConfig::WINDOW_W;
    ctx.janelaH = GameConfig::WINDOW_H;
    ctx.centerX = GameConfig::WINDOW_W / 2;
    ctx.centerY = GameConfig::WINDOW_H / 2;
    ctx.fullScreen = false;
    ctx.ignoreWarp = false;
    ctx.firstMouse = true;
    ctx.running = true;
    ctx.keyW = ctx.keyS = ctx.keyA = ctx.keyD = false;  // ← inicializa input
    ctx.deltaTime = 0.0f;
    ctx.lastTime  = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    ctx.stones.clear();
    ctx.playerStoneCount = 0;
    ctx.stoneSpawnTimer  = 1.0f;
    ctx.playerMoved = false;
    ctx.enemyActivated = false;
    ctx.throwCooldown = 0.0f;
    ctx.currentState = GameState::MENU;

    loadAssets(ctx);

    // // Texturas
    // ctx.enemyTexture = loadTexture("assets/sprites/mapinguari.png");
    // ctx.enemyStunnedTexture = loadTexture("assets/sprites/mapinguari_atordoado.png");
    // ctx.stoneTexture = loadTexture("assets/texturas/textura_preda.png");
    // ctx.treeTexture = loadTexture("assets/texturas/textura_arvore_mapinguari.png");
    // ctx.floorTexture = loadTexture("assets/texturas/chao.png");
    // ctx.texSkydome = loadTexture("assets/texturas/Ceu_Noturno.png");
    // ctx.sphereQuadric = gluNewQuadric();

    // gluQuadricTexture(ctx.sphereQuadric, GL_TRUE);
    // gluQuadricNormals(ctx.sphereQuadric, GLU_SMOOTH);

    setCamera(ctx,
        GameConfig::CAM_START_X,
        GameConfig::CAM_START_Y,
        GameConfig::CAM_START_Z,
        0.0f, 0.0f);

    loadLevel(ctx.level, "maps/map1.txt");
    applySpawn(ctx.level, ctx.camX, ctx.camZ);

    // Faz a câmera olhar pro inimigo
    if (!ctx.level.enemies.empty())
    {
        float dx = ctx.level.enemies[0].getX() - ctx.camX;
        float dz = ctx.level.enemies[0].getZ() - ctx.camZ;
        ctx.yaw = std::atan2(dx, -dz) * 180.0f / 3.14159f;
        ctx.pitch = 0.0f;
    }

    updateCamera(ctx);
}
