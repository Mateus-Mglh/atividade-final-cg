#include "utils/raycast.h"
#include <cmath>

// Testa interseção entre um raio 2D e um círculo
bool rayHitsCircle(
    float rayOx, float rayOz,
    float rayDx, float rayDz,
    float circleX, float circleZ,
    float radius,
    float maxDist,
    float &hitDist
)
{
    // Vetor da origem do raio até o centro do círculo
    float vx = circleX - rayOx;
    float vz = circleZ - rayOz;

    // Projeção do vetor no raio (produto escalar)
    float t = vx * rayDx + vz * rayDz;

    // Se estiver atrás da origem ou além do alcance, ignora
    if (t < 0.0f || t > maxDist)
        return false;

    // Ponto mais próximo do raio ao centro do círculo
    float closestX = rayOx + rayDx * t;
    float closestZ = rayOz + rayDz * t;

    // Distância do centro ao ponto mais próximo
    float dx = circleX - closestX;
    float dz = circleZ - closestZ;
    float distSq = dx * dx + dz * dz;

    // Teste de interseção
    if (distSq > radius * radius)
        return false;

    // Impacto válido
    hitDist = t;
    return true;
}