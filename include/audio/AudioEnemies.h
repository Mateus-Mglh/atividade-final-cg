#pragma once
#include <AL/al.h>
#include "audio/AudioEngine.h"
#include "audio/AudioTuning.h"

class AudioEnemies {
public:
    // ── Mapinguari ────────────────────────────
    void startMapinguariIntro(AudioEngine& audio, ALuint srcMapScream, ALuint srcPassoMonstro);
    void playMapinguariScream(AudioEngine& audio, ALuint srcMapScream);
    void stopMapinguariScream(AudioEngine& audio, ALuint srcMapScream);
    void startMapinguariLoop(AudioEngine& audio, ALuint srcMapLoop, float x, float z);
    void stopMapinguariLoop(AudioEngine& audio, ALuint srcMapLoop, ALuint srcPassoMonstro);
    void setMapinguariPos(AudioEngine& audio, ALuint srcMapLoop, ALuint srcPassoMonstro, float x, float z);
    void tickMapinguariScream(AudioEngine& audio, ALuint srcMapLoop, float dt, float distToPlayer);
    void tickPauseTimer(float dt, AudioEngine& audio, ALuint srcPassoMonstro);
    void pauseMapinguariLoop(AudioEngine& audio, ALuint srcMapLoop, ALuint srcPassoMonstro, float duration);
    void playBossSealed(AudioEngine& audio, ALuint srcMapScream);
    void playMonsterHit(AudioEngine& audio, ALuint srcMonsterHit);

    // ── Fase 2 – Cobra/Barco ──────────────────
    void playSnakeRattle(AudioEngine& audio, ALuint srcSnakeRattle);
    void playBoatHit(AudioEngine& audio, ALuint srcBoatHit);
    void playSnakeAttack(AudioEngine& audio, ALuint srcSnakeAttack);

    // ── Fase 3 – Boto ─────────────────────────
    void startBotoLoop(AudioEngine& audio, ALuint srcBotoSound, float x, float z);
    void stopBotoLoop(AudioEngine& audio, ALuint srcBotoSound);
    void setBotoPos(AudioEngine& audio, ALuint srcBotoSound, float x, float z);
    void playBotoDead(AudioEngine& audio, ALuint srcBotoDead);
    bool isBotoDeadPlaying(ALuint srcBotoDead) const;

    // ── Fase 3 – Minions ──────────────────────
    void playMinionSound(AudioEngine& audio, ALuint srcMinionSound);
    void stopMinionSound(AudioEngine& audio, ALuint srcMinionSound);

    // ── Estado ────────────────────────────────
    bool isMapLoopPlaying()  const { return mapLoopPlaying; }
    bool isMapIntroPlaying() const { return mapIntroPlaying; }
    float getMapIntroTimer() const { return mapIntroTimer; }
    void  tickMapIntro(float dt)   { if (mapIntroPlaying) mapIntroTimer -= dt; if (mapIntroTimer <= 0.0f) mapIntroPlaying = false; }

private:
    bool  mapLoopPlaying    = false;
    bool  mapIntroPlaying   = false;
    bool  mapScreamActive   = false;
    bool  mapSpottedPlayed  = false;
    bool  botoPlaying       = false;
    float mapIntroTimer     = 0.0f;
    float mapScreamTimer    = 0.0f;
    float mapLoopPauseTimer = 0.0f;
};