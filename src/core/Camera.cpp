#include "core/GameContext.h"
#include "core/Camera.h"
#include <cmath>

void setCamera(GameContext& ctx,
               float x, float y, float z,
               float newYaw, float newPitch)
{
    ctx.camX = x;
    ctx.camY = y;
    ctx.camZ = z;
    ctx.yaw = newYaw;
    ctx.pitch = newPitch;
}

void updateCamera(GameContext& ctx)
{
    const float DEG_TO_RAD = 3.14159265f / 180.0f;

    float radYaw = ctx.yaw * DEG_TO_RAD;
    float radPitch = ctx.pitch * DEG_TO_RAD;

    ctx.camDirX = cosf(radPitch) * sinf(radYaw);
    ctx.camDirY = sinf(radPitch);
    ctx.camDirZ = -cosf(radPitch) * cosf(radYaw);
}
