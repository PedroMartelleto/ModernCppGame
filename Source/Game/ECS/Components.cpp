#include "Components.h"
#include "../GameData.h"
#include "../Networking/Packet.h"

BitBuffer8 MobComponent::CreateEventBitBuffer() const
{
	BitBuffer8 buffer;
	buffer.Set(GameData::GetMobActionBit("MOVE_UP"), wantsToJump);
	buffer.Set(GameData::GetMobActionBit("MOVE_LEFT"), horizontalMoveDir < 0.0f);
	buffer.Set(GameData::GetMobActionBit("MOVE_RIGHT"), horizontalMoveDir > 0.0f);
	buffer.Set(GameData::GetMobActionBit("MOVE_DOWN"), horizontalMoveDir > 0.0f);
	buffer.Set(GameData::GetMobActionBit("SHOOT_DOWN"), readyToShoot);
	buffer.Set(GameData::GetMobActionBit("SHOOT_UP"), wantsToShoot);

	return buffer;
}