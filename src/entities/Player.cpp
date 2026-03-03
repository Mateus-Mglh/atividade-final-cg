#include "entities/Player.h"
#include "core/GameContext.h"
#include "utils/Raycast.h"
#include "entities/Minion.h"
#include "entities/DropItem.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

void Player::init()
{
    health        = 100;
    currentAmmo   = MAX_MAGAZINE;
    reserveAmmo   = MAX_RESERVE_AMMO;
    hasGun        = false;
    weaponVisible = false;
    healthAlpha   = 0.0f;
    weaponState   = W_IDLE;
    weaponTimer   = 0.0f;
    weaponFiring  = false;
    isReloading   = false;
    shootRequested  = false;
    reloadRequested = false;
}

void Player::update(float dt, GameContext& ctx)
{
    const float TIME_FRAME_1 = 0.05f;
    const float TIME_FRAME_2 = 0.12f;
    const float RELOAD_T2    = 0.85f;
    const float RELOAD_T3    = 0.25f;

    // Decrementa healthAlpha
    if (healthAlpha > 0.0f)
        healthAlpha = std::max(0.0f, healthAlpha - dt / 1.5f);

    if (weaponState == W_IDLE) return;

    weaponTimer -= dt;
    if (weaponTimer > 0.0f) return;

    if (weaponState == W_FIRE_1)
    {
        weaponState = W_FIRE_2;
        weaponTimer = TIME_FRAME_2;
    }
    else if (weaponState == W_FIRE_2)
    {
        weaponState = W_PUMP;
        weaponTimer = PUMP_TIME;
    }
    else if (weaponState == W_PUMP || weaponState == W_RETURN)
    {
        weaponState  = W_IDLE;
        weaponFiring = false;
        weaponTimer  = 0.0f;
    }
    else if (weaponState == W_RELOAD_1)
    {
        weaponState = W_RELOAD_2;
        weaponTimer = RELOAD_T2;
    }
    else if (weaponState == W_RELOAD_2)
    {
        weaponState = W_RELOAD_3;
        weaponTimer = RELOAD_T3;
    }
    else if (weaponState == W_RELOAD_3)
    {
        int needed    = MAX_MAGAZINE - currentAmmo;
        int fill      = std::min(needed, reserveAmmo);
        currentAmmo  += fill;
        reserveAmmo  -= fill;
        isReloading   = false;
        weaponState   = W_IDLE;
        weaponTimer   = 0.0f;
    }
    else
    {
        weaponState  = W_IDLE;
        weaponTimer  = 0.0f;
        weaponFiring = false;
        isReloading  = false;
    }
}

void Player::shoot(GameContext& ctx)
{
    if (!hasGun || currentAmmo <= 0 || weaponState != W_IDLE)
        return;

    currentAmmo--;
    weaponState  = W_FIRE_1;
    weaponFiring = true;
    weaponTimer  = 0.08f;
    ctx.audio.playShot();

    float rayDx = ctx.camDirX;
    float rayDz = ctx.camDirZ;
    float len   = std::sqrt(rayDx * rayDx + rayDz * rayDz);
    if (len > 0.0001f)
    {
        rayDx /= len;
        rayDz /= len;
    }

    float rayOx = ctx.camX;
    float rayOz = ctx.camZ;

    // Minion mais próximo na linha de tiro
    float closestDist  = 999.0f;
    int   closestIndex = -1;
    for (int i = 0; i < (int)ctx.minions.size(); i++)
    {
        auto& m = ctx.minions[i];
        if (!m.alive) continue;
        float hitDist;
        if (rayHitsCircle(rayOx, rayOz, rayDx, rayDz, m.x, m.z, 0.85f, 20.0f, hitDist))
        {
            if (hitDist < closestDist)
            {
                closestDist  = hitDist;
                closestIndex = i;
            }
        }
    }

    // Verifica boto
    float botoDist = 999.0f;
    bool  botoHit  = false;
    if (ctx.boto.alive)
    {
        float hitDist;
        if (rayHitsCircle(rayOx, rayOz, rayDx, rayDz, ctx.boto.x, ctx.boto.z, 1.0f, 20.0f, hitDist))
        {
            botoDist = hitDist;
            botoHit  = true;
        }
    }

    // Acerta o alvo mais próximo
    if (closestIndex != -1 && (!botoHit || closestDist < botoDist))
    {
        Minion& m = ctx.minions[closestIndex];
        if (!m.alive) return;
        float dropX = m.x;
        float dropZ = m.z;
        m.alive = false;
        ctx.minionKillCount++;
        ctx.audio.playBulletImpact();
        if ((rand() % 100) < 30)
        {
            DropItem drop;
            drop.x      = dropX;
            drop.z      = dropZ;
            drop.type   = (rand() % 5 == 0) ? DropType::HEALTH : DropType::AMMO;
            drop.active = true;
            ctx.drops.push_back(drop);
        }
    }
    else if (botoHit)
    {
        ctx.boto.health -= 5;
        ctx.audio.playBulletImpact();
        printf("Dano no Boto! Vida: %.1f\n", ctx.boto.health);
        if (ctx.boto.health <= 0)
        {
            ctx.boto.alive = false;
            ctx.audio.playBotoDead();
            printf("O Boto foi derrotado!\n");
        }
    }
}

void Player::tryReload(GameContext& ctx)
{
    if (isReloading || reloadRequested == false) return;

    reloadRequested = false;
    shootRequested  = false;

    if (currentAmmo < MAX_MAGAZINE && reserveAmmo > 0)
    {
        weaponState = W_RELOAD_1;
        weaponTimer = 0.5f;
        isReloading = true;
    }
}