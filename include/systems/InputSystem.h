#pragma once
struct GameContext;

class InputSystem
{
public:
    static void init(GameContext& ctx);

    static void keyboard(GameContext& ctx, unsigned char key, int x, int y);
    static void keyboardUp(GameContext& ctx, unsigned char key, int x, int y);
    static void mouseClick(GameContext& ctx, int button, int state, int x, int y);
    static void handleMouseMotion(GameContext& ctx, int x, int y);
    static void specialKey(GameContext& ctx, int key, int x, int y);
    static void specialKeyCallback(int key, int x, int y);

    // Callbacks GLUT
    static void keyboardCallback(unsigned char key, int x, int y);
    static void keyboardUpCallback(unsigned char key, int x, int y);
    static void mouseClickCallback(int button, int state, int x, int y);
    static void mouseMotionCallback(int x, int y);
};