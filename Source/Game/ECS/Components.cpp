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