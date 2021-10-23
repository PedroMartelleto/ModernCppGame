#pragma once
#include "../GameEntity.h"

class Mob : public GameEntity
{
public:
	Mob(Core* core);

	virtual void Create() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Destroy() override;
};

