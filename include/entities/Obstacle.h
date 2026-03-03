#pragma once
enum class ObstacleType { LOG, ROCK };
struct Obstacle
{
    float x, z;
    ObstacleType type;
    bool active = true;
};