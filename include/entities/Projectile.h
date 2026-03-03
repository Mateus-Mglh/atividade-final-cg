#pragma once

struct Projectile
{
    float x, y, z;
    float prevX, prevY, prevZ;
    float velX, velY, velZ;
    float rotation;
    float rotationSpeed;
    bool active;
};