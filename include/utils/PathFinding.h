#pragma once
#include <deque>
#include <utility>

struct GameContext;

// Retorna lista de posições (mundo) do inimigo até o jogador
std::deque<std::pair<float,float>> findPath(
    GameContext& ctx,
    float startX, float startZ,
    float goalX,  float goalZ
);