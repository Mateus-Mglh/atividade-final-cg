#include "audio/AudioAmbience.h"

// ── Fase 1 ────────────────────────────────────
void AudioAmbience::startPhase1(AudioEngine& audio, ALuint srcChuva, ALuint srcFloresta)
{
    if (amb1Playing) return;
    if (srcChuva)    audio.play(srcChuva);
    if (srcFloresta) audio.play(srcFloresta);
    amb1Playing = true;
}

void AudioAmbience::stopPhase1(AudioEngine& audio, ALuint srcChuva, ALuint srcFloresta)
{
    if (srcChuva)    audio.stop(srcChuva);
    if (srcFloresta) audio.stop(srcFloresta);
    amb1Playing = false;
}

void AudioAmbience::stopChuva(AudioEngine& audio, ALuint srcChuva)
{
    if (srcChuva) audio.stop(srcChuva);
}

// ── Fase 2 ────────────────────────────────────
void AudioAmbience::startPhase2(AudioEngine& audio, ALuint srcFloresta, ALuint srcRiver)
{
    if (amb2Playing) return;
    if (srcFloresta) audio.play(srcFloresta);
    if (srcRiver)    audio.play(srcRiver);
    amb2Playing = true;
}

void AudioAmbience::stopPhase2(AudioEngine& audio, ALuint srcFloresta, ALuint srcRiver)
{
    if (srcFloresta) audio.stop(srcFloresta);
    if (srcRiver)    audio.stop(srcRiver);
    amb2Playing = false;
}

void AudioAmbience::startRiver(AudioEngine& audio, ALuint srcRiver)
{
    if (srcRiver) audio.play(srcRiver);
}

void AudioAmbience::stopRiver(AudioEngine& audio, ALuint srcRiver)
{
    if (srcRiver) audio.stop(srcRiver);
}

// ── Fase 3 ────────────────────────────────────
void AudioAmbience::startPhase3(AudioEngine& audio, ALuint srcTrilha3, ALuint bufTrilha3)
{
    startTrilha3(audio, srcTrilha3, bufTrilha3);
}

void AudioAmbience::stopPhase3(AudioEngine& audio, ALuint srcTrilha3)
{
    stopTrilha3(audio, srcTrilha3);
}

void AudioAmbience::startTrilha3(AudioEngine& audio, ALuint srcTrilha3, ALuint bufTrilha3)
{
    if (!srcTrilha3 || amb3Playing) return;
    alSourcei(srcTrilha3, AL_BUFFER,  bufTrilha3);
    alSourcei(srcTrilha3, AL_LOOPING, AL_TRUE);
    alSourcef(srcTrilha3, AL_GAIN,    AudioTuning::TRILHA3_GAIN);
    alSourcePlay(srcTrilha3);
    amb3Playing = true;
}

void AudioAmbience::stopTrilha3(AudioEngine& audio, ALuint srcTrilha3)
{
    if (srcTrilha3) audio.stop(srcTrilha3);
    amb3Playing = false;
}