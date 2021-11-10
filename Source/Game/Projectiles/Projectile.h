#pragma once

#include "../../Engine/Engine.h"

struct ProjectileDirection
{
	bool up;
	bool right;
	bool down;
	bool left;

	bool IsNone() const;
	Vec2f AsVector() const;
	float AsAngle() const;
	std::string ToString() const;
};