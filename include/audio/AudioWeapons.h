#pragma once
#include <AL/al.h>
#include "audio/AudioEngine.h"

class AudioWeapons {
public:
    void playShot(AudioEngine& audio, ALuint srcShot);
    void playReload(AudioEngine& audio, ALuint srcReload);
    void playReloadClick(AudioEngine& audio, ALuint srcReloadClick);
    void playBulletImpact(AudioEngine& audio, ALuint srcBulletImpact);
};