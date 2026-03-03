#include "phases/Phase2.h"
#include "core/GameContext.h"
#include "core/Camera.h"
#include "graphics/RenderLighting.h"
#include "graphics/Drawlevel.h"
#include "graphics/SkyBox.h"
#include "graphics/Hud.h"
#include "core/Config.h"
#include "entities/Obstacle.h"
#include "entities/Snake.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>

void Phase2::init(GameContext& ctx)
{
    reset(ctx);
    ctx.audio.startAmbientPhase2(); 
    ctx.audio.playSnakeRattle();     
}

void Phase2::reset(GameContext& ctx)
{
    ctx.canoeX             = 0.0f;
    ctx.canoeZ             = 0.0f;
    ctx.canoeSpeed         = 5.0f;
    ctx.canoeHealth        = 3;
    ctx.phase2Timer        = 0.0f;
    ctx.obstacleSpawnTimer = 0.0f;
    ctx.waterUV            = 0.0f;
    ctx.obstacles.clear();

    ctx.snake.x     = ctx.canoeX;
    ctx.snake.z     = ctx.canoeZ + 15.0f;
    ctx.snake.speed = 3.0f;

    ctx.camX  = ctx.canoeX;
    ctx.camY  = 2.0f;
    ctx.camZ  = ctx.canoeZ + 6.0f;
    ctx.yaw   = 180.0f;
    ctx.pitch = -10.0f;
    updateCamera(ctx);
}

void Phase2::onExit(GameContext& ctx)
{
    ctx.audio.stopAmbientPhase2();
}

void Phase2::update(GameContext& ctx, float dt)
{
    ctx.audio.updateListener(ctx.camX, ctx.camY, ctx.camZ, ctx.yaw, ctx.pitch);

    updateCanoe(ctx, dt);
    updateObstacles(ctx, dt);
    updateSnake(ctx, dt);

    if (ctx.phase2Timer >= 60.0f && !transitioning)
    {
        transitioning    = true;
        ctx.inTransition = true;
        fade             = 0.0f;
    }

    if (transitioning)
    {
        fade += dt * 1.5f;
        if (fade >= 1.0f)
        {
            fade             = 1.0f;
            ctx.inTransition = false;
            ctx.requestPhase3 = true;
        }
    }
}

void Phase2::updateCanoe(GameContext& ctx, float dt)
{
    ctx.phase2Timer += dt;
    float speedMultiplier = 1.0f + ctx.phase2Timer * 0.1f;
    ctx.canoeSpeed = 5.0f * speedMultiplier;

    float lateralSpeed = 6.0f;
    if (ctx.keyA) ctx.canoeX -= lateralSpeed * dt;
    if (ctx.keyD) ctx.canoeX += lateralSpeed * dt;
    ctx.canoeX = std::max(ctx.riverMinX + 0.75f, std::min(ctx.riverMaxX - 0.75f, ctx.canoeX));

    float currentSpeed = ctx.canoeSpeed;
    if (ctx.keyS) currentSpeed *= 0.3f;
    ctx.canoeZ -= currentSpeed * dt;

    // Atualiza UV da água aqui, não no render
    ctx.waterUV += dt * ctx.canoeSpeed * 0.10f;

    ctx.camX = ctx.canoeX;
    ctx.camY = 2.0f;
    ctx.camZ = ctx.canoeZ;
    updateCamera(ctx);
}

void Phase2::updateObstacles(GameContext& ctx, float dt)
{
    ctx.obstacleSpawnTimer -= dt;
    if (ctx.obstacleSpawnTimer <= 0.0f)
    {
        float spawnInterval = std::max(0.5f, 2.0f - ctx.phase2Timer * 0.02f);
        ctx.obstacleSpawnTimer = spawnInterval;

        int count = std::min(1 + (int)(ctx.phase2Timer / 20.0f), 3);
        for (int i = 0; i < count; i++)
        {
            Obstacle obs;
            obs.x      = ctx.riverMinX + ((float)rand() / RAND_MAX) * (ctx.riverMaxX - ctx.riverMinX);
            obs.z      = ctx.canoeZ - 20.0f - i * 3.0f;
            obs.type   = (rand() % 2 == 0) ? ObstacleType::LOG : ObstacleType::ROCK;
            obs.active = true;
            ctx.obstacles.push_back(obs);
        }
    }

    ctx.obstacles.erase(
        std::remove_if(ctx.obstacles.begin(), ctx.obstacles.end(),
            [&](const Obstacle& o) { return o.z > ctx.canoeZ + 15.0f; }),
        ctx.obstacles.end()
    );

    float canoeHalfW = 0.75f;
    float canoeHalfL = 1.5f;
    float obsHalf    = 0.4f;

    for (auto& obs : ctx.obstacles)
    {
        if (!obs.active) continue;
        bool collisionX = std::abs(ctx.canoeX - obs.x) < (canoeHalfW + obsHalf);
        bool collisionZ = std::abs(ctx.canoeZ - obs.z) < (canoeHalfL + obsHalf);
        if (collisionX && collisionZ)
        {
            obs.active = false;
            ctx.canoeHealth--;
            ctx.audio.playBoatHit();
            if (ctx.canoeHealth <= 0)
            {
                ctx.audio.playSnakeAttack();
                ctx.gameOverReason = GameOverReason::PHASE2_FELL;
                ctx.currentState = GameState::GAME_OVER;
            }
        }
    }
}

