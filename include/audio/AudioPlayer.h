#pragma once
#include <AL/al.h>
#include "audio/AudioEngine.h"
#include "audio/AudioTuning.h"
#include <vector>

class AudioPlayer {
public:
    // ── Passos ────────────────────────────────
    void updateStep(bool moving);
    void processStep(AudioEngine& audio, ALuint srcPasso);

    // ── Dano / Respiração ─────────────────────
    void playHurt(AudioEngine& audio, ALuint srcHurt);
    void processBreath(AudioEngine& audio, ALuint srcBreath, int playerHealth);
    void initBreath(AudioEngine& audio, ALuint& srcBreath, ALuint bufBreath);

    // ── Amuleto ───────────────────────────────
    void playAmuletPickup(AudioEngine& audio, ALuint srcAmuletPickup);
    void ensureAmuletSources(AudioEngine& audio, ALuint bufAmuletHum,
                             std::vector<ALuint>& srcAmulets, size_t count);
    void setAmuletPos(AudioEngine& audio, std::vector<ALuint>& srcAmulets,
                      size_t idx, float x, float z);
    void stopAmuletHum(AudioEngine& audio, std::vector<ALuint>& srcAmulets, size_t idx);

    // ── Heartbeat ─────────────────────────────
    void playHeartbeat(AudioEngine& audio, ALuint srcHeartbeat);
    void startHeartbeat(AudioEngine& audio, ALuint srcHeartbeat);
    void stopHeartbeat(AudioEngine& audio, ALuint srcHeartbeat);

private:
    bool stepPlaying   = false;
    bool heartbeatLoop = false;
};