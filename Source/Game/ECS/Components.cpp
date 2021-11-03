#include "Components.h"
#include "../GameData.h"
#include "../Networking/Packet.h"

uint16_t MobComponent::globalMobID = 0;

NetworkBuffer MobComponent::CreateEventPacket() const
{
	BitBuffer8 buffer;
	buffer.Set(GameData::GetMobActionBit("JUMP"), wantsToJump);
	buffer.Set(GameData::GetMobActionBit("MOVE_LEFT"), horizontalMoveDir < 0.0f);
	buffer.Set(GameData::GetMobActionBit("MOVE_RIGHT"), horizontalMoveDir > 0.0f);

	NetworkBuffer net = { (uint8_t) PacketType::MobEventBuffer, buffer.bits };
	return net;
}