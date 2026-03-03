#include "core/GameContext.h"
#include <GL/glut.h>
#include "core/window.h"

// Atualiza o centro da janela (usado para controle do mouse)
void atualizaCentroJanela(GameContext& ctx, int w, int h)
{
    ctx.centerX = w / 2;
    ctx.centerY = h / 2;
}

// -------------------------------------------------------
// Controle de tela cheia
// -------------------------------------------------------

// UPDATE: atualiza apenas o estado interno
void toggleFullScreenState(GameContext& ctx) {
    ctx.ignoreWarp = true;
    ctx.firstMouse = true;
    ctx.fullScreen = !ctx.fullScreen;
}

// RENDER / UI: aplica a mudança visual
void applyFullScreenMode(GameContext& ctx) {
    if (ctx.fullScreen)
    {
        ctx.windowedW = ctx.janelaW; // ← salva antes de ir pro fullscreen
        ctx.windowedH = ctx.janelaH;
        glutFullScreen();
    }
    else
    {
        glutReshapeWindow(ctx.windowedW, ctx.windowedH); // ← restaura o tamanho original
    }
}

// Interface pública: alterna tela cheia
void altFullScreen(GameContext& ctx) {
    toggleFullScreenState(ctx);
    applyFullScreenMode(ctx);
}