#include "graphics/Hud.h"
#include <GL/glut.h>
#include <cstdlib>

void drawCrosshair(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST); // importante

    glColor3f(1, 1, 1);

    int cx = w / 2;
    int cy = h / 2;

    glBegin(GL_LINES);
    glVertex2f(cx - 10, cy);
    glVertex2f(cx + 10, cy);
    glVertex2f(cx, cy - 10);
    glVertex2f(cx, cy + 10);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

void drawPlayerHUD(GameContext& ctx)
{
    if (ctx.p3state != Phase3State::BOSS_FIGHT && ctx.p3state != Phase3State::FADE_IN)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ── Barra de vida vertical ────────────────
    float barX    = 20.0f;
    float barY    = 20.0f;
    float barW    = 18.0f;
    float barH    = 160.0f;
    float hpRatio = std::max(0.0f, std::min(1.0f, ctx.player.getHealth() / 100.0f));
    float fillH   = barH * hpRatio;

    // Fundo da barra
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.1f, 0.1f, 0.1f, 0.75f);
    glBegin(GL_QUADS);
        glVertex2f(barX,        barY);
        glVertex2f(barX + barW, barY);
        glVertex2f(barX + barW, barY + barH);
        glVertex2f(barX,        barY + barH);
    glEnd();

    // Preenchimento verde
    glColor4f(0.1f, 0.85f, 0.2f, 0.90f);
    glBegin(GL_QUADS);
        glVertex2f(barX,        barY);
        glVertex2f(barX + barW, barY);
        glVertex2f(barX + barW, barY + fillH);
        glVertex2f(barX,        barY + fillH);
    glEnd();

    // Borda da barra
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(barX,        barY);
        glVertex2f(barX + barW, barY);
        glVertex2f(barX + barW, barY + barH);
        glVertex2f(barX,        barY + barH);
    glEnd();

    // Texto HP acima da barra
    char bufHP[16];
    sprintf(bufHP, "%d", ctx.player.getHealth());
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glRasterPos2f(barX + 2.0f, barY + barH + 8.0f);
    for (char* c = bufHP; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawWeaponHUD(GameContext& ctx)
{
    if (ctx.currentState != GameState::PHASE3 || ctx.p3state != Phase3State::BOSS_FIGHT
        || !ctx.player.hasWeapon() || !ctx.player.isWeaponVisible())
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ── Sprite da arma (centro-baixo) ─────────
    GLuint tex = ctx.texGunDefault;
    auto state = ctx.player.getWeaponState();
    if (state == Player::W_FIRE_1 || state == Player::W_RETURN)
        tex = ctx.texGunFire1;
    else if (state == Player::W_FIRE_2 || state == Player::W_PUMP)
        tex = ctx.texGunFire2;
    else if (state == Player::W_RELOAD_1 || state == Player::W_RELOAD_3)
        tex = ctx.texGunReload1;
    else if (state == Player::W_RELOAD_2)
        tex = ctx.texGunReload2;

    glBindTexture(GL_TEXTURE_2D, tex);
    glColor4f(1, 1, 1, 1);
    float gunH = ctx.janelaH * 0.5f;
    float gunW = gunH;
    float gx   = (ctx.janelaW - gunW) / 2.0f;
    float gy   = 0.0f;
    if (ctx.player.weaponFiring) { gy -= 20.0f; gx += (rand() % 10 - 5); }
    glBegin(GL_QUADS);
        glTexCoord2f(0,1); glVertex2f(gx,       gy);
        glTexCoord2f(1,1); glVertex2f(gx + gunW, gy);
        glTexCoord2f(1,0); glVertex2f(gx + gunW, gy + gunH);
        glTexCoord2f(0,0); glVertex2f(gx,        gy + gunH);
    glEnd();

    // ── Painel de munição (canto inferior direito) ──
    float panelW  = 160.0f;
    float panelH  = 60.0f;
    float panelX  = ctx.janelaW - panelW - 15.0f;
    float panelY  = 15.0f;
    float iconSize = 48.0f;

    // Fundo semitransparente
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(panelX,          panelY);
        glVertex2f(panelX + panelW, panelY);
        glVertex2f(panelX + panelW, panelY + panelH);
        glVertex2f(panelX,          panelY + panelH);
    glEnd();

    // Ícone da arma (texGunDrop) à esquerda do painel
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ctx.texGunDrop);
    glColor4f(1, 1, 1, 0.9f);
    float iconX = panelX + 6.0f;
    float iconY = panelY + (panelH - iconSize) * 0.5f;
    glBegin(GL_QUADS);
        glTexCoord2f(0,1); glVertex2f(iconX,            iconY);
        glTexCoord2f(1,1); glVertex2f(iconX + iconSize,  iconY);
        glTexCoord2f(1,0); glVertex2f(iconX + iconSize,  iconY + iconSize);
        glTexCoord2f(0,0); glVertex2f(iconX,             iconY + iconSize);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Texto de munição
    char buf[32];
    if (ctx.player.getIsReloading())
        sprintf(buf, "Recarregando...");
    else
        sprintf(buf, "%d / %d", ctx.player.getCurrentAmmo(), ctx.player.getReserveAmmo());

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glRasterPos2f(iconX + iconSize + 8.0f, panelY + panelH * 0.5f - 4.0f);
    for (char* c = buf; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


void drawBossHUD(GameContext& ctx)
{
    if (!ctx.boto.alive)
        return;

    // Parâmetros da barra
    float barW = 400.0f;                   // largura total
    float barH = 25.0f;                    // altura
    float barX = (ctx.janelaW - barW) / 2; // centralizada
    float barY = ctx.janelaH - 40.0f;      // no topo

    // Percentual da vida
    float pct = ctx.boto.health / 100.0f;
    if (pct < 0.0f) pct = 0.0f;

    // Cor da barra: verde -> amarelo -> vermelho
    float r, g;
    if (pct > 0.5f) {
        // verde -> amarelo
        r = (1.0f - pct) * 2.0f;    // r = 0 quando pct=1, r =1 quando pct=0.5
        g = 1.0f;
    } else {
        // amarelo -> vermelho
        r = 1.0f;
        g = pct * 2.0f;             // g = 1 quando pct=0.5, g=0 quando pct=0
    }

    // Fundo da barra (cinza escuro)
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(barX,          barY);
        glVertex2f(barX + barW,   barY);
        glVertex2f(barX + barW,   barY + barH);
        glVertex2f(barX,          barY + barH);
    glEnd();

    // Barra da vida
    glColor3f(r, g, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(barX,            barY);
        glVertex2f(barX + barW * pct, barY);
        glVertex2f(barX + barW * pct, barY + barH);
        glVertex2f(barX,            barY + barH);
    glEnd();

    // Texto do nome do boss
    const char* bossName = "Boto";
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(barX + (barW/2 - 20), barY + barH + 5);
    for (const char* c = bossName; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
}

// void drawPlayerHUD(GameContext& ctx)
// {
//     if (ctx.p3state != Phase3State::BOSS_FIGHT && ctx.p3state != Phase3State::FADE_IN)
//         return;

//     char buf[64];
//     sprintf(buf, "HP: %d", ctx.player.getHealth());
//     glColor3f(1.0f, 1.0f, 1.0f);
//     glRasterPos2f(10.0f, 30.0f);
//     for (char* c = buf; *c != '\0'; c++)
//         glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
// }

static void drawText(const char* text, float x, float y, void* font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

static int textWidth(const char* text, void* font = GLUT_BITMAP_HELVETICA_18)
{
    int w = 0;
    for (const char* c = text; *c != '\0'; c++)
        w += glutBitmapWidth(font, *c);
    return w;
}

static void drawBox(float x, float y, float w, float h, bool selected)
{
    // Fundo
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (selected)
        glColor4f(0.8f, 0.6f, 0.1f, 0.35f);
    else
        glColor4f(0.0f, 0.0f, 0.0f, 0.45f);
    glBegin(GL_QUADS);
        glVertex2f(x,   y);
        glVertex2f(x+w, y);
        glVertex2f(x+w, y+h);
        glVertex2f(x,   y+h);
    glEnd();
    // Borda
    if (selected)
        glColor3f(0.9f, 0.7f, 0.2f);
    else
        glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(selected ? 2.0f : 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x,   y);
        glVertex2f(x+w, y);
        glVertex2f(x+w, y+h);
        glVertex2f(x,   y+h);
    glEnd();
    glDisable(GL_BLEND);
}

void renderMenu(GameContext& ctx)
{
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Fundo gradiente
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
        glColor3f(0.05f, 0.02f, 0.08f); glVertex2f(0,           0);
        glColor3f(0.05f, 0.02f, 0.08f); glVertex2f(ctx.janelaW, 0);
        glColor3f(0.10f, 0.05f, 0.15f); glVertex2f(ctx.janelaW, ctx.janelaH);
        glColor3f(0.10f, 0.05f, 0.15f); glVertex2f(0,           ctx.janelaH);
    glEnd();
    glDisable(GL_BLEND);

    float cx = ctx.janelaW / 2.0f;
    float cy = ctx.janelaH / 2.0f;

    // ── Tela de Créditos ──────────────────────
    if (ctx.menuShowCredits)
    {
        glColor3f(0.9f, 0.7f, 0.2f);
        const char* title = "CREDITOS";
        drawText(title, cx - textWidth(title, GLUT_BITMAP_TIMES_ROMAN_24) / 2.0f,
                 cy + 80, GLUT_BITMAP_TIMES_ROMAN_24);

        glColor3f(0.85f, 0.85f, 0.85f);
        const char* lines[] = {
            "Desenvolvido por:",
            "Miguel Pereira, Eliandro Silva, Vinicius, Mateus Migalhas",
            "",
            "Engine: OpenGL + GLUT",
            "Audio: OpenAL",
            "",
            "Pressione ENTER para voltar"
        };
        float lineY = cy + 40;
        for (const char* line : lines)
        {
            drawText(line, cx - textWidth(line) / 2.0f, lineY);
            lineY -= 28;
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_FOG);
        return;
    }

    // ── Tela Como Jogar ───────────────────────
    if (ctx.menuShowHowToPlay)
    {
        // Título
        glColor3f(0.9f, 0.7f, 0.2f);
        const char* title = "COMO JOGAR";
        drawText(title, cx - textWidth(title, GLUT_BITMAP_TIMES_ROMAN_24) / 2.0f,
                cy + 140, GLUT_BITMAP_TIMES_ROMAN_24);

        // Seção controles
        glColor3f(0.6f, 0.8f, 1.0f);
        const char* secControles = "Controles";
        drawText(secControles, cx - textWidth(secControles) / 2.0f, cy + 100);

        glColor3f(0.85f, 0.85f, 0.85f);
        const char* controles[][2] = {
            { "WASD",       "Mover"                      },
            { "Mouse",      "Olhar"                       },
            { "Clique Esq", "Arremessar pedra / Atirar"   },
            { "R",          "Recarregar"                  },
            { "ESC",        "Pausar"                        },
        };
        float lineY = cy + 75;
        for (auto& c : controles) {
            drawText(c[0], cx - 160, lineY);
            drawText("-", cx - 60,  lineY);
            drawText(c[1], cx - 40, lineY);
            lineY -= 24;
        }

        // Seção fases
        glColor3f(0.6f, 0.8f, 1.0f);
        const char* secFases = "Objetivos";
        drawText(secFases, cx - textWidth(secFases) / 2.0f, lineY - 10);
        lineY -= 34;

        glColor3f(0.85f, 0.85f, 0.85f);
        const char* fases[] = {
            "Fase 1: Colete os 3 amuletos para selar o Mapinguari",
            "Fase 2: Navegue pela canoa desviando de obstaculos",
            "Fase 3: Derrote o Boto Rosa",
        };
        for (auto& f : fases) {
            drawText(f, cx - textWidth(f) / 2.0f, lineY);
            lineY -= 24;
        }

        // Voltar
        glColor3f(0.5f, 0.5f, 0.5f);
        const char* back = "Pressione ENTER para voltar";
        drawText(back, cx - textWidth(back) / 2.0f, 25.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_FOG);
        return;
    }

    // ── Menu Principal ────────────────────────
    // Título
    glColor3f(0.9f, 0.7f, 0.2f);
    const char* title = "LENDAS DA AMAZONIA";
    drawText(title, cx - textWidth(title, GLUT_BITMAP_TIMES_ROMAN_24) / 2.0f,
             cy + 130, GLUT_BITMAP_TIMES_ROMAN_24);

    // Subtítulo
    glColor3f(0.6f, 0.5f, 0.3f);
    const char* sub = "Uma jornada pelas lendas do Norte";
    drawText(sub, cx - textWidth(sub) / 2.0f, cy + 95);

    // Itens do menu
    const char* items[] = { "Iniciar", "Como Jogar", "Creditos", "Sair" };
    float boxW   = 220.0f;
    float boxH   = 38.0f;
    float boxX   = cx - boxW / 2.0f;
    float startY = cy + 50.0f;
    float gap    = 52.0f;

    // Detecta hover do mouse
    int mouseYFlipped = ctx.janelaH - ctx.mouseY; // OpenGL Y invertido
    for (int i = 0; i < 4; i++)
    {
        float by = startY - i * gap;
        if (ctx.mouseX >= boxX && ctx.mouseX <= boxX + boxW &&
            mouseYFlipped >= by && mouseYFlipped <= by + boxH)
        {
            ctx.menuSelectedItem = i;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        float by = startY - i * gap;
        bool selected = (ctx.menuSelectedItem == i);
        drawBox(boxX, by, boxW, boxH, selected);

        if (selected)
            glColor3f(1.0f, 0.85f, 0.3f);
        else
            glColor3f(0.85f, 0.85f, 0.85f);

        const char* label = items[i];
        float tx = cx - textWidth(label) / 2.0f;
        float ty = by + 12.0f;
        drawText(label, tx, ty);
    }

    // Instrução
    glColor3f(0.4f, 0.4f, 0.4f);
    const char* hint = "Setas: navegar   |   Enter: confirmar   |   Mouse: clicar";
    drawText(hint, cx - textWidth(hint) / 2.0f, 20.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_FOG);
}

void renderGameOver(GameContext& ctx)
{
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const char* text = "Voce foi derrotado";
    switch (ctx.gameOverReason)
    {
        case GameOverReason::PHASE1_CAUGHT: text = "O Mapinguari te pegou!";    break;
        case GameOverReason::PHASE2_FELL:   text = "Tu caiu do batelao!";       break;
        case GameOverReason::PHASE2_SNAKE:  text = "A Cobra sofia te alcançou!";    break;
        case GameOverReason::PHASE3_BOSS:   text = "O Boto BOTOu forte em tu";  break;
        default: break;
    }

    glColor3f(1.0f, 0.0f, 0.0f);
    int textWidth = 0;
    for (const char* c = text; *c != '\0'; c++)
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    glRasterPos2f((ctx.janelaW - textWidth) / 2.0f, ctx.janelaH / 2.0f);
    for (const char* c = text; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    const char* sub = "Pressione ENTER para tentar novamente";
    int subWidth = 0;
    for (const char* c = sub; *c != '\0'; c++)
        subWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, *c);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f((ctx.janelaW - subWidth) / 2.0f, ctx.janelaH / 2.0f - 30.0f);
    for (const char* c = sub; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_FOG);
}

void renderVictory(GameContext& ctx)
{
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Fundo azul escuro
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Título
    glColor3f(1.0f, 0.8f, 0.0f);
    const char* title = "VITORIA!";
    int titleWidth = 0;
    for (const char* c = title; *c; c++)
        titleWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);

    glRasterPos2f((ctx.janelaW - titleWidth) / 2.0f, ctx.janelaH / 2.0f + 40);
    for (const char* c = title; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    // Formatar tempo
    int total = (int)ctx.runTime;
    int minutos = total / 60;
    int segundos = total % 60;

    char tempoStr[64];
    sprintf(tempoStr, "Tempo total: %02d:%02d", minutos, segundos);

    glColor3f(1.0f, 1.0f, 1.0f);
    int tempoWidth = 0;
    for (const char* c = tempoStr; *c; c++)
        tempoWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);

    glRasterPos2f((ctx.janelaW - tempoWidth) / 2.0f, ctx.janelaH / 2.0f);
    for (const char* c = tempoStr; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    // Enter para voltar
    const char* msg = "Pressione ENTER para voltar ao menu";
    int msgWidth = 0;
    for (const char* c = msg; *c; c++)
        msgWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);

    glRasterPos2f((ctx.janelaW - msgWidth) / 2.0f, ctx.janelaH / 2.0f - 60);
    for (const char* c = msg; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_FOG);
}

void renderPause(GameContext& ctx)
{
    float cx = ctx.janelaW / 2.0f;
    float cy = ctx.janelaH / 2.0f;

    glUseProgram(0);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Overlay escuro semi-transparente
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glBegin(GL_QUADS);
        glVertex2f(0,           0);
        glVertex2f(ctx.janelaW, 0);
        glVertex2f(ctx.janelaW, ctx.janelaH);
        glVertex2f(0,           ctx.janelaH);
    glEnd();
    glDisable(GL_BLEND);

    // Título
    glColor3f(1.0f, 0.6f, 0.0f);
    const char* title = "PAUSADO";
    drawText(title, cx - textWidth(title, GLUT_BITMAP_TIMES_ROMAN_24) / 2.0f,
             cy + 80, GLUT_BITMAP_TIMES_ROMAN_24);

    // Itens
    const char* items[] = { "Continuar", "Voltar ao Menu" };
    float boxW   = 220.0f;
    float boxH   = 38.0f;
    float boxX   = cx - boxW / 2.0f;
    float startY = cy + 20.0f;
    float gap    = 52.0f;

    // Hover do mouse
    int mouseYFlipped = ctx.janelaH - ctx.mouseY;
    for (int i = 0; i < 2; i++)
    {
        float by = startY - i * gap;
        if (ctx.mouseX >= boxX && ctx.mouseX <= boxX + boxW &&
            mouseYFlipped >= by && mouseYFlipped <= by + boxH)
            ctx.pauseSelectedItem = i;
    }

    for (int i = 0; i < 2; i++)
    {
        float by = startY - i * gap;
        bool selected = (ctx.pauseSelectedItem == i);
        drawBox(boxX, by, boxW, boxH, selected);

        if (selected) glColor3f(1.0f, 0.9f, 0.0f);
        else          glColor3f(0.9f, 0.9f, 0.9f);

        const char* label = items[i];
        drawText(label, cx - textWidth(label) / 2.0f, by + 12.0f);
    }

    // Hint
    glColor3f(0.4f, 0.4f, 0.4f);
    const char* hint = "ESC para continuar";
    drawText(hint, cx - textWidth(hint) / 2.0f, 20.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_FOG);
}

void drawPhase1HUD(GameContext& ctx)
{
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Amuletos com sprite
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ctx.amuletTexture);
    glColor3f(1.0f, 1.0f, 1.0f);
    // desenha miniatura do amuleto (ex: 32x32px no canto)
    float iconSize = 32.0f;
    float iconX = 10.0f, iconY = ctx.janelaH - 60.0f;
    glBegin(GL_QUADS);
        glTexCoord2f(0,1); glVertex2f(iconX,            iconY);
        glTexCoord2f(1,1); glVertex2f(iconX + iconSize,  iconY);
        glTexCoord2f(1,0); glVertex2f(iconX + iconSize,  iconY + iconSize);
        glTexCoord2f(0,0); glVertex2f(iconX,             iconY + iconSize);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    // Texto amuletos ao lado do ícone
    char bufAmulets[32];
    sprintf(bufAmulets, "Amuletos: %d / 3", ctx.amuletsCollected);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(iconX + iconSize + 8, iconY + 10);
    for (char* c = bufAmulets; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

    // Pedras — ícones visuais (quadradinhos) representando cada pedra
    float stoneY = ctx.janelaH - 100.0f;
    float sz = 14.0f, gap = 4.0f;
    for (int i = 0; i < 10; i++)
    {
        if (i < ctx.playerStoneCount)
            glColor3f(0.7f, 0.5f, 0.2f);  // pedra cheia
        else
            glColor3f(0.3f, 0.3f, 0.3f);  // pedra vazia
        float px = 10.0f + i * (sz + gap);
        glBegin(GL_QUADS);
            glVertex2f(px,      stoneY);
            glVertex2f(px + sz, stoneY);
            glVertex2f(px + sz, stoneY + sz);
            glVertex2f(px,      stoneY + sz);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);  // ← garante estado limpo
    drawCrosshair(ctx.janelaW, ctx.janelaH);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

void drawPhase2HUD(GameContext& ctx)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ctx.janelaW, 0, ctx.janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Corações
    float iconSize = 32.0f;
    float gap      = 6.0f;
    float startX   = 10.0f;
    float heartY   = ctx.janelaH - 42.0f;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, ctx.texHeart);
    glColor3f(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < ctx.canoeHealth; i++)  // ← só desenha os que tem
    {
        float hx = startX + i * (iconSize + gap);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2f(hx,            heartY);
            glTexCoord2f(1,1); glVertex2f(hx + iconSize, heartY);
            glTexCoord2f(1,0); glVertex2f(hx + iconSize, heartY + iconSize);
            glTexCoord2f(0,0); glVertex2f(hx,            heartY + iconSize);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    // Tempo — barra de progresso + texto
    float barW    = 200.0f;
    float barH    = 14.0f;
    float barX    = 10.0f;
    float barY    = ctx.janelaH - 65.0f;
    float progress = std::min(ctx.phase2Timer / 60.0f, 1.0f);

    // Fundo da barra
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(barX,        barY);
        glVertex2f(barX + barW, barY);
        glVertex2f(barX + barW, barY + barH);
        glVertex2f(barX,        barY + barH);
    glEnd();

    // Preenchimento da barra (verde → amarelo → vermelho)
    float r = progress * 2.0f;
    float g = 2.0f - progress * 2.0f;
    glColor3f(std::min(r, 1.0f), std::min(g, 1.0f), 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(barX,                    barY);
        glVertex2f(barX + barW * progress,  barY);
        glVertex2f(barX + barW * progress,  barY + barH);
        glVertex2f(barX,                    barY + barH);
    glEnd();

    // Texto do tempo
    char bufTime[32];
    int  secs = (int)ctx.phase2Timer;
    sprintf(bufTime, "%02d:%02d / 01:00", secs / 60, secs % 60);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(barX + barW + 10, barY + 2);
    for (char* c = bufTime; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_FOG);
}

