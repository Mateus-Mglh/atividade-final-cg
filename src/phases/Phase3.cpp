#include "phases/Phase3.h"

#include "core/GameContext.h"
#include "core/Camera.h"
#include "core/Config.h"
#include "core/movement.h"
#include "entities/Boto.h"
#include "entities/DropItem.h"
#include "entities/Minion.h"
#include "entities/Projectile.h"
#include "graphics/Drawlevel.h"
#include "graphics/Hud.h"
#include "graphics/RenderLighting.h"
#include "graphics/SkyBox.h"
#include "utils/Collision.h"

#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

static void updateBoto(GameContext& ctx, float dt)
{
    if (!ctx.boto.alive) return;

    float radius = GameConfig::PLAYER_RADIUS * GameConfig::TILE;

    ctx.boto.wanderTimer -= dt;
    if (ctx.boto.wanderTimer <= 0.0f)
    {
        ctx.boto.wanderTimer = 2.0f + (rand() % 30) * 0.1f;
        ctx.boto.targetX     = ctx.camX + ((rand() % 11) - 5) * 2.0f;
        ctx.boto.targetZ     = ctx.boto.z + ((rand() % 5) - 2) * 3.0f;
        ctx.boto.zigZagDir   = (ctx.boto.targetX > ctx.boto.x) ? 1.0f : -1.0f;
    }

    float newX = ctx.boto.x + (ctx.boto.targetX - ctx.boto.x) * dt * 3.5f;
    if (isPositionWalkable(ctx, newX, ctx.boto.z, radius))
        ctx.boto.x = newX;
    else
        ctx.boto.wanderTimer = 0.0f;

    float newZ = ctx.boto.z + (ctx.boto.targetZ - ctx.boto.z) * dt * 3.5f;
    if (isPositionWalkable(ctx, ctx.boto.x, newZ, radius))
        ctx.boto.z = newZ;
    else
        ctx.boto.wanderTimer = 0.0f;

    ctx.audio.setBotoPos(ctx.boto.x, ctx.boto.z);

    // Dano por proximidade
    float dx   = ctx.camX - ctx.boto.x;
    float dz   = ctx.camZ - ctx.boto.z;
    float dist = std::sqrt(dx*dx + dz*dz);
    if (dist < 1.5f)
    {
        ctx.boto.moveTimer += dt;
        if (ctx.boto.moveTimer >= 0.5f)
        {
            ctx.player.takeDamage(10); // ← era ctx.playerHealth -= 10
            ctx.audio.audioPlayHurt();
            ctx.boto.moveTimer = 0.0f;
        }
    }

    // Só projétil agora
    ctx.boto.attackTimer -= dt;
    if (ctx.boto.attackTimer <= 0.0f)
    {
        ctx.boto.attackTimer = 2.0f;
        
        // ctx.audio.playBoatoThrow();

        Projectile p;
        p.x = ctx.boto.x;
        p.y = 1.0f;
        p.z = ctx.boto.z;
        float pdx = ctx.camX - p.x;
        float pdz = ctx.camZ - p.z;
        float len = std::sqrt(pdx*pdx + pdz*pdz);
        if (len > 0.01f)
        {
            p.velX = (pdx / len) * 26.0f;
            p.velZ = (pdz / len) * 26.0f;
        }
        p.velY          = 0.0f;
        p.rotation      = 0.0f;
        p.rotationSpeed = 360.0f;
        p.active        = true;
        ctx.bossProjectiles.push_back(p);
    }
}

