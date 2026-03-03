#pragma once
#include <AL/al.h>
#include "audio/AudioEngine.h"
#include "audio/AudioTuning.h"

class AudioAmbience {
public:
    // ── Fase 1 ────────────────────────────────
    void startPhase1(AudioEngine& audio, ALuint srcChuva, ALuint srcFloresta);
    void stopPhase1(AudioEngine& audio, ALuint srcChuva, ALuint srcFloresta);
    void stopChuva(AudioEngine& audio, ALuint srcChuva);

    // ── Fase 2 ────────────────────────────────
    void startPhase2(AudioEngine& audio, ALuint srcFloresta, ALuint srcRiver);
    void stopPhase2(AudioEngine& audio, ALuint srcFloresta, ALuint srcRiver);
    void startRiver(AudioEngine& audio, ALuint srcRiver);
    void stopRiver(AudioEngine& audio, ALuint srcRiver);

    // ── Fase 3 ────────────────────────────────
    void startPhase3(AudioEngine& audio, ALuint srcTrilha3, ALuint bufTrilha3);
    void stopPhase3(AudioEngine& audio, ALuint srcTrilha3);
    void startTrilha3(AudioEngine& audio, ALuint srcTrilha3, ALuint bufTrilha3);
    void stopTrilha3(AudioEngine& audio, ALuint srcTrilha3);

    // ── Estado ────────────────────────────────
    bool isAmb1Playing() const { return amb1Playing; }
    bool isAmb2Playing() const { return amb2Playing; }
    bool isAmb3Playing() const { return amb3Playing; }

private:
    bool amb1Playing = false;
    bool amb2Playing = false;
    bool amb3Playing = false;
};