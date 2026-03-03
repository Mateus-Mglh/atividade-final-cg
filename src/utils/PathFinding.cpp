#include "utils/Pathfinding.h"
#include "core/GameContext.h"
#include "level/LevelMetrics.h"
#include <deque>
#include <queue>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

struct Node
{
    int x, z;
    float g, f;
    bool operator>(const Node& o) const { return f > o.f; }
};

static bool isWalkableTile(GameContext& ctx, int tx, int tz)
{
    const auto& data = ctx.level.map.data();
    if (tz < 0 || tz >= (int)data.size()) return false;
    if (tx < 0 || tx >= (int)data[tz].size()) return false;
    char c = data[tz][tx];
    return (c != '1' && c != 'T');
}

static int toIndex(int x, int z, int width)
{
    return z * width + x;
}

std::deque<std::pair<float,float>> findPath(
    GameContext& ctx,
    float startX, float startZ,
    float goalX,  float goalZ)
{
    const LevelMetrics& m = ctx.level.metrics;
    float tile = m.tile;

    // Converte posições mundo → grid
    int sx = (int)std::floor((startX - m.offsetX) / tile);
    int sz = (int)std::floor((startZ - m.offsetZ) / tile);
    int gx = (int)std::round((goalX - m.offsetX) / tile - 0.5f);
    int gz = (int)std::round((goalZ - m.offsetZ) / tile - 0.5f);

    // Verifica se start e goal estão dentro do mapa
    if (!isWalkableTile(ctx, sx, sz))
        return {};

    if (!isWalkableTile(ctx, gx, gz))
        return {};

    int W = ctx.level.map.getWidth();
    int H = ctx.level.map.getHeight();

    // Vizinhos (8 direções)
    const int dx[] = { 0, 0, 1, -1,  1,  1, -1, -1 };
    const int dz[] = { 1,-1, 0,  0,  1, -1,  1, -1 };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
    std::unordered_map<int, int>   cameFrom;
    std::unordered_map<int, float> gScore;
    std::unordered_set<int> closed;

    int startIdx = toIndex(sx, sz, W);
    int goalIdx  = toIndex(gx, gz, W);

    gScore[startIdx] = 0.0f;

    float dxStart = std::abs(sx - gx);
    float dzStart = std::abs(sz - gz);

    float h = (dxStart + dzStart) + (1.41421356f - 2.0f) * std::min(dxStart, dzStart);

    open.push({sx, sz, 0.0f, h});

    bool found = false;

    while (!open.empty())
    {
        Node cur = open.top(); open.pop();
        int curIdx = toIndex(cur.x, cur.z, W);

        if (closed.count(curIdx)) continue;
        closed.insert(curIdx);

        if (curIdx == goalIdx) { found = true; break; }

        for (int i = 0; i < 8; i++)
        {
            int nx = cur.x + dx[i];
            int nz = cur.z + dz[i];

            if (!isWalkableTile(ctx, nx, nz)) continue;

            bool diagonal = (dx[i] != 0 && dz[i] != 0);

            // Evita cortar canto
            if (diagonal)
            {
                if (!isWalkableTile(ctx, cur.x + dx[i], cur.z) ||
                    !isWalkableTile(ctx, cur.x, cur.z + dz[i]))
                    continue;
            }

            float moveCost = diagonal ? 1.41421356f : 1.0f;

            int nIdx = toIndex(nx, nz, W);
            float ng = gScore[curIdx] + moveCost;

            if (gScore.find(nIdx) == gScore.end() || ng < gScore[nIdx])
            {
                gScore[nIdx]   = ng;
                cameFrom[nIdx] = curIdx;

                float dxGoal = std::abs(nx - gx);
                float dzGoal = std::abs(nz - gz);

                float h = (dxGoal + dzGoal) + (1.41421356f - 2.0f) * std::min(dxGoal, dzGoal);

                open.push({nx, nz, ng, ng + h});
            }
        }
    }

    std::deque<std::pair<float,float>> path;
    if (!found) return path;

    // Reconstrói caminho
    int cur = goalIdx;
    while (cur != startIdx)
    {
        int tx = cur % W;
        int tz = cur / W;
        float wx, wz;
        m.tileCenter(tx, tz, wx, wz);
        path.push_back({wx, wz});
        cur = cameFrom[cur];
    }

    std::reverse(path.begin(), path.end());
    return path;
}