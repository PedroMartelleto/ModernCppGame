#pragma once

#include "../Commons.h"
#include "../Render/Window.h"
#include "../Render/ResourceManager.h"

class GameCore;
class TileMap;
class TextureAtlas;

enum class HostType : uint8_t
{
	NONE, CLIENT, SERVER
};

class Core
{
public:
	Core(int targetFPS, HostType hostType);

	void Create();
	void Destroy();
	void Render();
	void Update(float deltaTime);
	
	void Run();

	void SetWindowSizeAndCenter(int width, int height);

	float GetFrameTime() const;
public:
	GameCore* gameCore;
	Ref<ResourceManager> resourceManager;
private:
	float m_frameTime;
	bool m_isRunning;
	Window* m_window;
	HostType m_hostType;
};

