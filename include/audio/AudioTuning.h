#pragma once

namespace AudioTuning
{
    // ── Master ────────────────────────────────
    static constexpr float MASTER          = 1.0f;
    // ── Ambiente ──────────────────────────────
    static constexpr float CHUVA_GAIN      = 0.35f;
    static constexpr float FLORESTA_GAIN   = 0.45f;
    // ── Passos ────────────────────────────────
    static constexpr float STEP_GAIN       = 1.0f;
    // ── Player ────────────────────────────────
    static constexpr float HURT_GAIN       = 1.00f;
    static constexpr int   LOW_HP          = 50;
    static constexpr float BREATH_GAIN     = 1.80f;
    // ── Amuleto ───────────────────────────────
    static constexpr float AMULET_PICKUP_GAIN = 1.00f;
    static constexpr float AMULET_HUM_GAIN    = 0.60f;
    static constexpr float AMULET_REF_DIST    = 1.50f;
    static constexpr float AMULET_ROLLOFF     = 1.80f;
    static constexpr float AMULET_MAX_DIST    = 10.0f;
    // ── Coração batendo ───────────────────────
    static constexpr float HEARTBEAT_GAIN  = 0.85f;
    // ── Mapinguari ────────────────────────────
    static constexpr float MAP_SCREAM_GAIN = 0.20f;
    static constexpr float MAP_LOOP_GAIN   = 4.50f;
    static constexpr float MAP_REF_DIST    = 1.20f;
    static constexpr float MAP_ROLLOFF     = 2.80f;
    static constexpr float MAP_MAX_DIST    = 9.00f;
    static constexpr float MAP_SCREAM_MIN  = 1.0f;
    static constexpr float MAP_SCREAM_MAX  = 3.0f;
    static constexpr float MAP_SPOT_DIST   = 10.0f;
    // ── Monstro golpeado ─────────────────────
    static constexpr float MONSTER_HIT_GAIN = 1.60f;
    // ── Fase 2 ────────────────────────────────
    static constexpr float SNAKE_RATTLE_GAIN  = 1.00f;
    static constexpr float BOAT_HIT_GAIN      = 0.75f;
    static constexpr float SNAKE_ATTACK_GAIN  = 1.40f;
    static constexpr float RIVER_GAIN         = 0.70f;
    // ── Fase 3 – Armas ────────────────────────
    static constexpr float SHOT_GAIN           = 0.65f;
    static constexpr float RELOAD_GAIN         = 0.60f;
    static constexpr float RELOAD_CLICK_GAIN   = 0.50f;
    static constexpr float BULLET_IMPACT_GAIN  = 15.0f;
    // ── Boto ──────────────────────────────────
    static constexpr float BOTO_LOOP_GAIN      = 0.75f;
    static constexpr float BOTO_THROW_GAIN     = 1.00f;
    static constexpr float BOTO_REF_DIST       = 1.50f;
    static constexpr float BOTO_ROLLOFF        = 2.00f;
    static constexpr float BOTO_MAX_DIST       = 15.0f;
    static constexpr float BOTO_DEAD_GAIN      = 1.00f;
    static constexpr float BOTO_SOUND_GAIN     = 1.15f;
    // ── Minions ───────────────────────────────
    static constexpr float MINION_SOUND_GAIN   = 1.80f;
    static constexpr float MINION_REF_DIST     = 1.00f;
    static constexpr float MINION_ROLLOFF      = 2.50f;
    static constexpr float MINION_MAX_DIST     = 10.00f;
    static constexpr float MINION_START_DIST   = 6.00f;
    static constexpr float MINION_STOP_DIST    = 7.50f;
    // ── Trilha ────────────────────────────────
    static constexpr float TRILHA3_GAIN        = 1.0f;
}