#include "core/GameContext.h"
#include "core/movement.h"
#include "core/camera.h"
#include "core/config.h"
#include "utils/Collision.h"
#include <cmath>

void updateMovement(GameContext& ctx)
{
    float step = GameConfig::PLAYER_STEPS * ctx.deltaTime;

    float radYaw = ctx.yaw * 3.14159265f / 180.0f;
    float dirX = std::sin(radYaw);
    float dirZ = -std::cos(radYaw);

    float strafeX =  dirZ;
    float strafeZ = -dirX;

    float dx = 0.0f;
    float dz = 0.0f;

    if (ctx.keyW) { dx += dirX * step; dz += dirZ * step; }
    if (ctx.keyS) { dx -= dirX * step; dz -= dirZ * step; }
    if (ctx.keyA) { dx += strafeX * step; dz += strafeZ * step; }
    if (ctx.keyD) { dx -= strafeX * step; dz -= strafeZ * step; }

    if (dx == 0.0f && dz == 0.0f) return;

    float len = std::sqrt(dx*dx + dz*dz);
    if (len > 0.0f) {
        dx = (dx/len) * step;
        dz = (dz/len) * step;
    }

    if (dx != 0.0f || dz != 0.0f)
    {
        ctx.playerMoved = true; // ← marca que o jogador se moveu
        if (!ctx.enemyActivated)
            ctx.enemyActivated = true;
    }

    float radius = GameConfig::PLAYER_RADIUS * GameConfig::TILE;

    // Colisão com slide por eixo
    float nx = ctx.camX + dx;
    if (isPositionWalkable(ctx, nx, ctx.camZ, radius))
        ctx.camX = nx;

    float nz = ctx.camZ + dz;
    if (isPositionWalkable(ctx, ctx.camX, nz, radius))
        ctx.camZ = nz;

    updateCamera(ctx);
}