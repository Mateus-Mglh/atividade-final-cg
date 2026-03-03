#include "audio/AudioEnemies.h"
#include <cstdlib>

// ── Mapinguari ────────────────────────────────
void AudioEnemies::startMapinguariIntro(AudioEngine& audio, ALuint srcMapScream, ALuint srcPassoMonstro)
{
    if (!srcMapScream) return;
    alSourcei(srcMapScream, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(srcMapScream, AL_POSITION, 0, 0, 0);
    audio.stop(srcMapScream);
    audio.play(srcMapScream);
    mapIntroPlaying = true;
    mapIntroTimer   = 5.0f;
    if (srcPassoMonstro) {
        audio.stop(srcPassoMonstro);
        audio.play(srcPassoMonstro);
    }
}

void AudioEnemies::playMapinguariScream(AudioEngine& audio, ALuint srcMapScream)
{
    if (!srcMapScream) return;
    alSourcei(srcMapScream, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(srcMapScream, AL_POSITION, 0, 0, 0);
    audio.play(srcMapScream);
}

void AudioEnemies::stopMapinguariScream(AudioEngine& audio, ALuint srcMapScream)
{
    mapScreamActive = false;
    if (srcMapScream) audio.stop(srcMapScream);
}

void AudioEnemies::startMapinguariLoop(AudioEngine& audio, ALuint srcMapLoop, float x, float z)
{
    if (!srcMapLoop || mapLoopPlaying) return;
    audio.setSourcePos(srcMapLoop, Vec3{x, 0.0f, z});
    audio.play(srcMapLoop);
    mapLoopPlaying  = true;
    mapScreamActive = true;
    float r = (float)rand() / (float)RAND_MAX;
    mapScreamTimer = AudioTuning::MAP_SCREAM_MIN +
                     r * (AudioTuning::MAP_SCREAM_MAX - AudioTuning::MAP_SCREAM_MIN);
}

void AudioEnemies::stopMapinguariLoop(AudioEngine& audio, ALuint srcMapLoop, ALuint srcPassoMonstro)
{
    if (srcMapLoop)      audio.stop(srcMapLoop);
    if (srcPassoMonstro) audio.stop(srcPassoMonstro);
    mapLoopPlaying  = false;
    mapScreamActive = false;
}

void AudioEnemies::setMapinguariPos(AudioEngine& audio, ALuint srcMapLoop, ALuint srcPassoMonstro, float x, float z)
{
    if (srcMapLoop)      audio.setSourcePos(srcMapLoop,      Vec3{x, 0.0f, z});
    if (srcPassoMonstro) audio.setSourcePos(srcPassoMonstro, Vec3{x, 0.0f, z});
}

void AudioEnemies::tickMapinguariScream(AudioEngine& audio, ALuint srcMapLoop, float dt, float distToPlayer)
{
    if (!srcMapLoop || !mapScreamActive) return;
    if (distToPlayer <= AudioTuning::MAP_SPOT_DIST)
    {
        if (!mapSpottedPlayed)
        {
            ALint state = 0;
            alGetSourcei(srcMapLoop, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
            {
                audio.play(srcMapLoop);
                mapSpottedPlayed = true;
            }
        }
    }
    else
    {
        mapSpottedPlayed = false;
    }
    (void)dt;
}

void AudioEnemies::tickPauseTimer(float dt, AudioEngine& audio, ALuint srcPassoMonstro)
{
    if (mapLoopPauseTimer <= 0.0f) return;
    mapLoopPauseTimer -= dt;
    if (mapLoopPauseTimer <= 0.0f)
    {
        mapScreamActive = true;
        mapScreamTimer  = 1.0f;
        if (srcPassoMonstro) audio.play(srcPassoMonstro);
    }
}

void AudioEnemies::pauseMapinguariLoop(AudioEngine& audio, ALuint srcMapLoop, ALuint srcPassoMonstro, float duration)
{
    if (srcMapLoop)      audio.stop(srcMapLoop);
    if (srcPassoMonstro) audio.stop(srcPassoMonstro);
    mapScreamActive   = false;
    mapLoopPauseTimer = duration;
}

void AudioEnemies::playBossSealed(AudioEngine& audio, ALuint srcMapScream)
{
    if (srcMapScream) {
        alSourcei(srcMapScream, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcMapScream, AL_POSITION, 0, 0, 0);
        audio.play(srcMapScream);
    }
}

void AudioEnemies::playMonsterHit(AudioEngine& audio, ALuint srcMonsterHit)
{
    if (!srcMonsterHit) return;
    alSourcei(srcMonsterHit, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(srcMonsterHit, AL_POSITION, 0, 0, 0);
    audio.play(srcMonsterHit);
}

// ── Fase 2 ────────────────────────────────────
void AudioEnemies::playSnakeRattle(AudioEngine& audio, ALuint srcSnakeRattle)
{
    if (!srcSnakeRattle) return;
    audio.stop(srcSnakeRattle);
    audio.play(srcSnakeRattle);
}

void AudioEnemies::playBoatHit(AudioEngine& audio, ALuint srcBoatHit)
{
    if (!srcBoatHit) return;
    audio.stop(srcBoatHit);
    audio.play(srcBoatHit);
}

void AudioEnemies::playSnakeAttack(AudioEngine& audio, ALuint srcSnakeAttack)
{
    if (!srcSnakeAttack) return;
    audio.stop(srcSnakeAttack);
    audio.play(srcSnakeAttack);
}

// ── Fase 3 – Boto ─────────────────────────────
void AudioEnemies::startBotoLoop(AudioEngine& audio, ALuint srcBotoSound, float x, float z)
{
    if (!srcBotoSound || botoPlaying) return;
    audio.setSourcePos(srcBotoSound, {x, 0.0f, z});
    audio.play(srcBotoSound);
    botoPlaying = true;
}

void AudioEnemies::stopBotoLoop(AudioEngine& audio, ALuint srcBotoSound)
{
    if (srcBotoSound) audio.stop(srcBotoSound);
    botoPlaying = false;
}

void AudioEnemies::setBotoPos(AudioEngine& audio, ALuint srcBotoSound, float x, float z)
{
    if (srcBotoSound) audio.setSourcePos(srcBotoSound, {x, 0.0f, z});
}

void AudioEnemies::playBotoDead(AudioEngine& audio, ALuint srcBotoDead)
{
    if (!srcBotoDead) return;
    audio.stop(srcBotoDead);
    audio.play(srcBotoDead);
}

bool AudioEnemies::isBotoDeadPlaying(ALuint srcBotoDead) const
{
    if (!srcBotoDead) return false;
    ALint state = 0;
    alGetSourcei(srcBotoDead, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

// ── Fase 3 – Minions ──────────────────────────
void AudioEnemies::playMinionSound(AudioEngine& audio, ALuint srcMinionSound)
{
    if (!srcMinionSound) return;
    audio.stop(srcMinionSound);
    audio.play(srcMinionSound);
}

void AudioEnemies::stopMinionSound(AudioEngine& audio, ALuint srcMinionSound)
{
    if (srcMinionSound)
        audio.stop(srcMinionSound);
}