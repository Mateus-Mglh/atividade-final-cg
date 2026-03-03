#pragma once
#include "audio/AudioEngine.h"
#include "audio/AudioAmbience.h"
#include "audio/AudioPlayer.h"
#include "audio/AudioEnemies.h"
#include "audio/AudioWeapons.h"
#include <vector>

// ──────────────────────────────────────────────
//  Structs de info de áudio
// ──────────────────────────────────────────────
struct EnemyAudioState {
    bool  shouldPlay    = false;
    bool  screamPending = false;
    float screamTimer   = 0.0f;
};

struct EnemyAudioInfo {
    float x, z;
    bool  dead;
};

// ──────────────────────────────────────────────
//  AudioManager – orquestrador
// ──────────────────────────────────────────────
class AudioManager {
public:
    explicit AudioManager(AudioEngine& engine);

    // ── Ciclo de vida ─────────────────────────
    void init();
    void shutdown();
    void stopAllSounds();
    void stopPhase3Loops();

    // ── Listener ──────────────────────────────
    void updateListener(float x, float y, float z,
                        float yawDeg, float pitchDeg);

    // ── Update geral (chamar todo frame) ──────
    void update(float dt,
                float listenerX, float listenerZ,
                const std::vector<EnemyAudioInfo>& mapinguaris,
                int playerHealth,
                bool playerMoving,
                bool inTransition);

    // ── Ambiente ──────────────────────────────
    void startAmbientPhase1();
    void stopAmbientPhase1();
    void stopChuva();
    void startAmbientPhase2();
    void stopAmbientPhase2();
    void startRiver();
    void stopRiver();
    void startAmbientPhase3();
    void stopAmbientPhase3();
    void startTrilha3();
    void stopTrilha3();

    // ── Player ────────────────────────────────
    void audioUpdateStep(bool moving);
    void processStepAudio();
    void audioPlayHurt();
    void processBreathAudio(int playerHealth);
    void initBreath();
    void playAmuletPickup();
    void startAmuletHum();
    void stopAmuletHum(size_t idx);
    void setAmuletPos(size_t idx, float x, float z);
    void ensureAmuletSources(size_t count);
    void playHeartbeat();
    void startHeartbeat();
    void stopHeartbeat();

    // ── Inimigos ──────────────────────────────
    void startMapinguariIntro();
    void playMapinguariScream();
    void stopMapinguariScream();
    void startMapinguariLoop(float x, float z);
    void stopMapinguariLoop();
    void setMapinguariPos(float x, float z);
    void tickMapinguariScream(float dt, float distToPlayer);
    void pauseMapinguariLoop(float duration);
    void playBossSealed();
    void playMonsterHit();
    void playSnakeRattle();
    void playBoatHit();
    void playSnakeAttack();
    void startBotoLoop(float x, float z);
    void stopBotoLoop();
    void setBotoPos(float x, float z);
    void playBotoDead();
    void playMinionSound();
    bool isBotoDeadPlaying() const;

    // ── Armas ─────────────────────────────────
    void playShot();
    void playReload();
    void playReloadClick();
    void playBulletImpact();

private:
    AudioEngine& mAudio;
    bool mOk = false;

    // ── Módulos ───────────────────────────────
    AudioAmbience ambience;
    AudioPlayer   player;
    AudioEnemies  enemies;
    AudioWeapons  weapons;

    // ── Buffers ───────────────────────────────
    // Fase 1
    ALuint bufChuva        = 0;
    ALuint bufFloresta     = 0;
    ALuint bufPasso        = 0;
    ALuint bufHurt         = 0;
    ALuint bufAmuletPickup = 0;
    ALuint bufAmuletHum    = 0;
    ALuint bufHeartbeat    = 0;
    ALuint bufMapScream    = 0;
    ALuint bufMapLoop      = 0;
    ALuint bufMonsterHit   = 0;
    ALuint bufPassoMonstro = 0;
    ALuint bufBossSealed   = 0;
    // Fase 2
    ALuint bufAmb2         = 0;
    ALuint bufSnakeRattle  = 0;
    ALuint bufBoatHit      = 0;
    ALuint bufSnakeAttack  = 0;
    ALuint bufRiver        = 0;
    // Fase 3
    ALuint bufShot         = 0;
    ALuint bufReload       = 0;
    ALuint bufReloadClick  = 0;
    ALuint bufMinionSound  = 0;
    ALuint bufBotoDead     = 0;
    ALuint bufBotoSound    = 0;
    ALuint bufBulletImpact = 0;
    ALuint bufTrilha3      = 0;
    ALuint bufBreath       = 0;

    // ── Sources ───────────────────────────────
    // Fase 1
    ALuint srcChuva        = 0;
    ALuint srcFloresta     = 0;
    ALuint srcPasso        = 0;
    ALuint srcHurt         = 0;
    ALuint srcAmuletPickup = 0;
    ALuint srcHeartbeat    = 0;
    ALuint srcMapScream    = 0;
    ALuint srcMapLoop      = 0;
    ALuint srcMonsterHit   = 0;
    ALuint srcPassoMonstro = 0;
    // Fase 2
    ALuint srcAmb2         = 0;
    ALuint srcSnakeRattle  = 0;
    ALuint srcBoatHit      = 0;
    ALuint srcSnakeAttack  = 0;
    ALuint srcRiver        = 0;
    // Fase 3
    ALuint srcShot         = 0;
    ALuint srcReload       = 0;
    ALuint srcReloadClick  = 0;
    ALuint srcMinionSound  = 0;
    ALuint srcBotoDead     = 0;
    ALuint srcBotoSound    = 0;
    ALuint srcBulletImpact = 0;
    ALuint srcTrilha3      = 0;
    ALuint srcBreath       = 0;

    // ── Sources dinâmicos ─────────────────────
    std::vector<ALuint> srcAmulets;
    std::vector<ALuint> srcMinions;

    // ── Helpers ───────────────────────────────
    void loadBuffers();
    void createSources();
    ALuint tryLoad(const char* mono, const char* stereo = nullptr);
    void play2D(ALuint src);
    void playAt(ALuint src, float x, float z);
};