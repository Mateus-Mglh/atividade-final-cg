#include "audio/AudioWeapons.h"

void AudioWeapons::playShot(AudioEngine& audio, ALuint srcShot)
{
    if (!srcShot) return;
    audio.stop(srcShot);
    audio.play(srcShot);
}

void AudioWeapons::playReload(AudioEngine& audio, ALuint srcReload)
{
    if (!srcReload) return;
    audio.stop(srcReload);
    audio.play(srcReload);
}

void AudioWeapons::playReloadClick(AudioEngine& audio, ALuint srcReloadClick)
{
    if (!srcReloadClick) return;
    audio.stop(srcReloadClick);
    audio.play(srcReloadClick);
}

void AudioWeapons::playBulletImpact(AudioEngine& audio, ALuint srcBulletImpact)
{
    if (!srcBulletImpact) return;
    audio.stop(srcBulletImpact);
    audio.play(srcBulletImpact);
}