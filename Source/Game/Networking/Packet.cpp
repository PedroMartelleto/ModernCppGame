#include "Packet.h"

namespace Packet
{
	void InsertType(PacketType type, NetworkBuffer& data)
	{
		data.insert(data.begin(), (uint8_t) type);
	}
}