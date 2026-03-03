#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include <set>

#include "audio/AudioEngine.h"
#include "audio/AudioManager.h"
#include "entities/Amulet.h"
#include "entities/Boto.h"
#include "entities/DropItem.h"
#include "entities/Minion.h"
#include "entities/Obstacle.h"
#include "entities/Player.h"
#include "entities/Projectile.h"
#include "entities/Snake.h"
#include "entities/Stone.h"
#include "level/Level.h"
#include "systems/RainSystem.h"

// =====================================================================
// Enums
// =====================================================================

enum class GameState
{
    MENU,
    PHASE1,
    PHASE2,
    PHASE3,
    GAME_OVER,
    VICTORY,
    PAUSED
};

enum class Phase3State
{
    EXPLORE,
    FADE_OUT,
    FADE_IN,
    BOSS_FIGHT,
    VICTORY
};

enum class GameOverReason
{
    NONE,
    PHASE1_CAUGHT,
    PHASE2_FELL,
    PHASE2_SNAKE,
    PHASE3_BOSS
};

// =====================================================================
// GameContext
// =====================================================================

struct GameContext
{
    // ----- Janela -----
    int  janelaW, janelaH;           // tamanho atual
    int  windowedW, windowedH;       // tamanho salvo do modo janela
    int  centerX, centerY;
    bool fullScreen = false;

    // ----- Mouse -----
    bool ignoreWarp = false;
    bool firstMouse = true;

    // ----- Menu -----
    int  menuSelectedItem = 0;  // 0=Iniciar, 1=Como Jogar, 2=Créditos, 3=Sair
    int  mouseX = 0;
    int  mouseY = 0;
    bool menuShowCredits   = false;
    bool menuShowHowToPlay = false;

    // ----- Câmera -----
    float camX, camY, camZ;
    float camDirX, camDirY, camDirZ;
    float yaw   = 0.0f;
    float pitch = 0.0f;

    // ----- Estado -----
    GameState     currentState  = GameState::MENU;
    GameOverReason gameOverReason = GameOverReason::NONE;
    GameState previousState = GameState::MENU;
    bool          running       = true;
    bool          inTransition  = false;
    bool requestVictory = false;

    // ----- Input -----
    bool keyW = false, keyS = false;
    bool keyA = false, keyD = false;
    bool keyEnter = false;
    int pauseSelectedItem   = 0; 

    // ----- Tempo -----
    float deltaTime = 0.0f;
    float lastTime  = 0.0f;
    int   fps       = 0;
    float runTime = 0.0f;
    bool countingRunTime = false;


    std::set<std::pair<int,int>> visibleTiles;

    // ----- Texturas -----
    GLuint enemyTexture        = 0;
    GLuint enemyStunnedTexture = 0;
    GLuint stoneTexture        = 0;
    GLuint rockTexture         = 0;
    GLuint logTexture          = 0;
    GLuint treeTexture         = 0;
    GLuint floorTexture        = 0;
    GLuint waterTexture        = 0;
    GLuint amuletTexture       = 0;
    GLuint botoTexture         = 0;
    GLuint botinhoTexture      = 0;
    GLuint texSkydome          = 0;
    GLuint texSkydome3         = 0;
    GLuint texGunDrop          = 0;
    GLuint texGunDefault       = 0;
    GLuint texGunFire1         = 0;
    GLuint texGunFire2         = 0;
    GLuint texGunReload1       = 0;
    GLuint texGunReload2       = 0;
    GLuint texHealthOverlay    = 0;
    GLuint texHealth           = 0;
    GLuint texAmmo             = 0;
    GLuint progLighting        = 0;
    GLuint texWall             = 0;
    GLuint texCeiling          = 0;
    GLuint texSnake            = 0;
    GLuint texDumbbell         = 0;
    GLuint texHeart            = 0;
    GLuint texCanoa            = 0;

    // ----- Fase 1 -----
    std::vector<Stone>      stones;
    std::vector<Projectile> projectiles;
    std::vector<Amulet>     amulets;
    int   playerStoneCount = 0;
    int   amuletsCollected = 0;
    float stoneSpawnTimer  = 0.0f;
    float throwCooldown    = 0.0f;
    bool  playerMoved      = false;
    bool  enemyActivated   = false;

    // ----- Fase 2 -----
    float canoeX             = 0.0f;
    float canoeZ             = 0.0f;
    float canoeSpeed         = 5.0f;
    int   canoeHealth        = 3;
    float phase2Timer        = 0.0f;
    float obstacleSpawnTimer = 0.0f;
    float riverMinX          = -6.0f;
    float riverMaxX          =  6.0f;
    float waterUV            = 0.0f;
    bool  requestPhase2      = false;
    bool  requestPhase3      = false;
    std::vector<Obstacle> obstacles;
    Snake snake;

    // ----- Fase 3 -----
    Phase3State p3state      = Phase3State::EXPLORE;
    float phase3Timer        = 0.0f;
    float gunX               = 0.0f;
    float gunZ               = 0.0f;
    float minionSpawnTimer   = 0.0f;
    int   minionKillCount    = 0;
    bool  weaponPickedUp     = false;
    bool  bossIntro          = false;
    Boto  boto;
    std::vector<Minion>     minions;
    std::vector<Projectile> bossProjectiles;
    std::vector<Projectile> playerProjectiles;
    std::vector<DropItem>   drops;

    // ----- Entidades -----
    Level       level;
    Player      player;
    RainSystem  rain;
    GLUquadric* sphereQuadric = nullptr;
    AudioEngine  audioEngine;
    AudioManager audio{ audioEngine };
};

void initGameContext(GameContext& ctx);