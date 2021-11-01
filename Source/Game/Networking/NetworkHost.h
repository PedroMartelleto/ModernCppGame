#pragma once

class NetworkHost
{
public:
	virtual void Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Update(float deltaTime) = 0;
};