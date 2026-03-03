#pragma once

struct GameContext;

// Atualiza centro após reshape
void atualizaCentroJanela(GameContext& ctx, int w, int h);

void toggleFullScreenState(GameContext& ctx);

void atualizaCentroJanela(GameContext& ctx, int w, int h);

void applyFullScreenMode(GameContext& ctx);

// Alterna fullscreen (ALT+ENTER)
void altFullScreen(GameContext& ctx);