void Phase2::updateSnake(GameContext& ctx, float dt)
{
    float snakeDx   = ctx.canoeX - ctx.snake.x;
    float snakeDz   = ctx.canoeZ - ctx.snake.z;
    float snakeDist = std::sqrt(snakeDx*snakeDx + snakeDz*snakeDz);

    if (snakeDist > 0.01f)
    {
        float snakeSpeed = ctx.canoeSpeed;
        ctx.snake.x += (snakeDx / snakeDist) * snakeSpeed * dt;
        ctx.snake.z += (snakeDz / snakeDist) * snakeSpeed * dt;
    }

    float canoeHalfW  = 0.75f;
    float canoeHalfL  = 1.5f;
    float snakeRadius = 0.5f;

    bool collisionX = std::abs(ctx.canoeX - ctx.snake.x) < (canoeHalfW + snakeRadius);
    bool collisionZ = std::abs(ctx.canoeZ - ctx.snake.z) < (canoeHalfL + snakeRadius);
    if (collisionX && collisionZ)
    {
        ctx.audio.playSnakeAttack();
        ctx.gameOverReason = GameOverReason::PHASE2_SNAKE;
        ctx.currentState = GameState::GAME_OVER;
    }
}

void Phase2::render(GameContext& ctx)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0f, (float)ctx.janelaW / ctx.janelaH, 0.1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        ctx.camX, ctx.camY, ctx.camZ,
        ctx.camX + ctx.camDirX,
        ctx.camY + ctx.camDirY,
        ctx.camZ + ctx.camDirZ,
        0.0f, 1.0f, 0.0f
    );

    setupLightingPhase2(ctx);

    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawSkydome(ctx, ctx.camX, ctx.camY, ctx.camZ);
    glEnable(GL_FOG);
    glEnable(GL_LIGHTING);

    renderWater(ctx);
    renderWorld(ctx);
    renderHUD(ctx);
    renderFade(ctx);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Phase2::renderWater(GameContext& ctx)
{
    float tileRepeat = 8.0f;
    float uvOffset   = ctx.waterUV;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ctx.waterTexture);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, uvOffset + tileRepeat); glVertex3f(ctx.riverMinX, GameConfig::EPS_Y, ctx.camZ - 50.0f);
        glTexCoord2f(1.0f, uvOffset + tileRepeat); glVertex3f(ctx.riverMaxX, GameConfig::EPS_Y, ctx.camZ - 50.0f);
        glTexCoord2f(1.0f, uvOffset);              glVertex3f(ctx.riverMaxX, GameConfig::EPS_Y, ctx.camZ + 20.0f);
        glTexCoord2f(0.0f, uvOffset);              glVertex3f(ctx.riverMinX, GameConfig::EPS_Y, ctx.camZ + 20.0f);
    glEnd();
}

void Phase2::renderWorld(GameContext& ctx)
{
    renderObstacles(ctx);
    renderSnake(ctx);
    renderCanoe(ctx);

    // Árvores nas margens
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

    float startZ = ctx.camZ - 50.0f;
    int treeCount = 25;
    float mapStartZ = ctx.camZ - 50.0f;
    float mapEndZ   = ctx.camZ + 22.0f;

    for (int i = 0; i < treeCount; i++)
    {
        float tz = startZ + i * GameConfig::TILE;
        drawTree(ctx.riverMinX - GameConfig::TILE * 0.5f, tz, ctx.treeTexture, ctx.camX, ctx.camZ);
        drawTree(ctx.riverMaxX + GameConfig::TILE * 0.5f, tz, ctx.treeTexture, ctx.camX, ctx.camZ);
    }

    int widthCount = (ctx.riverMaxX - ctx.riverMinX) / GameConfig::TILE;

    for (int i = -1; i <= widthCount + 1; i++)
    {
        float tx = ctx.riverMinX + i * GameConfig::TILE;

        // Parede da frente
        drawTree(tx, mapStartZ, ctx.treeTexture, ctx.camX, ctx.camZ);

        // Parede de trás
        drawTree(tx, mapEndZ, ctx.treeTexture, ctx.camX, ctx.camZ);
    }

    glEnable(GL_LIGHTING);
}

void Phase2::renderHUD(GameContext& ctx)
{
    drawPhase2HUD(ctx);
}

void Phase2::renderFade(GameContext& ctx)
{
    if (fade <= 0.0f) return;
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor4f(0.0f, 0.0f, 0.0f, fade);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(ctx.janelaW, 0);
        glVertex2f(ctx.janelaW, ctx.janelaH);
        glVertex2f(0, ctx.janelaH);
    glEnd();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Phase2::renderObstacles(GameContext& ctx)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    for (const auto& obs : ctx.obstacles)
    {
        if (!obs.active) continue;

        glPushMatrix();
        glTranslatef(obs.x, 0.3f, obs.z);

        if (obs.type == ObstacleType::ROCK)
        {
            glBindTexture(GL_TEXTURE_2D, ctx.rockTexture);
            glColor3f(1.0f, 1.0f, 1.0f);
            glScalef(0.8f, 0.5f, 0.7f);
            gluSphere(ctx.sphereQuadric, 0.6f, 16, 16);
        }
        else // LOG
        {
            glBindTexture(GL_TEXTURE_2D, ctx.logTexture);
            glColor3f(0.65f, 0.45f, 0.25f);
            drawTexturedBox(0.0f, 0.0f, 0.0f, 0.5f, 0.35f, 1.8f);
        }

        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
}

void Phase2::renderSnake(GameContext& ctx)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawSpriteAt(ctx.snake.x, 0.0f, ctx.snake.z, 1.67f, 3.0f, ctx.texSnake, ctx.camX, ctx.camZ);
    glEnable(GL_LIGHTING);
}

void Phase2::renderCanoe(GameContext& ctx)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ctx.texCanoa);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTexturedBox(ctx.canoeX, 0.2f, ctx.canoeZ, 1.5f, 0.4f, 3.0f);
    glDisable(GL_TEXTURE_2D);
}