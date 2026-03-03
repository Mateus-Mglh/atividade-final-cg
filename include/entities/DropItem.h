#pragma once
enum class DropType { AMMO, HEALTH };
struct DropItem
{
    float x, z;
    DropType type;
    bool active = true;
};