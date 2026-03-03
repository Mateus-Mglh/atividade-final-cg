#pragma once

struct GameContext;

void updateStones(GameContext& ctx);
void trySpawnStone(GameContext& ctx);
void tryCollectStones(GameContext& ctx);
void renderStones(GameContext& ctx);
void tryThrowStone(GameContext& ctx);
void updateProjectiles(GameContext& ctx);
void renderProjectiles(GameContext& ctx);