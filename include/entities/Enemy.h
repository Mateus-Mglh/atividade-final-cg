#ifndef ENEMY_H
#define ENEMY_H
#include <deque>
#include <utility>

struct GameContext;

class Enemy
{
public:
    Enemy(float startX, float startZ);
    void update(GameContext& ctx, float deltaTime);
    void applyStun(float duration);
    void sinkDown(float deltaTime);
    bool isStunned() const;

    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    float getHalfWidth() const { return 0.4f; }
    float getHalfDepth() const { return 0.4f; }
    float getHeight() const    { return 3.0f; }

private:
    float x, y, z;
    float speed;
    float radius;
    std::deque<std::pair<float,float>> path;
    float pathTimer = 0.0f;
    float stunTimer = 0.0f;

    void updatePath(GameContext& ctx);
    void moveAlongPath(GameContext& ctx, float deltaTime);
};
#endif