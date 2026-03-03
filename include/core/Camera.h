#pragma once

struct GameContext;

// Define posição e orientação da câmera
void setCamera(GameContext& ctx,
               float x, float y, float z,
               float newYaw, float newPitch);

// Atualiza vetor direção
void updateCamera(GameContext& ctx);