static void updateMinions(GameContext& ctx, float dt, float& minionSoundTimer)
{
    for (auto& m : ctx.minions)
    {
        if (!m.alive) continue;

        float dx   = ctx.camX - m.x;
        float dz   = ctx.camZ - m.z;
        float dist = std::sqrt(dx*dx + dz*dz);

        minionSoundTimer -= dt;
        if (minionSoundTimer <= 0.0f)
        {
            bool hasAlive = false;
            for (const auto& m : ctx.minions)
                if (m.alive) { hasAlive = true; break; }
            if (hasAlive)
                ctx.audio.playMinionSound();
            minionSoundTimer = 4.0f + (rand() % 3);  // 4 a 6 segundos
        }

        if (dist > 1.5f)
        {
            m.x += (dx / dist) * m.speed * dt;
            m.z += (dz / dist) * m.speed * dt;
        }

        // Dano a cada 1 segundo quando dentro de 1.5f
        if (dist <= 1.5f)
        {
            m.damageTimer += dt;
            if (m.damageTimer >= 1.0f)
            {
                ctx.player.takeDamage(10);
                ctx.audio.audioPlayHurt();
                m.damageTimer = 0.0f;
            }
        }
        else
        {
            m.damageTimer = 0.0f; // reseta se sair do alcance
        }
    }

    ctx.minions.erase(
        std::remove_if(ctx.minions.begin(), ctx.minions.end(),
            [](const Minion& m) { return !m.alive; }),
        ctx.minions.end()
    );
}

static void updateMinionSpawns(GameContext& ctx, float dt)
{
    ctx.minionSpawnTimer -= dt;
    if (ctx.minionSpawnTimer > 0.0f) return;

    const int maxMinions = 20;
    if ((int)ctx.minions.size() >= maxMinions) return;

    const auto& mapData = ctx.level.map.data();
    int rows = ctx.level.map.getHeight();
    LevelMetrics metrics = LevelMetrics::fromMap(ctx.level.map, GameConfig::TILE);

    std::vector<std::pair<float,float>> spawnPoints;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < (int)mapData[i].size(); j++)
        {
            if (mapData[i][j] != 'B') continue;

            float wx, wz;
            metrics.tileCenter(j, i, wx, wz);

            // Verifica se jogador está em cima
            float pdx = ctx.camX - wx;
            float pdz = ctx.camZ - wz;
            if (std::sqrt(pdx*pdx + pdz*pdz) < GameConfig::TILE) continue;

            // Verifica se algum minion está em cima
            bool occupied = false;
            for (const auto& m : ctx.minions)
            {
                if (!m.alive) continue;
                float mdx = m.x - wx;
                float mdz = m.z - wz;
                if (std::sqrt(mdx*mdx + mdz*mdz) < GameConfig::TILE * 0.5f)
                {
                    occupied = true;
                    break;
                }
            }
            if (occupied) continue;

            spawnPoints.push_back({wx, wz});
        }
    }

    if (spawnPoints.empty()) return;

    auto& pt = spawnPoints[rand() % spawnPoints.size()];
    Minion m;
    m.x           = pt.first;
    m.z           = pt.second;
    m.speed       = 6.5f;
    m.alive       = true;
    m.damageTimer = 0.0f;
    ctx.minions.push_back(m);

    ctx.minionSpawnTimer = 1.5f + (rand() % 10) * 0.2f;
    ctx.audio.playMinionSound();
}

static void updateBossProjectiles(GameContext& ctx, float dt)
{
    for (auto& p : ctx.bossProjectiles)
    {
        if (!p.active) continue;
        p.x += p.velX * dt;
        p.y += p.velY * dt;
        p.z += p.velZ * dt;
        p.rotation += p.rotationSpeed * dt;

        float dx = ctx.camX - p.x;
        float dz = ctx.camZ - p.z;
        if (std::sqrt(dx*dx + dz*dz) < 1.0f)
        {
            p.active = false;
            ctx.player.takeDamage(30); // ← era ctx.playerHealth -= 30
            ctx.audio.audioPlayHurt();
        }

        if (p.z > ctx.camZ + 30.0f || p.z < ctx.camZ - 50.0f)
            p.active = false;
    }

    ctx.bossProjectiles.erase(
        std::remove_if(ctx.bossProjectiles.begin(), ctx.bossProjectiles.end(),
            [](const Projectile& p) { return !p.active; }),
        ctx.bossProjectiles.end()
    );
}

