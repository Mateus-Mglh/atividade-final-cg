// include/core/config.h
#pragma once

namespace GameConfig
{
    // Janela
    constexpr int WINDOW_W = 800;
    constexpr int WINDOW_H = 600;
    constexpr const char* WINDOW_TITLE = "SELVA";

    // Câmera
    constexpr float CAM_START_X = 0.0f;
    constexpr float CAM_START_Y = 1.0f;
    constexpr float CAM_START_Z = 5.0f;

    // Jogador
    constexpr float PLAYER_STEPS = 8.0f; // unidades por segundo
    constexpr float MOUSE_SENSITIVITY  = 0.1f;
    constexpr float PITCH_LIMIT        = 89.0f;
    constexpr float PLAYER_RADIUS = 0.1f; // fração do tile

    // Mundo
    constexpr float TILE       = 3.0f;  // tamanho do tile no mundo
    constexpr float WALL_H     = 3.0f;  // altura da parede
    constexpr float CEILING_H  = 3.0f;  // altura do teto
    constexpr float EPS_Y      = 0.0f;  // Y do chão

    // Inimigo
    constexpr float ENEMY_KILL_DIST = 1.6f;
}