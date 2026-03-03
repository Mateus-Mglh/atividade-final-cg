#pragma once
#include <vector>

struct GameContext;

struct RainDrop {
    float x, y, z;
};

class RainSystem
{
public:
    RainSystem() = default;

    void init(int numDrops, float mapSize, float maxHeight);
    void update(float dt, float maxHeight);
    void render(GameContext& ctx);

private:
    std::vector<RainDrop> rainDrops;
    float speed = 15.0f;
    float length = 1.0f;
};