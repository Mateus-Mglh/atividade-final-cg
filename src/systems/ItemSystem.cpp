#include "systems/ItemSystem.h"
#include "core/GameContext.h"
#include "core/Config.h"
#include "entities/Stone.h"
#include "level/LevelMetrics.h"
#include "utils/Collision.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <GL/glut.h>

static bool isValidStonePosition(GameContext& ctx, float x, float z)
{
    // Verifica se não está dentro de parede
    const auto& data = ctx.level.map.data();
    const LevelMetrics& m = ctx.level.metrics;
    
    int tx = (int)std::floor((x - m.offsetX) / m.tile);
    int tz = (int)std::floor((z - m.offsetZ) / m.tile);
    
    if (tz < 0 || tz >= (int)data.size()) return false;
    if (tx < 0 || tx >= (int)data[tz].size()) return false;
    
    char c = data[tz][tx];
    return (c == '0' || c == 'E'); // só spawn em chão
}

void trySpawnStone(GameContext& ctx)
{
    // Limite de 6 pedras simultâneas
    int activeCount = 0;
    for (const auto& s : ctx.stones)
        if (s.active) activeCount++;
    
    if (activeCount >= 6) return;
    
    // Tenta spawnar em posição aleatória do mapa
    const LevelMetrics& m = ctx.level.metrics;
    int W = ctx.level.map.getWidth();
    int H = ctx.level.map.getHeight();
    
    for (int attempts = 0; attempts < 20; attempts++)
    {
        int tx = rand() % W;
        int tz = rand() % H;
        
        float wx, wz;
        m.tileCenter(tx, tz, wx, wz);
        
        if (isValidStonePosition(ctx, wx, wz))
        {
            ctx.stones.push_back({wx, wz, true});
            return;
        }
    }
}

void tryCollectStones(GameContext& ctx)
{
    if (ctx.playerStoneCount >= 10) return; // limite de 10
    
    for (auto& stone : ctx.stones)
    {
        if (!stone.active) continue;
        
        float dx   = ctx.camX - stone.x;
        float dz   = ctx.camZ - stone.z;
        float dist = std::sqrt(dx*dx + dz*dz);
        
        if (dist < 0.8f)
        {
            stone.active = false;
            ctx.playerStoneCount++;
            if (ctx.playerStoneCount >= 10) break;
        }
    }
}

void updateStones(GameContext& ctx)
{
    tryCollectStones(ctx);
}

void renderStones(GameContext& ctx)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ctx.stoneTexture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);

    for (const auto& stone : ctx.stones)
    {
        if (!stone.active) continue;

        glPushMatrix();
        glTranslatef(stone.x, 0.15f, stone.z);
        gluSphere(ctx.sphereQuadric, 0.15f, 24, 24);
        glPopMatrix();
    }

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_TEXTURE_2D);
}

void tryThrowStone(GameContext& ctx)
{
    if (ctx.playerStoneCount <= 0) return;
    if (ctx.throwCooldown > 0.0f) return;

    float speed = 12.0f;

    Projectile p;
    float offset = 0.4f;

    p.x = ctx.camX + ctx.camDirX * offset;
    p.y = ctx.camY - 0.2f + ctx.camDirY * offset;
    p.z = ctx.camZ + ctx.camDirZ * offset;


    p.velX = ctx.camDirX * speed;
    p.velY = ctx.camDirY * speed;
    p.velZ = ctx.camDirZ * speed;

    p.rotation = 0.0f;
    p.rotationSpeed = 360.0f;

    p.active = true;

    ctx.projectiles.push_back(p);

    ctx.playerStoneCount--;
    ctx.throwCooldown = 1.0f;
}

void updateProjectiles(GameContext& ctx)
{
    const float gravity = 15.0f;

    // Quantos subpassos por frame
    const int subSteps = 4;
    float stepDT = ctx.deltaTime / subSteps;

    for (auto& p : ctx.projectiles)
    {
        if (!p.active) continue;

        for (int i = 0; i < subSteps; i++)
        {
            // Salva posição anterior
            p.prevX = p.x;
            p.prevY = p.y;
            p.prevZ = p.z;

            // Gravidade
            p.velY -= gravity * stepDT;

            // Move
            p.x += p.velX * stepDT;
            p.y += p.velY * stepDT;
            p.z += p.velZ * stepDT;

            // Colisão com chão
            if (p.y <= 0.15f)
            {
                // Corrige posição usando a anterior
                p.y = 0.15f;

                // Só rebate se estava descendo
                if (p.velY < 0.0f)
                    p.velY *= -0.6f;

                // Pequeno atrito horizontal
                p.velX *= 0.9f;
                p.velZ *= 0.9f;

                // Se estiver muito fraca, para
                if (std::abs(p.velY) < 1.0f)
                {
                    p.velY = 0.0f;
                    p.active = false;
                }

                break;
            }

            if (!isPositionWalkable(ctx, p.x, p.z, 0.15f))
            {
                // Volta para posição anterior
                p.x = p.prevX;
                p.z = p.prevZ;

                // Rebote simples horizontal
                p.velX *= -0.6f;
                p.velZ *= -0.6f;

                break;
            }

            // Colisão com inimigos
            for (auto& enemy : ctx.level.enemies)
            {
                float minX = enemy.getX() - enemy.getHalfWidth();
                float maxX = enemy.getX() + enemy.getHalfWidth();

                float minY = enemy.getY();
                float maxY = enemy.getY() + enemy.getHeight();

                float minZ = enemy.getZ() - enemy.getHalfDepth();
                float maxZ = enemy.getZ() + enemy.getHalfDepth();

                float closestX = std::max(minX, std::min(p.x, maxX));
                float closestY = std::max(minY, std::min(p.y, maxY));
                float closestZ = std::max(minZ, std::min(p.z, maxZ));

                float dx = p.x - closestX;
                float dy = p.y - closestY;
                float dz = p.z - closestZ;

                float distSq = dx*dx + dy*dy + dz*dz;

                float projectileRadius = 0.15f;

                if (distSq < projectileRadius * projectileRadius)
                {
                    enemy.applyStun(2.0f);
                    p.active = false;
                    ctx.audio.playMonsterHit();
                    ctx.audio.pauseMapinguariLoop(2.0f);
                    break;
                }
            }

            if (!p.active) break;
        }

        // Rotação fora do substep
        p.rotation += p.rotationSpeed * ctx.deltaTime;
    }

    // Remove inativos
    ctx.projectiles.erase(
        std::remove_if(
            ctx.projectiles.begin(),
            ctx.projectiles.end(),
            [](const Projectile& p) { return !p.active; }
        ),
        ctx.projectiles.end()
    );
}

void renderProjectiles(GameContext& ctx)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ctx.stoneTexture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);
    for (const auto& p : ctx.projectiles)
    {
        if (!p.active) continue;
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glRotatef(p.rotation, 1, 0, 0);
        gluSphere(ctx.sphereQuadric, 0.15f, 24, 24); // <- aqui
        glPopMatrix();
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_TEXTURE_2D);   
}