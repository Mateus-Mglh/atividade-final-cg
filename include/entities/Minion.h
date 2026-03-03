#pragma once
struct Minion
{
    float x, z;
    float speed  = 4.0f;
    bool  alive  = true;
    float damageTimer = 0.0f; // cooldown de dano
};