#include <GL/glew.h>
#include <GL/glut.h>

#include "core/GameContext.h"
#include "core/app.h"
#include "core/Config.h"

GameContext ctx;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(GameConfig::WINDOW_W, GameConfig::WINDOW_H);
    glutCreateWindow(GameConfig::WINDOW_TITLE);

    if (glewInit() != GLEW_OK)
        return 1;

    initGameContext(ctx);

    appInit(ctx);

    glutMainLoop();
    return 0;
}