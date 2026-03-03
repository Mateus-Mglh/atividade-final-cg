#include "utils/assets.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"

void loadAssets(GameContext& ctx)
{
    ctx.enemyTexture        = loadTexture("assets/sprites/mapinguari.png");
    ctx.enemyStunnedTexture = loadTexture("assets/sprites/mapinguari_atordoado.png");
    ctx.botoTexture         = loadTexture("assets/sprites/boto.png");
    ctx.botinhoTexture      = loadTexture("assets/sprites/botinho.png");
    ctx.texGunDrop          = loadTexture("assets/sprites/gun.png");
    ctx.stoneTexture        = loadTexture("assets/texturas/textura_preda.png");
    ctx.rockTexture         = loadTexture("assets/texturas/rocha.png");
    ctx.logTexture          = loadTexture("assets/texturas/tronco.png");
    ctx.treeTexture         = loadTexture("assets/texturas/arbusto.png");
    ctx.floorTexture        = loadTexture("assets/texturas/chao.png");
    ctx.waterTexture        = loadRepeatingTexture("assets/texturas/tex_water_seamless.png");
    ctx.texSkydome          = loadTexture("assets/texturas/ceu.png");
    ctx.texSkydome3         = loadTexture("assets/texturas/ceu_dia.png");
    ctx.amuletTexture       = loadTexture("assets/texturas/amuleto.png");
    ctx.texWall             = loadTexture("assets/texturas/parede.png");
    ctx.texCeiling          = loadTexture("assets/texturas/telhado.png");
    ctx.texGunDefault       = loadTexture("assets/sprites/gun_default.png");
    ctx.texGunFire1         = loadTexture("assets/sprites/gun_fire1.png");
    ctx.texGunFire2         = loadTexture("assets/sprites/gun_fire2.png");
    ctx.texGunReload1       = loadTexture("assets/sprites/gun_reload1.png");
    ctx.texGunReload2       = loadTexture("assets/sprites/gun_reload2.png");
    ctx.texSnake            = loadTexture("assets/sprites/cobra.png");
    ctx.texDumbbell         = loadTexture("assets/sprites/peso.png");
    ctx.texHealth           = loadTexture("assets/texturas/health.png");
    ctx.texAmmo             = loadTexture("assets/texturas/municao.png");
    ctx.texHealthOverlay    = loadTexture("assets/texturas/heal.png");
    ctx.texCanoa            = loadTexture("assets/texturas/canoa.png");
    ctx.texHeart            = loadTexture("assets/sprites/heart.png");
    
    printf("Carregando shaders...\n");
    ctx.progLighting = createShader("assets/shaders/lighting.vert", "assets/shaders/lighting.frag");
    printf("Shaders carregados!\n");
    ctx.sphereQuadric       = gluNewQuadric();
    gluQuadricTexture(ctx.sphereQuadric, GL_TRUE);
    gluQuadricNormals(ctx.sphereQuadric, GLU_SMOOTH);
}