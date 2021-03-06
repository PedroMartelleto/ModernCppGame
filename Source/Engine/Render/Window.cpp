#include "Window.h"
#include "Render2D.h"
#include <SDL2/SDL.h>

Window::Window(int width, int height, const std::string & title) :
	m_width(width),
	m_height(height),
	m_title(title),
	m_isCloseRequested(false)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	m_glContext = SDL_GL_CreateContext(m_window);
	if (m_glContext == nullptr)
	{
		fprintf(stderr, "Failed to create GL context.");
	}

	SDL_GL_SetSwapInterval(1);

	glewExperimental = GL_TRUE;

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
	}
}

Window::~Window()
{
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Window::Update()
{
	for (int i = 0; i < Input::NUM_MOUSEBUTTONS; i++)
	{
		Input::SetMouseDown(i, false);
		Input::SetMouseUp(i, false);
	}

	for (int i = 0; i < Input::NUM_KEYS; i++)
	{
		Input::SetKeyDown(i, false);
		Input::SetKeyUp(i, false);
	}

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			m_isCloseRequested = true;
		}

		if (e.type == SDL_MOUSEMOTION)
		{
			Input::SetMouseX(e.motion.x);
			Input::SetMouseY(e.motion.y);
		}

		if (e.type == SDL_KEYDOWN)
		{
			int value = e.key.keysym.scancode;

			Input::SetKey(value, true);
			Input::SetKeyDown(value, true);
		}
		if (e.type == SDL_KEYUP)
		{
			int value = e.key.keysym.scancode;

			Input::SetKey(value, false);
			Input::SetKeyUp(value, true);
		}
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			int value = e.button.button;

			Input::SetMouse(value, true);
			Input::SetMouseDown(value, true);
		}
		if (e.type == SDL_MOUSEBUTTONUP)
		{
			int value = e.button.button;

			Input::SetMouse(value, false);
			Input::SetMouseUp(value, true);
		}
	}
}

void Window::SetWindowSizeAndCenter(int width, int height)
{
	SDL_SetWindowSize(m_window, width, height);
	Render2D::Resize((float)width, (float)height);

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	SDL_SetWindowPosition(m_window, (displayMode.w - width) / 2, (displayMode.h - height) / 2);
	
	m_width = width;
	m_height = height;
}

void Window::SwapBuffers()
{
	SDL_GL_SwapWindow(m_window);
}

void Window::BindAsRenderTarget() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#if PROFILING_SET_1x1_VIEWPORT == 0
	glViewport(0, 0, GetWidth(), GetHeight());
#else
	glViewport(0, 0, 1, 1);
#endif
}

void Window::SetFullScreen(bool value)
{
	int mode = 0;
	if (value)
		mode = SDL_WINDOW_FULLSCREEN;
	else
		mode = 0;

	SDL_SetWindowFullscreen(m_window, mode);
}