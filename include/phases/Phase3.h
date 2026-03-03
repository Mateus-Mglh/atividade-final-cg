#pragma once
#include "phases/Phase.h"

class Phase3 : public Phase
{
public:
    void init(GameContext& ctx) override;
    void update(GameContext& ctx, float dt) override;
    void render(GameContext& ctx) override;
    void onExit(GameContext& ctx) override;
    void reset(GameContext& ctx) override;

private:
    float fade     = 0.0f;
    float fadeWait = 0.0f;
    float minionSoundTimer = 0.0f;

    // Funções auxiliares privadas para organização
    void updateEnemies(GameContext& ctx, float dt);  // Boto + minions + projéteis
    void updateDrops(GameContext& ctx, float dt);    // Coleta e remoção de drops
    void updateFade(GameContext& ctx, float dt);     // Fade in/out da fase
    void handlePlayerReload(GameContext& ctx); // Lógica de recarga

    // Render
    void renderWorld(GameContext& ctx);
    void renderEnemies(GameContext& ctx);
    void renderHUD(GameContext& ctx);
    void renderFade(GameContext& ctx);
    void renderHealthOverlay(GameContext& ctx);
};