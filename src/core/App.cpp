#include "core/GameContext.h"
#include "core/Game.h"
#include "systems/InputSystem.h"
#include "core/Config.h"

#include <GL/glew.h>
#include <GL/glut.h>

static GameContext* gCtx = nullptr;
static Game* gGame = nullptr;

// --------------------
// Callbacks
// --------------------

static void displayCb()
{
    if (gGame)
        gGame->render();
}

static void idleCb()
{
    if (gGame)
        gGame->update();

    glutPostRedisplay();
}

// --------------------
// Inicialização app
// --------------------

void appInit(GameContext& ctx)
{
    gCtx = &ctx;

    ctx.audio.init();
    InputSystem::init(ctx);

    static Game game(ctx);
    gGame = &game;
    game.init();

    glutDisplayFunc(displayCb);
    glutIdleFunc(idleCb);
    glutReshapeFunc(Game::reshapeCallback);
    glutKeyboardFunc(InputSystem::keyboardCallback);
    glutKeyboardUpFunc(InputSystem::keyboardUpCallback);
    glutMouseFunc(InputSystem::mouseClickCallback);
    glutPassiveMotionFunc(InputSystem::mouseMotionCallback);
    glutSpecialFunc(InputSystem::specialKeyCallback);

    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}