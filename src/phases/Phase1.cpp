#include "phases/Phase1.h"
#include "core/GameContext.h"
#include "core/Camera.h"
#include "core/movement.h"
#include "graphics/Drawlevel.h"
#include "graphics/SkyBox.h"
#include "graphics/RenderLighting.h"
#include "core/Config.h"
#include "utils/Collision.h"
#include "systems/AmuletSystem.h"
#include "systems/ItemSystem.h"
#include "graphics/Hud.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

void Phase1::init(GameContext& ctx)
{
    srand((unsigned int)time(NULL));
    reset(ctx);

    // Orienta câmera para o inimigo inicial
    if (!ctx.level.enemies.empty())
    {
        float dx = ctx.level.enemies[0].getX() - ctx.camX;
        float dz = ctx.level.enemies[0].getZ() - ctx.camZ;
        ctx.yaw   = std::atan2(dx, -dz) * 180.0f / 3.14159f;
        ctx.pitch = 0.0f;
        updateCamera(ctx);
    }

    // ── Áudio ──────────────────────────────────
    ctx.audio.startAmbientPhase1(); // chuva + floresta em loop

    if (!ctx.level.enemies.empty())
        ctx.audio.startMapinguariIntro();
}

void Phase1::reset(GameContext& ctx)
{
    state            = Phase1State::EXPLORE;
    fadeAlpha        = 0.0f;
    sealTimer        = 0.0f;
    sealingTriggered = false;
    sealTextVisible  = false;

    ctx.playerStoneCount = 0;
    ctx.throwCooldown    = 0.0f;
    ctx.amuletsCollected = 0;
    ctx.enemyActivated   = false;

    ctx.camY  = 1.0f;
    ctx.yaw   = 0.0f;
    ctx.pitch = 0.0f;
    updateCamera(ctx);

    initAmulets(ctx);
    ctx.stones.clear();
    ctx.projectiles.clear();
    ctx.stoneSpawnTimer = 0.0f;
}

void Phase1::onExit(GameContext& ctx)
{
    ctx.audio.stopChuva();   
    ctx.audio.stopMapinguariLoop();
    ctx.audio.stopMapinguariScream();
    ctx.audio.audioUpdateStep(false);
    ctx.audio.processStepAudio(); 
}

void Phase1::update(GameContext& ctx, float dt)
{
    ctx.audio.updateListener(
        ctx.camX, ctx.camY, ctx.camZ,
        ctx.yaw, ctx.pitch
    );

    ctx.audio.update(dt, ctx.camX, ctx.camZ, {}, 100, 
                     ctx.keyW || ctx.keyA || ctx.keyS || ctx.keyD, ctx.inTransition);

    switch (state)
    {
        case Phase1State::EXPLORE:      updateExplore(ctx, dt);      break;
        case Phase1State::BOSS_SEALING: updateBossSealing(ctx, dt);  break;
        case Phase1State::TRANSITION:   updateTransition(ctx, dt);   break;
    }

    // Distância do player até o Mapinguari
    if (!ctx.level.enemies.empty())
    {
        float dx   = ctx.level.enemies[0].getX() - ctx.camX;
        float dz   = ctx.level.enemies[0].getZ() - ctx.camZ;
        float dist = std::sqrt(dx*dx + dz*dz);
        ctx.audio.tickMapinguariScream(dt, dist);
    }
}

void Phase1::updateExplore(GameContext& ctx, float dt)
{
    updateMovement(ctx);
    updateAmulets(ctx);
    updateStones(ctx);
    updateProjectiles(ctx);
    ctx.rain.update(dt, 20.0f);

    ctx.stoneSpawnTimer -= dt;
    if (ctx.stoneSpawnTimer <= 0.0f)
    {
        trySpawnStone(ctx);
        ctx.stoneSpawnTimer = 3.0f;
    }

    if (ctx.throwCooldown > 0.0f)
        ctx.throwCooldown = std::max(0.0f, ctx.throwCooldown - dt);

    for (auto& enemy : ctx.level.enemies)
    {
        enemy.update(ctx, dt);
        // Mantém o som 3D colado na posição do Mapinguari
        ctx.audio.setMapinguariPos(enemy.getX(), enemy.getZ());
    }

    // Quando o Mapinguari é ativado pela primeira vez, toca o grito
    if (ctx.enemyActivated && !mapinguariScreamPlayed)
    {
        mapinguariScreamPlayed = true;
        ctx.audio.startMapinguariIntro();
    }

    if (ctx.amuletsCollected >= 3 && !sealingTriggered)
    {
        sealingTriggered = true;
        state = Phase1State::BOSS_SEALING;
    }
}

void Phase1::updateBossSealing(GameContext& ctx, float dt)
{
    sealTimer += dt;
    ctx.keyW = ctx.keyA = ctx.keyS = ctx.keyD = false;

    // Prende câmera no inimigo
    if (!ctx.level.enemies.empty())
    {
        float ex = ctx.level.enemies[0].getX() - ctx.camX;
        float ey = ctx.level.enemies[0].getY() + 1.0f - ctx.camY;
        float ez = ctx.level.enemies[0].getZ() - ctx.camZ;
        ctx.yaw   = std::atan2(ex, -ez) * 180.0f / 3.14159f;
        ctx.pitch = std::atan2(ey, std::sqrt(ex*ex + ez*ez)) * 180.0f / 3.14159f;
        updateCamera(ctx);
    }

    // 0s-1s: câmera treme
    if (sealTimer < 1.0f)
    {
        ctx.yaw   += ((rand() % 3) - 1) * 0.3f;
        ctx.pitch += ((rand() % 3) - 1) * 0.15f;
        updateCamera(ctx);
    }

    // 0.5s: toca grito de selamento (mapinguari_apresentacao reutilizado)
    if (sealTimer >= 0.5f && !sealSoundPlayed)
    {
        sealSoundPlayed = true;
        ctx.audio.stopMapinguariScream(); // para o timer de gritos periódicos
        ctx.audio.playMapinguariScream(); // toca o grito de selamento
        ctx.audio.stopMapinguariLoop();// para o loop de perseguição
    }

    // 0.5s-3s: inimigo afunda
    if (sealTimer >= 0.5f && sealTimer < 3.0f)
    {
        for (auto& enemy : ctx.level.enemies)
            enemy.sinkDown(dt);
    }

    // 3s+: texto aparece e fade começa
    if (sealTimer >= 3.0f)
    {
        sealTextVisible = true;
        fadeAlpha = std::min(fadeAlpha + dt * 15.0f, 1.0f);
    }

    if (sealTimer >= 3.2f)
        state = Phase1State::TRANSITION;
}

