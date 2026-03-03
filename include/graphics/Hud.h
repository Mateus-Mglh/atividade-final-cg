#pragma once
#include "core/GameContext.h"

void drawCrosshair(int w, int h);
void drawWeaponHUD(GameContext& ctx);
void drawBossHUD(GameContext& ctx);
void drawPlayerHUD(GameContext& ctx);
void renderMenu(GameContext& ctx);
void renderGameOver(GameContext& ctx);
void renderVictory(GameContext& ctx);
void renderPause(GameContext& ctx);
void drawPhase1HUD(GameContext& ctx);
void drawPhase2HUD(GameContext& ctx);