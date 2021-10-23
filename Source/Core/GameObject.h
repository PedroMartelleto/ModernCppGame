#pragma once

class GameObject
{
public:
	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual void Render() = 0;
	virtual void Update(float deltaTime) = 0;
};