#include "audio/AudioManager.h"
#include "audio/AudioAmbience.h"
#include "audio/AudioTuning.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>

// ─────────────────────────────────────────────────────────────
//  Ctor / init / shutdown
// ─────────────────────────────────────────────────────────────
AudioManager::AudioManager(AudioEngine& engine)
    : mAudio(engine) {}

void AudioManager::init()
{
    if (mOk) return;

    mOk = mAudio.init();
    if (!mOk) {
        std::printf("[Audio] Falha ao iniciar OpenAL\n");
        return;
    }

    mAudio.setDistanceModel();
    loadBuffers();
    createSources();
    std::printf("[Audio] AudioManager inicializado.\n");
}

void AudioManager::shutdown()
{
    mAudio.shutdown();
    mOk = false;
}

// ─────────────────────────────────────────────────────────────
//  Helpers internos
// ─────────────────────────────────────────────────────────────
ALuint AudioManager::tryLoad(const char* mono, const char* stereo)
{
    ALuint buf = mAudio.loadWav(mono);
    if (!buf && stereo)
        buf = mAudio.loadWav(stereo);
    if (!buf)
        std::printf("[Audio] NAO encontrado: %s\n", mono);
    return buf;
}

void AudioManager::play2D(ALuint src)
{
    if (!src) return;
    alSourcei(src, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(src, AL_POSITION, 0, 0, 0);
    mAudio.stop(src);
    mAudio.play(src);
}

void AudioManager::playAt(ALuint src, float x, float z)
{
    if (!src) return;
    mAudio.setSourcePos(src, {x, 0.0f, z});
    mAudio.stop(src);
    mAudio.play(src);
}

// ─────────────────────────────────────────────────────────────
//  loadBuffers  – carrega todos os WAVs
// ─────────────────────────────────────────────────────────────
void AudioManager::loadBuffers()
{
    // Fase 1
    bufChuva        = tryLoad("assets/audio/sons/chuva.wav");
    bufFloresta     = tryLoad("assets/audio/sons/floresta_fase1.wav");
    bufPasso        = tryLoad("assets/audio/sons/passos_v2.wav");
    bufHurt         = tryLoad("assets/audio/sons/humano_dor.wav");
    bufAmuletPickup = tryLoad("assets/audio/sons/peguei_amuleto.wav");
    bufMapScream    = tryLoad("assets/audio/sons/mapinguari_apresentacao.wav");
    bufMapLoop      = tryLoad("assets/audio/sons/mapinguari_perseguicao.wav");
    bufMonsterHit   = tryLoad("assets/audio/sons/monstro_golpeado.wav");
    bufPassoMonstro = tryLoad("assets/audio/sons/passo_monstro.wav");

    // Fase 2
    bufBoatHit      = tryLoad("assets/audio/sons/batida_barco.wav");
    bufSnakeAttack  = tryLoad("assets/audio/sons/cobra_ataque.wav");
    bufSnakeRattle  = tryLoad("assets/audio/sons/cobra_inicial.wav");
    bufRiver        = tryLoad("assets/audio/sons/rio_som.wav");
    // Fase 3
    bufBreath       = tryLoad("assets/audio/sons/breath_mono.wav");
    bufShot         = tryLoad("assets/audio/sons/shot_mono.wav");
    bufReload       = tryLoad("assets/audio/sons/reload_mono.wav");
    bufReloadClick  = tryLoad("assets/audio/sons/click_reload_mono.wav");
    bufMinionSound  = tryLoad("assets/audio/sons/botinho_som.wav");
    bufBotoDead     = tryLoad("assets/audio/sons/boto_abatido.wav");
    bufBotoSound    = tryLoad("assets/audio/sons/botossauro_rex.wav");
    bufBulletImpact = tryLoad("assets/audio/sons/impacto_do_chumbu.wav");
    bufTrilha3      = tryLoad("assets/audio/sons/trilha_sonora3.wav");
}

// ─────────────────────────────────────────────────────────────
//  createSources – cria sources com configurações padrão
// ─────────────────────────────────────────────────────────────
void AudioManager::createSources()
{
    // ── Ambiente fase 1 (2D loop) ─────────────
    if (bufChuva) {
        srcChuva = mAudio.createSource(bufChuva, true);
        alSourcei(srcChuva, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcChuva, AL_POSITION, 0, 0, 0);
        mAudio.setSourceDistance(srcChuva, 1.0f, 0.0f, 1000.0f);
        mAudio.setSourceGain(srcChuva, AudioTuning::MASTER * AudioTuning::CHUVA_GAIN);
        std::printf("[Audio] Chuva OK\n");
    }
    if (bufFloresta) {
        srcFloresta = mAudio.createSource(bufFloresta, true);
        alSourcei(srcFloresta, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcFloresta, AL_POSITION, 0, 0, 0);
        mAudio.setSourceDistance(srcFloresta, 1.0f, 0.0f, 1000.0f);
        mAudio.setSourceGain(srcFloresta, AudioTuning::MASTER * AudioTuning::FLORESTA_GAIN);
        std::printf("[Audio] Floresta OK\n");
    }

    // ── Passos (2D loop) ──────────────────────
    if (bufPasso) {
        srcPasso = mAudio.createSource(bufPasso, true);
        alSourcei(srcPasso, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcPasso, AL_POSITION, 0, 0, 0);
        mAudio.setSourceDistance(srcPasso, 1.0f, 0.0f, 1000.0f);
        mAudio.setSourceGain(srcPasso, AudioTuning::MASTER * AudioTuning::STEP_GAIN);
        std::printf("[Audio] Passo OK\n");
    }

    // ── Passos Monstro (3D loop) ──────────────────────
    if (bufPassoMonstro) 
    {
        srcPassoMonstro = mAudio.createSource(bufPassoMonstro, true); // loop
        alSourcei(srcPassoMonstro, AL_SOURCE_RELATIVE, AL_FALSE); // 3D real
        mAudio.setSourceDistance(srcPassoMonstro,
            AudioTuning::MAP_REF_DIST,
            AudioTuning::MAP_ROLLOFF,
            AudioTuning::MAP_MAX_DIST);
        mAudio.setSourceGain(srcPassoMonstro, AudioTuning::MASTER * AudioTuning::MAP_LOOP_GAIN);
        mAudio.stop(srcPassoMonstro);
        std::printf("[Audio] PassoMonstro OK\n");
    }

    // ── Hurt (2D one-shot) ────────────────────
    if (bufHurt) {
        srcHurt = mAudio.createSource(bufHurt, false);
        alSourcei(srcHurt, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcHurt, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcHurt, AudioTuning::MASTER * AudioTuning::HURT_GAIN);
        std::printf("[Audio] Hurt OK\n");
    }

    // ── Amuleto pickup (2D one-shot) ──────────
    if (bufAmuletPickup) {
        srcAmuletPickup = mAudio.createSource(bufAmuletPickup, false);
        alSourcei(srcAmuletPickup, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcAmuletPickup, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcAmuletPickup, AudioTuning::MASTER * AudioTuning::AMULET_PICKUP_GAIN);
        std::printf("[Audio] AmuletPickup OK\n");
    }

    // ── Mapinguari scream (2D one-shot) ───────
    if (bufMapScream) {
        srcMapScream = mAudio.createSource(bufMapScream, false);
        alSourcei(srcMapScream, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcMapScream, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcMapScream, AudioTuning::MASTER * AudioTuning::MAP_SCREAM_GAIN);
        std::printf("[Audio] MapinguariScream OK\n");
    }

    // ── Mapinguari loop (3D) ──────────────────
    if (bufMapLoop) {
        srcMapLoop = mAudio.createSource(bufMapLoop, false); // ← era true, agora false
        alSourcei(srcMapLoop, AL_SOURCE_RELATIVE, AL_FALSE);
        mAudio.setSourceDistance(srcMapLoop,
            AudioTuning::MAP_REF_DIST,
            AudioTuning::MAP_ROLLOFF,
            AudioTuning::MAP_MAX_DIST);
        mAudio.setSourceGain(srcMapLoop, AudioTuning::MASTER * AudioTuning::MAP_LOOP_GAIN);
        mAudio.stop(srcMapLoop);
        std::printf("[Audio] MapinguariLoop OK\n");
    }

    // ── Monstro golpeado (2D one-shot) ────────
    if (bufMonsterHit) {
        srcMonsterHit = mAudio.createSource(bufMonsterHit, false);
        alSourcei(srcMonsterHit, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcMonsterHit, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcMonsterHit, AudioTuning::MASTER * AudioTuning::MONSTER_HIT_GAIN);
        std::printf("[Audio] MonsterHit OK\n");
    }

    // ── Fase 2 ────────────────────────────────
    if (bufSnakeRattle) {
        srcSnakeRattle = mAudio.createSource(bufSnakeRattle, false);
        alSourcei(srcSnakeRattle, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcSnakeRattle, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcSnakeRattle, AudioTuning::SNAKE_RATTLE_GAIN);
        std::printf("[Audio] SnakeRattle OK\n");
    }
    if (bufBoatHit) {
        srcBoatHit = mAudio.createSource(bufBoatHit, false);
        alSourcei(srcBoatHit, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcBoatHit, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcBoatHit, AudioTuning::BOAT_HIT_GAIN);
        std::printf("[Audio] BoatHit OK\n");
    }
    if (bufSnakeAttack) {
        srcSnakeAttack = mAudio.createSource(bufSnakeAttack, false);
        alSourcei(srcSnakeAttack, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcSnakeAttack, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcSnakeAttack, AudioTuning::SNAKE_ATTACK_GAIN);
        std::printf("[Audio] SnakeAttack OK\n");
    }
    if (bufRiver) {
        srcRiver = mAudio.createSource(bufRiver, true); // loop
        alSourcei(srcRiver, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcRiver, AL_POSITION, 0, 0, 0);
        mAudio.setSourceDistance(srcRiver, 1.0f, 0.0f, 1000.0f);
        mAudio.setSourceGain(srcRiver, AudioTuning::RIVER_GAIN);
        std::printf("[Audio] River OK\n");
    }

    // ── Fase 3 – tiro ─────────────────────────
    if (bufShot) {
        srcShot = mAudio.createSource(bufShot, false);
        alSourcei(srcShot, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcShot, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcShot, AudioTuning::MASTER * AudioTuning::SHOT_GAIN);
        std::printf("[Audio] Shot OK\n");
    }

    // ── Fase 3 – recarga ──────────────────────
    if (bufReload) {
        srcReload = mAudio.createSource(bufReload, false);
        alSourcei(srcReload, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcReload, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcReload, AudioTuning::MASTER * AudioTuning::RELOAD_GAIN);
        std::printf("[Audio] Reload OK\n");
    }
    if (bufReloadClick) {
        srcReloadClick = mAudio.createSource(bufReloadClick, false);
        alSourcei(srcReloadClick, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcReloadClick, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcReloadClick, AudioTuning::MASTER * AudioTuning::RELOAD_CLICK_GAIN);
        std::printf("[Audio] ReloadClick OK\n");
    }

    // ── Fase 3 – impacto do tiro ──────────────
    if (bufBulletImpact) {
        srcBulletImpact = mAudio.createSource(bufBulletImpact, false);
        alSourcei(srcBulletImpact, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcBulletImpact, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcBulletImpact, AudioTuning::MASTER * AudioTuning::BULLET_IMPACT_GAIN);
        std::printf("[Audio] BulletImpact OK\n");
    }

    // ── Fase 3 – boto abatido (one-shot) ──────
    if (bufBotoDead) {
        srcBotoDead = mAudio.createSource(bufBotoDead, false);
        alSourcei(srcBotoDead, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcBotoDead, AL_POSITION, 0, 0, 0);
        mAudio.setSourceGain(srcBotoDead, AudioTuning::MASTER * AudioTuning::BOTO_DEAD_GAIN);
        std::printf("[Audio] BotoDead OK\n");
    }

    // ── Fase 3 – boto som (3D loop) ───────────
    if (bufBotoSound) {
        srcBotoSound = mAudio.createSource(bufBotoSound, true);
        alSourcei(srcBotoSound, AL_SOURCE_RELATIVE, AL_FALSE);
        mAudio.setSourceDistance(srcBotoSound,
            AudioTuning::BOTO_REF_DIST,
            AudioTuning::BOTO_ROLLOFF,
            AudioTuning::BOTO_MAX_DIST);
        mAudio.setSourceGain(srcBotoSound, AudioTuning::MASTER * AudioTuning::BOTO_SOUND_GAIN);
        mAudio.stop(srcBotoSound);
        std::printf("[Audio] BotoSound OK\n");
    }

    // ── Fase 3 – minion som (3D loop) ─────────
    if (bufMinionSound) {
        srcMinionSound = mAudio.createSource(bufMinionSound, true);
        alSourcei(srcMinionSound, AL_SOURCE_RELATIVE, AL_FALSE);
        mAudio.setSourceDistance(srcMinionSound,
            AudioTuning::BOTO_REF_DIST,
            AudioTuning::BOTO_ROLLOFF,
            AudioTuning::BOTO_MAX_DIST);
        mAudio.setSourceGain(srcMinionSound, AudioTuning::MINION_SOUND_GAIN);
        mAudio.stop(srcMinionSound);
        std::printf("[Audio] MinionSound OK\n");
    }

    // ── Fase 3 – trilha sonora (2D loop) ──────
    if (bufTrilha3) {
        srcTrilha3 = mAudio.createSource(bufTrilha3, true);
        alSourcei(srcTrilha3, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(srcTrilha3, AL_POSITION, 0, 0, 0);
        mAudio.setSourceDistance(srcTrilha3, 1.0f, 0.0f, 1000.0f);
        mAudio.setSourceGain(srcTrilha3, AudioTuning::TRILHA3_GAIN);
        std::printf("[Audio] Trilha3 OK\n");
    }

    // ── Fase 3 – respiração (2D loop) ────────────
    if (bufBreath) {
        srcBreath = mAudio.createSource(bufBreath, true); // loop
        alSourcei(srcBreath, AL_SOURCE_RELATIVE, AL_TRUE); // 2D
        alSource3f(srcBreath, AL_POSITION, 0, 0, 0);
        mAudio.setSourceDistance(srcBreath, 1.0f, 0.0f, 1000.0f);
        mAudio.setSourceGain(srcBreath, AudioTuning::MASTER * AudioTuning::BREATH_GAIN);
        mAudio.stop(srcBreath); // começa parado
        std::printf("[Audio] Breath OK\n");
    }
}

// ─────────────────────────────────────────────────────────────
//  Listener
// ─────────────────────────────────────────────────────────────
void AudioManager::updateListener(float x, float y, float z,
                                   float yawDeg, float pitchDeg)
{
    if (!mOk) return;
    float ry = yawDeg   * 3.14159f / 180.0f;
    float rp = pitchDeg * 3.14159f / 180.0f;
    Vec3 fwd = { cosf(rp)*sinf(ry), sinf(rp), -cosf(rp)*cosf(ry) };
    Vec3 up  = { 0.0f, 1.0f, 0.0f };
    Vec3 vel = { 0.0f, 0.0f, 0.0f };
    mAudio.setListener({x,y,z}, vel, fwd, up);
}

// ─────────────────────────────────────────────────────────────
//  Fase 1 – Ambiente
// ─────────────────────────────────────────────────────────────
void AudioManager::startAmbientPhase1() { if (mOk) ambience.startPhase1(mAudio, srcChuva, srcFloresta); }
void AudioManager::stopAmbientPhase1()  { ambience.stopPhase1(mAudio, srcChuva, srcFloresta); }
void AudioManager::stopChuva()          { ambience.stopChuva(mAudio, srcChuva); }

// ─────────────────────────────────────────────────────────────
//  Fase 1 – Amuleto
// ─────────────────────────────────────────────────────────────
void AudioManager::playAmuletPickup()                { if (mOk) player.playAmuletPickup(mAudio, srcAmuletPickup); }
void AudioManager::ensureAmuletSources(size_t count) { if (mOk) player.ensureAmuletSources(mAudio, bufAmuletHum, srcAmulets, count); }
void AudioManager::setAmuletPos(size_t idx, float x, float z) { player.setAmuletPos(mAudio, srcAmulets, idx, x, z); }
void AudioManager::stopAmuletHum(size_t idx)         { player.stopAmuletHum(mAudio, srcAmulets, idx); }

// ─────────────────────────────────────────────────────────────
//  Fase 1 – Coração
// ─────────────────────────────────────────────────────────────
void AudioManager::playHeartbeat()                   { if (mOk) player.playHeartbeat(mAudio, srcHeartbeat); }
void AudioManager::startHeartbeat()                  { if (mOk) player.startHeartbeat(mAudio, srcHeartbeat); }
void AudioManager::stopHeartbeat()                   { player.stopHeartbeat(mAudio, srcHeartbeat); }

// ─────────────────────────────────────────────────────────────
//  Fase 1 – Mapinguari
// ─────────────────────────────────────────────────────────────
void AudioManager::startMapinguariIntro()                          { if (mOk) enemies.startMapinguariIntro(mAudio, srcMapScream, srcPassoMonstro); }
void AudioManager::playMapinguariScream()                          { if (mOk) enemies.playMapinguariScream(mAudio, srcMapScream); }
void AudioManager::stopMapinguariScream()                          { enemies.stopMapinguariScream(mAudio, srcMapScream); }
void AudioManager::startMapinguariLoop(float x, float z)           { if (mOk) enemies.startMapinguariLoop(mAudio, srcMapLoop, x, z); }
void AudioManager::stopMapinguariLoop()                            { enemies.stopMapinguariLoop(mAudio, srcMapLoop, srcPassoMonstro); }
void AudioManager::setMapinguariPos(float x, float z)              { enemies.setMapinguariPos(mAudio, srcMapLoop, srcPassoMonstro, x, z); }
void AudioManager::tickMapinguariScream(float dt, float dist)      { if (mOk) enemies.tickMapinguariScream(mAudio, srcMapLoop, dt, dist); }
void AudioManager::pauseMapinguariLoop(float duration)             { enemies.pauseMapinguariLoop(mAudio, srcMapLoop, srcPassoMonstro, duration); }
void AudioManager::playBossSealed()                                { if (mOk) enemies.playBossSealed(mAudio, srcMapScream); }
void AudioManager::playMonsterHit()                                { if (mOk) enemies.playMonsterHit(mAudio, srcMonsterHit); }

// ─────────────────────────────────────────────────────────────
//  Passos do jogador
// ─────────────────────────────────────────────────────────────
void AudioManager::audioUpdateStep(bool moving)      { player.updateStep(moving); }
void AudioManager::processStepAudio()                { if (mOk) player.processStep(mAudio, srcPasso); }

// ─────────────────────────────────────────────────────────────
//  Dano / Respiração
// ─────────────────────────────────────────────────────────────
void AudioManager::audioPlayHurt()                   { if (mOk) player.playHurt(mAudio, srcHurt); }
void AudioManager::processBreathAudio(int health)    { if (mOk) player.processBreath(mAudio, srcBreath, health); }
void AudioManager::initBreath()                      { if (mOk) player.initBreath(mAudio, srcBreath, bufBreath); }

// ─────────────────────────────────────────────────────────────
//  Fase 2 – Ambiente
// ─────────────────────────────────────────────────────────────
void AudioManager::playSnakeRattle()                               { if (mOk) enemies.playSnakeRattle(mAudio, srcSnakeRattle); }
void AudioManager::playBoatHit()                                   { if (mOk) enemies.playBoatHit(mAudio, srcBoatHit); }
void AudioManager::playSnakeAttack()                               { if (mOk) enemies.playSnakeAttack(mAudio, srcSnakeAttack); }
void AudioManager::startAmbientPhase2() { if (mOk) ambience.startPhase2(mAudio, srcFloresta, srcRiver); }
void AudioManager::stopAmbientPhase2()  { ambience.stopPhase2(mAudio, srcFloresta, srcRiver); }

// ─────────────────────────────────────────────────────────────
//  Fase 2 – Rio
// ─────────────────────────────────────────────────────────────
void AudioManager::startRiver()         { if (mOk) ambience.startRiver(mAudio, srcRiver); }
void AudioManager::stopRiver()          { ambience.stopRiver(mAudio, srcRiver); }

// ─────────────────────────────────────────────────────────────
//  Fase 3 – Armas
// ─────────────────────────────────────────────────────────────
void AudioManager::playShot()         { if (mOk) weapons.playShot(mAudio, srcShot); }
void AudioManager::playReload()       { if (mOk) weapons.playReload(mAudio, srcReload); }
void AudioManager::playReloadClick()  { if (mOk) weapons.playReloadClick(mAudio, srcReloadClick); }
void AudioManager::playBulletImpact() { if (mOk) weapons.playBulletImpact(mAudio, srcBulletImpact); }

// ─────────────────────────────────────────────────────────────
//  Fase 3 – Boto
// ─────────────────────────────────────────────────────────────
void AudioManager::startBotoLoop(float x, float z)                 { if (mOk) enemies.startBotoLoop(mAudio, srcBotoSound, x, z); }

// ─────────────────────────────────────────────────────────────
//  Fase 3 – Trilha sonora
// ─────────────────────────────────────────────────────────────
void AudioManager::startTrilha3()       { if (mOk) ambience.startTrilha3(mAudio, srcTrilha3, bufTrilha3); }
void AudioManager::stopTrilha3()        { ambience.stopTrilha3(mAudio, srcTrilha3); }

// ─────────────────────────────────────────────────────────────
//  Fase 3 – Ambiente (atualiza startAmbientPhase3)
// ─────────────────────────────────────────────────────────────
void AudioManager::startAmbientPhase3() { if (mOk) ambience.startPhase3(mAudio, srcTrilha3, bufTrilha3); }
void AudioManager::stopAmbientPhase3()  { ambience.stopPhase3(mAudio, srcTrilha3); }

// ─────────────────────────────────────────────────────────────
//  Fase 3 – Boto abatido
// ─────────────────────────────────────────────────────────────
void AudioManager::stopBotoLoop()                                  { enemies.stopBotoLoop(mAudio, srcBotoSound); }
void AudioManager::setBotoPos(float x, float z)                    { enemies.setBotoPos(mAudio, srcBotoSound, x, z); }
void AudioManager::playBotoDead()                                  { if (mOk) enemies.playBotoDead(mAudio, srcBotoDead); }
void AudioManager::playMinionSound()                               { if (mOk) enemies.playMinionSound(mAudio, srcMinionSound); }
bool AudioManager::isBotoDeadPlaying() const                       { return enemies.isBotoDeadPlaying(srcBotoDead); }

void AudioManager::stopPhase3Loops()
{
    if (!mOk) return;

    enemies.stopMinionSound(mAudio, srcMinionSound);
    ambience.stopPhase3(mAudio, srcTrilha3);
    enemies.stopBotoLoop(mAudio, srcBotoSound);
}

// ─────────────────────────────────────────────────────────────
//  update() – chamado todo frame
// ─────────────────────────────────────────────────────────────
void AudioManager::update(float dt,
                           float listenerX, float listenerZ,
                           const std::vector<EnemyAudioInfo>& /*mapinguaris*/,
                           int playerHealth,
                           bool playerMoving,
                           bool inTransition)
{
    if (!mOk) return;
    audioUpdateStep(playerMoving && !inTransition);
    processStepAudio();
    processBreathAudio(playerHealth);

    // Bloco 1 — espera apresentacao terminar para iniciar loop
    if (enemies.isMapIntroPlaying())
    {
        enemies.tickMapIntro(dt);
        if (!enemies.isMapIntroPlaying() && !enemies.isMapLoopPlaying())
            enemies.startMapinguariLoop(mAudio, srcMapLoop, 0.0f, 0.0f);
    }

    // Bloco 2 — pausa do loop durante o stun
    enemies.tickPauseTimer(dt, mAudio, srcPassoMonstro);

    (void)listenerX; (void)listenerZ;
}

void AudioManager::stopAllSounds()
{
    if (!mOk) return;
    stopAmbientPhase1();
    stopAmbientPhase2();
    stopAmbientPhase3();
    stopMapinguariLoop();
    stopBotoLoop();
    stopHeartbeat();

    std::vector<ALuint> allSources = {
        srcChuva, srcFloresta, srcPasso, srcPassoMonstro,
        srcHurt, srcAmuletPickup,
        srcMapScream, srcMapLoop, srcMonsterHit,
        srcSnakeRattle, srcBoatHit, srcSnakeAttack, srcRiver,
        srcShot, srcReload, srcReloadClick,
        srcBulletImpact,
        srcTrilha3,
        srcBotoSound, srcBotoDead,
        srcBreath
    };
    for (ALuint s : allSources)
        if (s) mAudio.stop(s);
    for (ALuint s : srcAmulets)
        if (s) mAudio.stop(s);
    for (ALuint s : srcMinions)
        if (s) mAudio.stop(s);
}