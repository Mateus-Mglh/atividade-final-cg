#include "audio/AudioPlayer.h"

// ── Passos ────────────────────────────────────
void AudioPlayer::updateStep(bool moving)
{
    stepPlaying = moving;
}

void AudioPlayer::processStep(AudioEngine& audio, ALuint srcPasso)
{
    if (!srcPasso) return;
    ALint state = 0;
    alGetSourcei(srcPasso, AL_SOURCE_STATE, &state);
    bool isPlaying = (state == AL_PLAYING);
    if (stepPlaying && !isPlaying) {
        alSourcei(srcPasso, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcPasso, AL_POSITION, 0, 0, 0);
        audio.play(srcPasso);
    } else if (!stepPlaying && isPlaying) {
        audio.stop(srcPasso);
    }
}

// ── Dano / Respiração ─────────────────────────
void AudioPlayer::playHurt(AudioEngine& audio, ALuint srcHurt)
{
    if (!srcHurt) return;
    audio.stop(srcHurt);
    audio.play(srcHurt);
}

void AudioPlayer::processBreath(AudioEngine& audio, ALuint srcBreath, int playerHealth)
{
    if (!srcBreath) return;
    if (playerHealth > AudioTuning::LOW_HP) {
        audio.setSourceGain(srcBreath, 0.0f);
        return;
    }
    float t = (float)(AudioTuning::LOW_HP - playerHealth) / (float)AudioTuning::LOW_HP;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float gain = AudioTuning::MASTER * AudioTuning::BREATH_GAIN * (0.20f + t * 0.80f);
    audio.setSourceGain(srcBreath, gain);
    ALint st = 0;
    alGetSourcei(srcBreath, AL_SOURCE_STATE, &st);
    if (st != AL_PLAYING) audio.play(srcBreath);
}

void AudioPlayer::initBreath(AudioEngine& audio, ALuint& srcBreath, ALuint bufBreath)
{
    if (!bufBreath || srcBreath) return;
    srcBreath = audio.createSource(bufBreath, true);
    if (srcBreath) {
        alSourcei(srcBreath, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcBreath, AL_POSITION, 0, 0, 0);
        audio.setSourceGain(srcBreath, 0.0f);
        audio.play(srcBreath);
    }
}

// ── Amuleto ───────────────────────────────────
void AudioPlayer::playAmuletPickup(AudioEngine& audio, ALuint srcAmuletPickup)
{
    if (!srcAmuletPickup) return;
    alSourcei(srcAmuletPickup, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(srcAmuletPickup, AL_POSITION, 0, 0, 0);
    audio.play(srcAmuletPickup);
}

void AudioPlayer::ensureAmuletSources(AudioEngine& audio, ALuint bufAmuletHum,
                                       std::vector<ALuint>& srcAmulets, size_t count)
{
    if (!bufAmuletHum) return;
    if (srcAmulets.size() == count) return;
    for (ALuint s : srcAmulets) if (s) audio.stop(s);
    srcAmulets.clear();
    srcAmulets.resize(count, 0);
    for (size_t i = 0; i < count; ++i) {
        ALuint s = audio.createSource(bufAmuletHum, true);
        if (!s) continue;
        alSourcei(s, AL_SOURCE_RELATIVE, AL_FALSE);
        audio.setSourceDistance(s,
            AudioTuning::AMULET_REF_DIST,
            AudioTuning::AMULET_ROLLOFF,
            AudioTuning::AMULET_MAX_DIST);
        audio.setSourceGain(s, AudioTuning::MASTER * AudioTuning::AMULET_HUM_GAIN);
        audio.play(s);
        srcAmulets[i] = s;
    }
}

void AudioPlayer::setAmuletPos(AudioEngine& audio, std::vector<ALuint>& srcAmulets,
                                size_t idx, float x, float z)
{
    if (idx < srcAmulets.size() && srcAmulets[idx])
        audio.setSourcePos(srcAmulets[idx], {x, 0.0f, z});
}

void AudioPlayer::stopAmuletHum(AudioEngine& audio, std::vector<ALuint>& srcAmulets, size_t idx)
{
    if (idx < srcAmulets.size() && srcAmulets[idx])
        audio.stop(srcAmulets[idx]);
}

// ── Heartbeat ─────────────────────────────────
void AudioPlayer::playHeartbeat(AudioEngine& audio, ALuint srcHeartbeat)
{
    if (!srcHeartbeat) return;
    alSourcei(srcHeartbeat, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(srcHeartbeat, AL_POSITION, 0, 0, 0);
    audio.play(srcHeartbeat);
}

void AudioPlayer::startHeartbeat(AudioEngine& audio, ALuint srcHeartbeat)
{
    if (!srcHeartbeat || heartbeatLoop) return;
    alSourcei(srcHeartbeat, AL_LOOPING, AL_TRUE);
    alSourcei(srcHeartbeat, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(srcHeartbeat, AL_POSITION, 0, 0, 0);
    audio.setSourceGain(srcHeartbeat, AudioTuning::MASTER * AudioTuning::HEARTBEAT_GAIN);
    audio.play(srcHeartbeat);
    heartbeatLoop = true;
}

void AudioPlayer::stopHeartbeat(AudioEngine& audio, ALuint srcHeartbeat)
{
    if (srcHeartbeat) audio.stop(srcHeartbeat);
    heartbeatLoop = false;
}