// -------------------------------------------------------
void Phase3::init(GameContext& ctx)
{
    fade     = 0.0f;      // fade inicial da fase
    fadeWait = 0.0f;
    ctx.audio.initBreath();
    srand((unsigned int)time(NULL));

    // Busca posição da arma no mapa
    const auto& data = ctx.level.map.data();
    const LevelMetrics& m = ctx.level.metrics;
    for (int z = 0; z < (int)data.size(); z++)
    {
        for (int x = 0; x < (int)data[z].size(); x++)
        {
            if (data[z][x] == 'G')
            {
                float wx, wz;
                m.tileCenter(x, z, wx, wz);
                ctx.gunX = wx;
                ctx.gunZ = wz;
            }
        }
    }

    reset(ctx);           // tudo que precisa ser zerado
}

void Phase3::reset(GameContext& ctx)
{
    // Jogador
    ctx.player.init();
    ctx.weaponPickedUp = false;

    // Boto
    ctx.boto.x           =  0.0f;
    ctx.boto.z           = -15.0f;
    ctx.boto.health      = 100.0f;
    ctx.boto.alive       = true;
    ctx.boto.moveTimer   = 0.0f;
    ctx.boto.attackTimer = 2.0f;
    ctx.boto.zigZagDir   = 1.0f;

    // Minions e projéteis
    ctx.minions.clear();
    ctx.bossProjectiles.clear();
    ctx.playerProjectiles.clear();
    ctx.drops.clear();

    // Estado da fase
    ctx.minionSpawnTimer = 0.0f;
    ctx.minionKillCount  = 0;
    ctx.p3state          = Phase3State::EXPLORE;
    ctx.bossIntro        = false;

    // Câmera
    ctx.camY  = 1.0f;
    ctx.yaw   = 0.0f;
    ctx.pitch = 0.0f;
    updateCamera(ctx);

    fade = 0.0f;
    fadeWait = 0.0f;
}

void Phase3::onExit(GameContext& ctx)
{
    ctx.audio.stopPhase3Loops();
    ctx.audio.stopAllSounds();
}

void Phase3::update(GameContext& ctx, float dt)
{
    ctx.audio.updateListener(ctx.camX, ctx.camY, ctx.camZ, ctx.yaw, ctx.pitch);
    ctx.audio.update(dt, ctx.camX, ctx.camZ, {}, ctx.player.getHealth(),
                     ctx.keyW || ctx.keyA || ctx.keyS || ctx.keyD, ctx.inTransition);

    switch (ctx.p3state)
    {
        case Phase3State::EXPLORE:
            updateMovement(ctx);
            // Arma no chão só no mapa de exploração (não dá pra atirar ainda)
            if (!ctx.weaponPickedUp && !ctx.player.hasWeapon())
            {
                float dx = ctx.camX - ctx.gunX;
                float dz = ctx.camZ - ctx.gunZ;
                if (std::sqrt(dx*dx + dz*dz) < 1.5f)
                {
                    ctx.weaponPickedUp = true; // marcou que pegou
                    ctx.p3state = Phase3State::FADE_OUT;
                    fade = 0.0f;
                }
            }
            break;

        case Phase3State::FADE_OUT:
        case Phase3State::FADE_IN:
            updateFade(ctx, dt);
            break;

        case Phase3State::BOSS_FIGHT:
            updateMovement(ctx);
            updateEnemies(ctx, dt);     // Boto + minions + projéteis
            updateDrops(ctx, dt);       // Coleta e remoção de drops
            handlePlayerReload(ctx); // Recarga da arma

            ctx.rain.update(dt, 20.0f);

            if (ctx.player.getHealth() <= 0)
            {
                reset(ctx);                  // reseta a fase
                ctx.gameOverReason = GameOverReason::PHASE3_BOSS;
                ctx.currentState = GameState::GAME_OVER; // vai para tela de Game Over
                return;
            }

            if (!ctx.boto.alive)
            {
                ctx.p3state = Phase3State::VICTORY;
                ctx.audio.playBotoDead();
                ctx.audio.stopTrilha3();      // ← para trilha
                ctx.audio.stopBotoLoop();
                ctx.audio.stopPhase3Loops();
                fade  = 0.0f;
                return;
            }

            // DISPARO INSTANTÂNEO
            if (ctx.player.shootRequested && ctx.player.hasWeapon() && ctx.player.getCurrentAmmo() > 0)
            {
                ctx.player.shootRequested = false;
                ctx.player.shoot(ctx);
            }
            break;

        case Phase3State::VICTORY:
            fade += dt * 0.5f;  // fade mais lento
            if (fade >= 1.0f)
            {
                fade = 1.0f;
                if (!ctx.audio.isBotoDeadPlaying())
                    ctx.requestVictory = true;
            }   
            break;
    }
}

