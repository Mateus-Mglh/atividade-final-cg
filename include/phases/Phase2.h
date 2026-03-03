#pragma once
#include "phases/Phase.h"

class Phase2 : public Phase
{
public:
    void init(GameContext& ctx) override;
    void update(GameContext& ctx, float dt) override;
    void render(GameContext& ctx) override;
    void onExit(GameContext& ctx) override;
    void reset(GameContext& ctx) override;

private:
    float fade = 0.0f;
    bool  transitioning = false;
    
    // Update
    void updateCanoe(GameContext& ctx, float dt);
    void updateObstacles(GameContext& ctx, float dt);
    void updateSnake(GameContext& ctx, float dt);

    // Render
    void renderWater(GameContext& ctx);
    void renderWorld(GameContext& ctx);
    void renderHUD(GameContext& ctx);
    void renderFade(GameContext& ctx);
    void renderObstacles(GameContext& ctx);
    void renderSnake(GameContext& ctx);
    void renderCanoe(GameContext& ctx);
};