#include <GL/glew.h>
#include "core/Game.h"
#include "core/GameContext.h"
#include "core/Camera.h"
#include "core/Config.h"
#include "core/Fps.h"
#include "core/Movement.h"
#include "core/Window.h"
#include "graphics/Drawlevel.h"
#include "graphics/Hud.h"
#include "graphics/SkyBox.h"
#include "level/Level.h"
#include "phases/Phase1.h"
#include "phases/Phase2.h"
#include "phases/Phase3.h"
#include <GL/glut.h>
#include <cmath>

Game* Game::instance = nullptr;

Game::Game(GameContext& context) : ctx(context)
{
    instance = this;
}

bool Game::init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    ctx.rain.init(500, 45.0f, 20.0f);

    // Iluminação
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat ambientLight[] = { 0.02f, 0.02f, 0.05f, 1.0f }; // quase zero
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    GLfloat lightPos[]     = { 0.5f, 1.0f, 0.3f, 0.0f };
    GLfloat lightDiffuse[] = { 0.05f, 0.06f, 0.12f, 1.0f }; // bem fraco
    GLfloat lightAmbient[] = { 0.0f,  0.0f,  0.02f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);

    // Fog (limita visibilidade)
    glEnable(GL_FOG);
    GLfloat fogColor[] = { 0.01f, 0.01f, 0.03f, 1.0f };
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 5.0f);
    glFogf(GL_FOG_END,   20.0f);

    return true;
}

void Game::update()
{
    float currentTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    ctx.deltaTime = currentTime - ctx.lastTime;
    ctx.lastTime  = currentTime;
    ctx.deltaTime = std::min(ctx.deltaTime, 0.05f);

    if (ctx.countingRunTime)
        ctx.runTime += ctx.deltaTime;

    fpsUpdate(ctx);
    
    ctx.player.update(ctx.deltaTime, ctx);

    switch (ctx.currentState)
    {
        case GameState::PHASE1:
            if (currentPhase)
                currentPhase->update(ctx, ctx.deltaTime);
            if (ctx.requestPhase2)
            {
                ctx.requestPhase2 = false;
                changeState(GameState::PHASE2);
            }
            break;

        case GameState::PHASE2:
            if (currentPhase)
                currentPhase->update(ctx, ctx.deltaTime);
            if (ctx.requestPhase3)
            {
                ctx.requestPhase3 = false;
                changeState(GameState::PHASE3);
            }
            break;

        case GameState::PHASE3:
            if(currentPhase)
                currentPhase->update(ctx, ctx.deltaTime);

            if (ctx.requestVictory)
            {
                ctx.requestVictory = false;
                changeState(GameState::VICTORY);
            }
            break;

        case GameState::MENU:      updateMenu();       break;
        case GameState::GAME_OVER: updateGameOver();   break;
        case GameState::VICTORY:   updateVictory();    break;
        case GameState::PAUSED:    updatePaused();     break;
        default: break;
    }
}

void Game::changeState(GameState newState)
{
    ctx.keyW = ctx.keyA = ctx.keyS = ctx.keyD = false;
    ctx.lastTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    if (currentPhase)
        currentPhase->onExit(ctx);

    if (newState == GameState::MENU)
    {
        ctx.audio.stopAllSounds();
        ctx.countingRunTime = false;
        ctx.runTime = 0.0f;
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    }
    else
    {
        glutSetCursor(GLUT_CURSOR_NONE);
    }

    if (newState == GameState::VICTORY)
    {
        ctx.countingRunTime = false;
    }

    ctx.currentState = newState;
    switch (newState)
    {
        case GameState::PHASE1:
            ctx.runTime = 0.0f;          
            ctx.countingRunTime = true;  

            currentPhase = std::make_unique<Phase1>();
            loadLevel(ctx.level, "maps/map1.txt");
            applySpawn(ctx.level, ctx.camX, ctx.camZ);
            currentPhase->init(ctx);
            break;

        case GameState::PHASE2:
            currentPhase = std::make_unique<Phase2>();
            currentPhase->init(ctx);
            break;

        case GameState::PHASE3:
            currentPhase = std::make_unique<Phase3>();
            loadLevel(ctx.level, "maps/map3_explore.txt");
            applySpawn(ctx.level, ctx.camX, ctx.camZ);
            currentPhase->init(ctx);
            break;

        default:
            break;
    }
}

void Game::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (ctx.currentState)
    {
        case GameState::PHASE1:
        case GameState::PHASE2:
        case GameState::PHASE3:
            if (currentPhase)
                currentPhase->render(ctx);
            break;
        case GameState::MENU:      renderMenu(ctx);      break;
        case GameState::GAME_OVER: renderGameOver(ctx);  break;
        case GameState::VICTORY:   renderVictory(ctx);   break;
        case GameState::PAUSED:
            if (currentPhase) currentPhase->render(ctx);
            renderPause(ctx);
            break;
    }

    fpsRender(ctx, "Lendas da Amazonia");
    glutSwapBuffers();
}

void Game::reshape(int w, int h)
{
    ctx.janelaW = w;
    ctx.janelaH = h;
    atualizaCentroJanela(ctx, w, h);
    glViewport(0, 0, w, h);
}

void Game::reshapeCallback(int w, int h)
{
    if (instance)
        instance->reshape(w, h);
}

void Game::updateMenu() {
    if (ctx.keyEnter)
    {
        ctx.keyEnter = false;
        if (ctx.menuShowCredits || ctx.menuShowHowToPlay)
        {
            ctx.menuShowCredits   = false;
            ctx.menuShowHowToPlay = false;
            return;
        }
        switch (ctx.menuSelectedItem)
        {
            case 0: changeState(GameState::PHASE1); break;
            case 1: ctx.menuShowHowToPlay = true;   break;
            case 2: ctx.menuShowCredits   = true;   break;
            case 3: std::exit(0);                   break;
        }
    }
}

void Game::updateGameOver() {
    if (ctx.keyEnter)
    {
        ctx.keyEnter = false;
        changeState(GameState::PHASE1);
    }
}

void Game::updateVictory() 
{
    if (ctx.keyEnter)
    {
        ctx.keyEnter = false;
        changeState(GameState::MENU);
    }
}

void Game::updatePaused()
{
    if (ctx.keyEnter)
    {
        ctx.keyEnter = false;
        switch (ctx.pauseSelectedItem)
        {
            case 0: // Continuar
                ctx.currentState = ctx.previousState;
                glutSetCursor(GLUT_CURSOR_NONE);
                break;
            case 1: // Menu
                changeState(GameState::MENU);
                break;
        }
    }
}