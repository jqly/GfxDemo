
#ifndef GFX_INPUT_H
#define GFX_INPUT_H

#include "calc.h"

#ifndef _ANDROID_

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#endif

namespace gfx
{

class KeyboardInput {
public:
    bool Esc;
    bool A;
    bool W;
    bool S;
    bool D;
    bool H;
    bool J;
    bool K;
    bool L;
    bool N;
    bool M;
};

class MouseInput {
public:
    bool left_button_press;
    bool right_button_press;
    calc::Vec2 cursor;
    calc::Vec2 scroll;
};

class WindowInput {
public:
    int width;
    int height;
};

class Input {
public:
    KeyboardInput keyboard;
    MouseInput mouse;
    WindowInput window;
};

#ifdef GLFW_OPENGL_API

void init_input_with_glfw(GLFWwindow* ctx);
void fill_input_with_glfw(GLFWwindow* ctx, Input* input);

#else
    static_assert(false, "No GLFW, no input.");
#endif

}


#endif