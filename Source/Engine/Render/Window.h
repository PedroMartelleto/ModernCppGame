#pragma once

#include <SDL2/SDL.h>
#include <string>
#include "../Core/Math.h"
#include "../Core/Input.h"
#include "../Commons.h"

// Class that handles window management in SDL2. Code from https://github.com/BennyQBD/3DEngineCpp/blob/master/src/rendering/window.h.

class Window
{
public:
	Window(int width, int height, const std::string& title);
	virtual ~Window();

	void Update();
	void SwapBuffers();
	void BindAsRenderTarget() const;

	inline bool IsCloseRequested() const { return m_isCloseRequested; }
	inline int GetWidth() const { return m_width; }
	inline int GetHeight()  const { return m_height; }
	inline float GetAspect() const { return (float)m_width / (float)m_height; }
	inline const std::string& GetTitle() const { return m_title; }
	inline Vec2f GetCenter() const { return Vec2f((float)m_width / 2.0f, (float)m_height / 2.0f); }
	inline SDL_Window* GetSDLWindow() { return m_window; }

	void SetWindowSizeAndCenter(int width, int height);

	void SetFullScreen(bool value);
protected:
private:
	int m_width = 0;
	int m_height = 0;
	std::string m_title = "";
	SDL_Window* m_window = nullptr;
	SDL_GLContext m_glContext = nullptr;
	bool m_isCloseRequested = false;

	Window(const Window& other) {}
	void operator=(const Window& other) {}
};