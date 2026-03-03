#pragma once
#include <algorithm>

struct GameContext;
struct DropItem;
struct Minion;

class Player
{
public:
    enum WeaponState
    {
        W_IDLE,
        W_FIRE_1,
        W_FIRE_2,
        W_PUMP,
        W_RETURN,
        W_RELOAD_1,
        W_RELOAD_2,
        W_RELOAD_3
    };

    static constexpr int   MAX_MAGAZINE     = 12;
    static constexpr int   MAX_RESERVE_AMMO = 25;
    static constexpr float PUMP_TIME        = 0.15f;

    void init();
    void update(float dt, GameContext& ctx);
    void shoot(GameContext& ctx);
    void tryReload(GameContext& ctx);

    // Getters
    int   getHealth()       const { return health; }
    int   getCurrentAmmo()  const { return currentAmmo; }
    int   getReserveAmmo()  const { return reserveAmmo; }
    bool  hasWeapon()       const { return hasGun; }
    bool  isWeaponVisible() const { return weaponVisible; }
    bool  getIsReloading()  const { return isReloading; }
    float getHealthAlpha()  const { return healthAlpha; }
    WeaponState getWeaponState() const { return weaponState; }
    bool weaponFiring = false; // usado pelo HUD

    // Setters
    void setHasGun(bool value)        { hasGun = value; }
    void setWeaponVisible(bool value) { weaponVisible = value; }
    void addHealth(int amount)        { health = std::min(health + amount, 100); }
    void takeDamage(int amount)       { health -= amount; }
    void addAmmo(int amount)          { reserveAmmo = std::min(reserveAmmo + amount, MAX_RESERVE_AMMO); }
    void setHealthAlpha(float value)  { healthAlpha = value; }

    // Flags de input
    bool shootRequested  = false;
    bool reloadRequested = false;

private:
    // Vida e munição
    int   health      = 100;
    int   currentAmmo = MAX_MAGAZINE;
    int   reserveAmmo = MAX_RESERVE_AMMO;

    // Arma
    bool        isReloading   = false;
    bool        hasGun        = false;
    bool        weaponVisible = false;
    float       healthAlpha   = 0.0f;
    WeaponState weaponState   = W_IDLE;
    float       weaponTimer   = 0.0f;
};