void Phase3::updateFade(GameContext& ctx, float dt)
{
    ctx.inTransition = true;
    if (ctx.p3state == Phase3State::FADE_OUT)
    {
        fade += dt * 1.5f;
        if (fade >= 1.0f)
        {
            fade = 1.0f;
            fadeWait = 0.0f;
            ctx.p3state = Phase3State::FADE_IN;
        }
    }
    else if (ctx.p3state == Phase3State::FADE_IN)
    {
        fadeWait += dt;
        if (fadeWait >= 1.0f && !ctx.bossIntro)
        {
            ctx.bossIntro = true;
            loadLevel(ctx.level, "maps/map3_boss.txt");
            applySpawn(ctx.level, ctx.camX, ctx.camZ);
            ctx.player.setHasGun(true);
            ctx.player.setWeaponVisible(true);
        }
        if (fadeWait >= 1.0f)
        {
            fade -= dt * 1.5f;
            if (fade <= 0.0f)
            {
                fade = 0.0f;
                ctx.inTransition = false;
                ctx.p3state = Phase3State::BOSS_FIGHT;
                ctx.audio.stopTrilha3();               
                ctx.audio.startTrilha3();             
                ctx.audio.startBotoLoop(ctx.boto.x, ctx.boto.z);
            }
        }
    }
}

void Phase3::updateEnemies(GameContext& ctx, float dt)
{
    // Atualiza Boto, minions e projéteis
    updateBoto(ctx, dt);
    updateMinions(ctx, dt, minionSoundTimer);
    updateMinionSpawns(ctx, dt);
    updateBossProjectiles(ctx, dt);
}

void Phase3::updateDrops(GameContext& ctx, float dt)
{
    for (auto& drop : ctx.drops)
    {
        if (!drop.active) continue;
        float dx = ctx.camX - drop.x;
        float dz = ctx.camZ - drop.z;
        if (std::sqrt(dx*dx + dz*dz) < 1.0f)
        {
            drop.active = false;
            if (drop.type == DropType::AMMO)
                ctx.player.addAmmo(6);
            else if (drop.type == DropType::HEALTH)
            {
                ctx.player.addHealth(20);
                ctx.player.setHealthAlpha(1.5f);
            }
        }
    }
    ctx.drops.erase(
        std::remove_if(ctx.drops.begin(), ctx.drops.end(),
            [](const DropItem& d){ return !d.active; }),
        ctx.drops.end()
    );
}

void Phase3::handlePlayerReload(GameContext& ctx)
{
    bool wasReloading = ctx.player.getIsReloading();
    ctx.player.tryReload(ctx);
    bool nowReloading = ctx.player.getIsReloading();

    // Começou a recarregar agora
    if (!wasReloading && nowReloading)
        ctx.audio.playReload(); // ← aqui
}

