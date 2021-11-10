#include "Components.h"
#include "../GameData.h"
#include "../Networking/Packet.h"

BitBuffer8 MobComponent::CreateEventBitBuffer() const
{
	BitBuffer8 buffer;
	buffer.Set(GameData::GetMobActionBit("JUMP"), wantsToJump);
	buffer.Set(GameData::GetMobActionBit("MOVE_LEFT"), horizontalMoveDir < 0.0f);
	buffer.Set(GameData::GetMobActionBit("MOVE_RIGHT"), horizontalMoveDir > 0.0f);
	buffer.Set(GameData::GetMobActionBit("SHOOT_LEFT"), shootDirection.left);
	buffer.Set(GameData::GetMobActionBit("SHOOT_UP"), shootDirection.up);
	buffer.Set(GameData::GetMobActionBit("SHOOT_RIGHT"), shootDirection.right);
	buffer.Set(GameData::GetMobActionBit("SHOOT_DOWN"), shootDirection.down);
	buffer.Set(GameData::GetMobActionBit("SHOOT"), wantsToShoot);

	return buffer;
}

std::string ProjectileDirection::ToString() const
{
	auto vec = AsVector();
	std::stringstream stream;

	if (vec.y > 0.0f)
	{
		stream << "U";
	}
	else if (vec.y < 0.0f)
	{
		stream << "D";
	}

	if (vec.x > 0.0f)
	{
		stream << "R";
	}
	else if (vec.x < 0.0f)
	{
		stream << "L";
	}

	return stream.str();
}

bool ProjectileDirection::IsNone() const
{
	return !up && !down && !left && !right;
}

Vec2f ProjectileDirection::AsVector() const
{
	float yDir = up ? -1.0f : 0.0f;
	if (down) yDir += 1.0f;

	float xDir = right ? 1.0f : 0.0f;
	if (left) xDir -= 1.0f;

	return Vec2f(xDir, yDir);
}

float ProjectileDirection::AsAngle() const
{
	auto dir = AsVector();
	return atan2f(dir.y, dir.x);
}