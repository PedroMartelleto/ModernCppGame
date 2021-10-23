#pragma once
#include "GameObject.h"

class Mob;
class Core;

class GameLoop : public GameObject
{
public:
	GameLoop(Core* core);

	void Create() override;
	void Update(float deltaTime) override;
	void Render() override;
	void Destroy() override;
private:
	Core* m_core;
	Mob* m_player;
};

