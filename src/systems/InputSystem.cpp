#include "core/GameContext.h"
#include "systems/InputSystem.h"
#include "core/Movement.h"
#include "core/Camera.h"
#include "core/Config.h"
#include "core/Window.h"
#include "systems/ItemSystem.h"
#include <GL/glut.h>
#include <cstdlib>

static GameContext* gCtx = nullptr;

void InputSystem::init(GameContext& ctx)
{
    gCtx = &ctx;
}

void InputSystem::keyboard(GameContext& ctx, unsigned char key, int, int)
{
    switch (key)
    {
    case 'w': case 'W': ctx.keyW = true;  break;
    case 's': case 'S': ctx.keyS = true;  break;
    case 'a': case 'A': ctx.keyA = true;  break;
    case 'd': case 'D': ctx.keyD = true;  break;

    case 'm': case 'M':
        ctx.currentState = GameState::MENU;
        printf("Mudou pra Menu\n");
        break;

    case 'r': case 'R':
        ctx.player.reloadRequested = true;
        break;

    case 13: // ENTER
        ctx.keyEnter = true;
        break;
    
    case 27:
    switch (ctx.currentState)
    {
        case GameState::PHASE1:
        case GameState::PHASE2:
        case GameState::PHASE3:
            ctx.previousState = ctx.currentState;
            ctx.currentState = GameState::PAUSED;
            ctx.pauseSelectedItem = 0;
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            break;

        case GameState::PAUSED:
            ctx.currentState = ctx.previousState;
            glutSetCursor(GLUT_CURSOR_NONE);
            break;

        case GameState::MENU:
            break;

        default:
            std::exit(0);
            break;
    }
    break;
    }
}

void InputSystem::keyboardUp(GameContext& ctx, unsigned char key, int, int)
{
    switch (key)
    {
    case 'w': case 'W': ctx.keyW = false; break;
    case 's': case 'S': ctx.keyS = false; break;
    case 'a': case 'A': ctx.keyA = false; break;
    case 'd': case 'D': ctx.keyD = false; break;
    case 13: ctx.keyEnter = false;  break;
    }

    if ((key == 13 || key == '\r') && (glutGetModifiers() & GLUT_ACTIVE_ALT))
        altFullScreen(ctx);
}

void InputSystem::mouseClick(GameContext& ctx, int button, int state, int x, int y)
{
    if (ctx.currentState == GameState::GAME_OVER)
    return;

    if (ctx.currentState == GameState::PAUSED)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            ctx.mouseX   = x;
            ctx.mouseY   = y;
            ctx.keyEnter = true;
        }
        return;
    }

    if (ctx.currentState == GameState::MENU)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            ctx.mouseX = x;
            ctx.mouseY = y;
            ctx.keyEnter = true; // confirma o item que está sob o mouse
        }
        return;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // 2. Lógica específica da Fase 3
        if (ctx.currentState == GameState::PHASE3)
        {
            // Só permite atirar se estiver na luta e TIVER a arma
            if (ctx.p3state == Phase3State::BOSS_FIGHT && ctx.player.hasWeapon())
            {
                ctx.player.shootRequested = true;
            }
        }
        else 
        {
            tryThrowStone(ctx);
        }
    }
}

void InputSystem::handleMouseMotion(GameContext& ctx, int x, int y)
{
    if (ctx.ignoreWarp)
    {
        ctx.ignoreWarp = false;
        return;
    }

    if (ctx.firstMouse)
    {
        ctx.firstMouse = false;
        ctx.ignoreWarp = true;
        glutWarpPointer(ctx.centerX, ctx.centerY);
        return;
    }
    
    ctx.mouseX = x;
    ctx.mouseY = y;

    if (ctx.currentState == GameState::MENU) return;
    if (ctx.currentState == GameState::PAUSED) return;

    int dx = x - ctx.centerX;
    int dy = y - ctx.centerY;

    ctx.yaw   += dx * GameConfig::MOUSE_SENSITIVITY;
    ctx.pitch -= dy * GameConfig::MOUSE_SENSITIVITY;

    if (ctx.pitch >  GameConfig::PITCH_LIMIT) ctx.pitch =  GameConfig::PITCH_LIMIT;
    if (ctx.pitch < -GameConfig::PITCH_LIMIT) ctx.pitch = -GameConfig::PITCH_LIMIT;

    updateCamera(ctx);

    ctx.ignoreWarp = true;
    glutWarpPointer(ctx.centerX, ctx.centerY);
}

void InputSystem::specialKey(GameContext& ctx, int key, int, int)
{
    if (ctx.currentState == GameState::MENU)
    {
        if (key == GLUT_KEY_UP)
            ctx.menuSelectedItem = (ctx.menuSelectedItem - 1 + 4) % 4;
        if (key == GLUT_KEY_DOWN)
            ctx.menuSelectedItem = (ctx.menuSelectedItem + 1) % 4;
    }
    else if (ctx.currentState == GameState::PAUSED)
    {
        if (key == GLUT_KEY_UP)
            ctx.pauseSelectedItem = (ctx.pauseSelectedItem - 1 + 2) % 2;
        if (key == GLUT_KEY_DOWN)
            ctx.pauseSelectedItem = (ctx.pauseSelectedItem + 1) % 2;
    }
}

void InputSystem::specialKeyCallback(int key, int x, int y)
    { if (gCtx) specialKey(*gCtx, key, x, y); }

// Callbacks GLUT
void InputSystem::keyboardCallback(unsigned char key, int x, int y)
    { if (gCtx) keyboard(*gCtx, key, x, y); }

void InputSystem::keyboardUpCallback(unsigned char key, int x, int y)
    { if (gCtx) keyboardUp(*gCtx, key, x, y); }

void InputSystem::mouseClickCallback(int button, int state, int x, int y)
    { if (gCtx) mouseClick(*gCtx, button, state, x, y); }

void InputSystem::mouseMotionCallback(int x, int y)
    { if (gCtx) handleMouseMotion(*gCtx, x, y); }