#pragma once
#include "phases/Phase.h"

enum class Phase1State
{
    EXPLORE,
    BOSS_SEALING,
    TRANSITION
};

class Phase1 : public Phase
{
public:
    void init(GameContext& ctx) override;
    void update(GameContext& ctx, float dt) override;
    void render(GameContext& ctx) override;
    void onExit(GameContext& ctx) override;
    void reset(GameContext& ctx) override;

private:
    Phase1State state = Phase1State::EXPLORE;
    float fadeAlpha            = 0.0f;
    float sealTimer            = 0.0f;
    float transitionTimer      = 0.0f;
    bool  sealingTriggered     = false;
    bool  sealTextVisible      = false;

    // ── Flags de áudio ────────────────────────
    bool mapinguariScreamPlayed = false; // grito ao ativar o inimigo
    bool sealSoundPlayed        = false; // som do selamento (evita repetir)

    // Update
    void updateExplore(GameContext& ctx, float dt);
    void updateBossSealing(GameContext& ctx, float dt);
    void updateTransition(GameContext& ctx, float dt);

    // Render
    void renderWorld(GameContext& ctx);
    void renderAmulets(GameContext& ctx);
    void setupAmuletLighting(GameContext& ctx);
    void renderHUD(GameContext& ctx);
    void renderFade(GameContext& ctx);
};