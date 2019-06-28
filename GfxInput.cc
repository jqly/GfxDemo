#include "GfxInput.h"

#ifndef _ANDROID_

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#endif

namespace gfx
{

#ifdef GLFW_OPENGL_API


void fill_glfw_keyboard_input(GLFWwindow* ctx, KeyboardInput* keyboard)
{
    keyboard->Esc = (glfwGetKey(ctx, GLFW_KEY_ESCAPE) == GLFW_PRESS);
	keyboard->A = (glfwGetKey(ctx, GLFW_KEY_A) == GLFW_PRESS);
	keyboard->W = (glfwGetKey(ctx, GLFW_KEY_W) == GLFW_PRESS);
	keyboard->S = (glfwGetKey(ctx, GLFW_KEY_S) == GLFW_PRESS);
	keyboard->D = (glfwGetKey(ctx, GLFW_KEY_D) == GLFW_PRESS);
	keyboard->H = (glfwGetKey(ctx, GLFW_KEY_H) == GLFW_PRESS);
	keyboard->J = (glfwGetKey(ctx, GLFW_KEY_J) == GLFW_PRESS);
	keyboard->K = (glfwGetKey(ctx, GLFW_KEY_K) == GLFW_PRESS);
	keyboard->L = (glfwGetKey(ctx, GLFW_KEY_L) == GLFW_PRESS);
	keyboard->N = (glfwGetKey(ctx, GLFW_KEY_N) == GLFW_PRESS);
	keyboard->M = (glfwGetKey(ctx, GLFW_KEY_M) == GLFW_PRESS);
}

////
// Mouse scroll in glfw needs a callback.
////

namespace
{

static float scroll_callback_mouse_scroll_x = 0;
static float scroll_callback_mouse_scroll_y = 0;

void scroll_callback(GLFWwindow* ctx, double xoffset, double yoffset)
{
	scroll_callback_mouse_scroll_x = static_cast<float>(xoffset);
    scroll_callback_mouse_scroll_y = static_cast<float>(yoffset);
}

}

void set_mouse_scroll_callback(GLFWwindow* ctx)
{
    glfwSetScrollCallback(ctx, scroll_callback);
}

void fill_glfw_mouse_input(GLFWwindow* ctx, MouseInput* mouse)
{
    mouse->scroll.x = scroll_callback_mouse_scroll_x;
    mouse->scroll.y = scroll_callback_mouse_scroll_y;
	scroll_callback_mouse_scroll_x = 0;
	scroll_callback_mouse_scroll_y = 0;
    int left_mouse_state = glfwGetMouseButton(ctx, GLFW_MOUSE_BUTTON_LEFT);
    mouse->left_button_press = (left_mouse_state == GLFW_PRESS);
    int right_mouse_state = glfwGetMouseButton(ctx, GLFW_MOUSE_BUTTON_RIGHT);
    mouse->right_button_press = (right_mouse_state == GLFW_PRESS);

    double xpos, ypos;
	glfwGetCursorPos(ctx, &xpos, &ypos);
	mouse->cursor.x = static_cast<float>(xpos);
    mouse->cursor.y = static_cast<float>(ypos);
}

void fill_glfw_window_input(GLFWwindow* ctx, WindowInput* window)
{
	glfwGetWindowSize(ctx, &window->width, &window->height);
}

void init_input_with_glfw(GLFWwindow* ctx)
{
	set_mouse_scroll_callback(ctx);
}

void fill_input_with_glfw(GLFWwindow* ctx, Input* input)
{
	fill_glfw_keyboard_input(ctx, &input->keyboard);
	fill_glfw_mouse_input(ctx, &input->mouse);
	fill_glfw_window_input(ctx, &input->window);
}

#else

#endif

}