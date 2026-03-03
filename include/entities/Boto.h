#pragma once
struct Boto
{
    float x, z;

    float health      = 100.0f;

    float moveTimer   = 0.0f;
    float attackTimer = 0.0f;
    float zigZagSpeed = 4.0f;
    float zigZagDir   = 1.0f;
    float targetX     = 0.0f;
    float targetZ     = 0.0f;
    float wanderTimer = 0.0f;

    bool  alive       = true;
    bool  attackReady = true;
};