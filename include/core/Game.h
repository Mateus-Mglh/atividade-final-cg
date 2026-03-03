#ifndef GAME_H
#define GAME_H
#include <memory>
#include "core/GameContext.h"
#include "phases/Phase.h"

class Game
{
public:
    Game(GameContext& context);
    static Game* instance;
    bool init();
    void update();
    void render();
    void changeState(GameState newState);
    static void reshapeCallback(int w, int h);

private:
    GameContext& ctx;
    
    // Update
    void updateMenu();
    void updateGameOver();
    void updateVictory();
    void updatePaused();
    void reshape(int w, int h);
    std::unique_ptr<Phase> currentPhase;
};
#endif