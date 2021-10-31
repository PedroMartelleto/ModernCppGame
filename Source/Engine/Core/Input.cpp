
#include "Input.h"
#include "../Render/window.h"
#include "SDL2/SDL.h"
#include <cstring>

bool Input::m_inputs[Input::NUM_KEYS] = { false };
bool Input::m_downKeys[Input::NUM_KEYS] = { false };
bool Input::m_upKeys[Input::NUM_KEYS] = { false };
bool Input::m_mouseInput[Input::NUM_MOUSEBUTTONS] = { false };
bool Input::m_downMouse[Input::NUM_MOUSEBUTTONS] = { false };
bool Input::m_upMouse[Input::NUM_MOUSEBUTTONS] = { false };
int  Input::m_mouseX = 0;
int  Input::m_mouseY = 0;
Window* Input::m_window = nullptr;

void Input::Create(Window* window)
{
	m_mouseX = 0;
	m_mouseY = 0;
	m_window = window;

	memset(m_inputs, 0, NUM_KEYS * sizeof(bool));
	memset(m_downKeys, 0, NUM_KEYS * sizeof(bool));
	memset(m_upKeys, 0, NUM_KEYS * sizeof(bool));

	memset(m_mouseInput, 0, NUM_MOUSEBUTTONS * sizeof(bool));
	memset(m_downMouse, 0, NUM_MOUSEBUTTONS * sizeof(bool));
	memset(m_upMouse, 0, NUM_MOUSEBUTTONS * sizeof(bool));
}

void Input::SetCursor(bool visible)
{
	if (visible)
		SDL_ShowCursor(1);
	else
		SDL_ShowCursor(0);
}

void Input::SetMousePosition(const Vec2f& pos)
{
	SDL_WarpMouseInWindow(m_window->GetSDLWindow(), (int)pos.x, (int)pos.y);
}