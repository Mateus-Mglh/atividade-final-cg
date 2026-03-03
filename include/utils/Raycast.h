#pragma once

// -------------------------------------------------------
// Raycast
// Funções utilitárias de interseção para tiros (hitscan)
// Não depende de Player, Enemy ou GameContext
// -------------------------------------------------------

// Testa se um raio 2D (origem + direção) acerta um círculo
// Retorna true se houve acerto e escreve a distância do impacto
bool rayHitsCircle(
    float rayOx, float rayOz,   // origem do raio
    float rayDx, float rayDz,   // direção do raio (normalizada)
    float circleX, float circleZ, // centro do alvo
    float radius,               // raio do alvo
    float maxDist,              // alcance máximo do raio
    float &hitDist              // distância até o impacto
);

// Wrapper semântico para inimigos (opcional)
inline bool rayHitsEnemy(
    float rayOx, float rayOz,
    float rayDx, float rayDz,
    float enemyX, float enemyZ,
    float enemyRadius,
    float maxDist,
    float &hitDist
)
{
    return rayHitsCircle(
        rayOx, rayOz,
        rayDx, rayDz,
        enemyX, enemyZ,
        enemyRadius,
        maxDist,
        hitDist
    );
}