void Phase3::render(GameContext& ctx)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Câmera e projeção
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
    renderEnemies(ctx);
    renderHUD(ctx);
    renderFade(ctx);
    renderHealthOverlay(ctx);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Phase3::renderWorld(GameContext& ctx)
{
    glDisable(GL_FOG);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    GLuint skyOriginal = ctx.texSkydome;
    ctx.texSkydome = ctx.texSkydome3;
    drawSkydome(ctx, ctx.camX, ctx.camY, ctx.camZ);
    ctx.texSkydome = skyOriginal;

    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_LIGHTING);
    glUseProgram(ctx.progLighting);
    setupLightingPhase3(ctx);
    glUniform1i(glGetUniformLocation(ctx.progLighting, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(ctx.progLighting, "uAmuletCount"), 0);
    renderLevel(ctx, ctx.level.map, true);
    glUseProgram(0);
    glDisable(GL_LIGHTING);
    drawLevelTrees(ctx, ctx.level.map);
    glEnable(GL_LIGHTING);

    // Drops
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    for (const auto& drop : ctx.drops)
    {
        if (!drop.active) continue;
        GLuint tex = (drop.type == DropType::AMMO) ? ctx.texAmmo : ctx.texHealth;
        drawSpriteAt(drop.x, 0.0f, drop.z, 0.4f, 0.8f, tex, ctx.camX, ctx.camZ);
    }
    glEnable(GL_LIGHTING);

    // Arma no chão
    if (!ctx.player.hasWeapon() && ctx.p3state == Phase3State::EXPLORE)
    {
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawSpriteAt(ctx.gunX, 0.0f, ctx.gunZ, 0.5f, 1.0f, ctx.texGunDrop, ctx.camX, ctx.camZ);
        glEnable(GL_LIGHTING);
    }
}

void Phase3::renderEnemies(GameContext& ctx)
{
    // Boto
    if (ctx.boto.alive && ctx.p3state == Phase3State::BOSS_FIGHT)
    {
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        drawSpriteAt(ctx.boto.x, 0.0f, ctx.boto.z, 1.0f, 2.0f, ctx.botoTexture, ctx.camX, ctx.camZ);
        glEnable(GL_LIGHTING);
    }

    // Minions
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    for (const auto& m : ctx.minions)
    {
        if (!m.alive) continue;
        drawSpriteAt(m.x, 0.0f, m.z, 0.6f, 1.2f, ctx.botinhoTexture, ctx.camX, ctx.camZ);
    }
    glEnable(GL_LIGHTING);

    // Projéteis
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (const auto& p : ctx.bossProjectiles)
    {
        if (!p.active) continue;
        drawSpriteAt(p.x, p.y, p.z, 0.6f, 0.6f, ctx.texDumbbell, ctx.camX, ctx.camZ);
    }
    glEnable(GL_LIGHTING);
}

void Phase3::renderHUD(GameContext& ctx)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (ctx.p3state == Phase3State::FADE_IN || ctx.p3state == Phase3State::BOSS_FIGHT)
    {
        drawBossHUD(ctx);
        drawPlayerHUD(ctx);
        drawCrosshair(ctx.janelaW, ctx.janelaH);
        drawWeaponHUD(ctx);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Phase3::renderFade(GameContext& ctx)
{
    if (fade <= 0.0f) return;

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
}

void Phase3::renderHealthOverlay(GameContext& ctx)
{
    float alpha = ctx.player.getHealthAlpha() / 1.5f; 
    if (alpha <= 0.0f) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ctx.texHealthOverlay);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(0, 0);
        glTexCoord2f(1,0); glVertex2f(ctx.janelaW, 0);
        glTexCoord2f(1,1); glVertex2f(ctx.janelaW, ctx.janelaH);
        glTexCoord2f(0,1); glVertex2f(0, ctx.janelaH);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_BLEND);
}
