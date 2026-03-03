#include "entities/Enemy.h"
#include "core/GameContext.h"
#include "core/Config.h"
#include "utils/Collision.h"
#include "utils/Pathfinding.h"
#include <cmath>
#include <GL/glut.h>

Enemy::Enemy(float startX, float startZ) : x(startX), y(0.0f), z(startZ),
      speed(9.0f),   // velocidade do inimigo
      radius(0.3f)   // raio para colisão
{
}

void Enemy::updatePath(GameContext& ctx)
{
    path = findPath(ctx, x, z, ctx.camX, ctx.camZ);
}

void Enemy::update(GameContext& ctx, float deltaTime)
{
    if (!ctx.enemyActivated) return;
    if (y < 0.0f) return;
    // Checa morte
    float dx     = ctx.camX - x;
    float dz     = ctx.camZ - z;
    float distSq = dx*dx + dz*dz;
    if (distSq < (GameConfig::ENEMY_KILL_DIST * GameConfig::ENEMY_KILL_DIST))
    {
        ctx.gameOverReason = GameOverReason::PHASE1_CAUGHT;
        ctx.currentState = GameState::GAME_OVER;
        return;
    }

    if (stunTimer > 0.0f)
    {
        stunTimer -= deltaTime;
        if (stunTimer < 0.0f)
            stunTimer = 0.0f;
        return;
    }

    // Recalcula path a cada 0.5s
    pathTimer -= deltaTime;
    if (pathTimer <= 0.0f)
    {
        updatePath(ctx);
        pathTimer = 0.5f;
    }

    moveAlongPath(ctx, deltaTime);
}

void Enemy::applyStun(float duration)
{
    stunTimer = duration;
}

bool Enemy::isStunned() const
{
    return stunTimer > 0.0f;
}

void Enemy::sinkDown(float deltaTime)
{
    y -= deltaTime * 1.0f;

    if (y < -3.5f) // altura total + margem
        y = -3.5f;
}

void Enemy::moveAlongPath(GameContext& ctx, float deltaTime)
{
    float tx, tz;

    if (path.empty())
    {
        // Path vazio — vai direto pro jogador
        tx = ctx.camX;
        tz = ctx.camZ;
    }
    else
    {
        tx = path.front().first;
        tz = path.front().second;
    }

    float dx = tx - x;
    float dz = tz - z;
    float len = std::sqrt(dx*dx + dz*dz);

    if (len < 0.2f)
    {
        if (!path.empty()) path.pop_front();
        return;
    }

    dx /= len;
    dz /= len;

    float newX = x + dx * speed * deltaTime;
    float newZ = z + dz * speed * deltaTime;

    if (isPositionWalkable(ctx, newX, z, radius)) x = newX;
    if (isPositionWalkable(ctx, x, newZ, radius)) z = newZ;
}