void Phase1::updateTransition(GameContext& ctx, float dt)
{
    ctx.inTransition = true;
    transitionTimer += dt;
    if (transitionTimer >= 6.0f)
    {
        transitionTimer  = 0.0f;
        fadeAlpha        = 0.0f;
        ctx.inTransition = false;
        ctx.requestPhase2 = true;
    }
}

void Phase1::render(GameContext& ctx)
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

    renderWorld(ctx);
    renderHUD(ctx);
    renderFade(ctx);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Phase1::renderWorld(GameContext& ctx)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    GLfloat lightPos[] = { 0.5f, 1.0f, 0.3f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Sky
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glColor3f(0.3f, 0.3f, 0.4f);
    drawSkydome(ctx, ctx.camX, ctx.camY, ctx.camZ);
    glColor3f(1.0f, 1.0f, 1.0f);
    ctx.rain.render(ctx);
    glEnable(GL_FOG);
    glEnable(GL_LIGHTING);

    // Mapa
    glUseProgram(ctx.progLighting);
    setupLightingPhase1(ctx);
    glUniform1i(glGetUniformLocation(ctx.progLighting, "uUseTexture"), 1);
    glUniform1i(glGetUniformLocation(ctx.progLighting, "uTexture"), 0);
    setupAmuletLighting(ctx);
    renderLevel(ctx, ctx.level.map, true);
    glUseProgram(0);

    // Entidades
    renderAmulets(ctx);
    renderStones(ctx);
    renderProjectiles(ctx);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_LIGHTING);
    drawEnemies(ctx, ctx.level.enemies);
    glEnable(GL_LIGHTING);

    drawLevelTrees(ctx, ctx.level.map);
}

void Phase1::renderHUD(GameContext& ctx)
{
    drawPhase1HUD(ctx);
}

void Phase1::renderFade(GameContext& ctx)
{
    if (fadeAlpha <= 0.0f) return;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor4f(0.0f, 0.0f, 0.0f, fadeAlpha);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(ctx.janelaW, 0);
        glVertex2f(ctx.janelaW, ctx.janelaH);
        glVertex2f(0, ctx.janelaH);
    glEnd();

    // Texto do sealing
    if (sealTextVisible)
    {
        glColor4f(1.0f, 1.0f, 1.0f, fadeAlpha);
        const char* text = "O espirito foi selado.";
        int textWidth = 0;
        for (const char* c = text; *c != '\0'; c++)
            textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
        glRasterPos2f((ctx.janelaW - textWidth) / 2.0f, ctx.janelaH / 2.0f);
        for (const char* c = text; *c != '\0'; c++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_FOG);
}

void Phase1::renderAmulets(GameContext& ctx)
{
    float time = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    for (const auto& amulet : ctx.amulets)
    {
        if (amulet.collected) continue;

        float bob = 0.15f * std::sin(time * 2.0f);

        // Luz emanando do amuleto
        GLfloat lightPos[]     = { amulet.x, 0.5f + bob, amulet.z, 1.0f }; // w=1 = pontual
        GLfloat lightDiffuse[] = { 0.4f, 0.3f, 0.1f, 1.0f }; // tom dourado fraco
        GLfloat lightAmbient[] = { 0.05f, 0.04f, 0.01f, 1.0f };
        GLfloat attenuation    = 0.3f; // quanto cai com distância

        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_POSITION,             lightPos);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,              lightDiffuse);
        glLightfv(GL_LIGHT1, GL_AMBIENT,              lightAmbient);
        glLightf (GL_LIGHT1, GL_QUADRATIC_ATTENUATION, attenuation);

        drawSpriteAt(
            amulet.x, bob, amulet.z,
            0.5f, 1.0f,
            ctx.amuletTexture,
            ctx.camX, ctx.camZ
        );
    }

    // Desliga a luz ao terminar
    glDisable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
}

void Phase1::setupAmuletLighting(GameContext& ctx)
{
    int amuletCount = 0;
    float mx[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mx);

    for (const auto& amulet : ctx.amulets)
    {
        if (amulet.collected) continue;

        float ax = mx[0]*amulet.x + mx[4]*0.5f + mx[8]*amulet.z  + mx[12];
        float ay = mx[1]*amulet.x + mx[5]*0.5f + mx[9]*amulet.z  + mx[13];
        float az = mx[2]*amulet.x + mx[6]*0.5f + mx[10]*amulet.z + mx[14];

        char name[32];
        sprintf(name, "uAmuletPos[%d]", amuletCount);
        glUniform3f(glGetUniformLocation(ctx.progLighting, name), ax, ay, az);
        amuletCount++;
    }
    glUniform1i(glGetUniformLocation(ctx.progLighting, "uAmuletCount"), amuletCount);
}