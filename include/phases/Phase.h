#pragma once

struct GameContext;

class Phase
{
public:
    virtual ~Phase() = default;

    // Chamado quando a fase começa
    virtual void init(GameContext& ctx) = 0;

    // Atualização lógica
    virtual void update(GameContext& ctx, float deltaTime) = 0;

    // Renderização
    virtual void render(GameContext& ctx) = 0;

    // Chamado antes de trocar de fase
    virtual void onExit(GameContext& ctx) {}

    // Reiniciar valores da fase
    virtual void reset(GameContext& ctx) {}
};