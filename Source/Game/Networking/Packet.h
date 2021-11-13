#pragma once

#include "BitBuffer.h"
#include "../../Engine/Commons.h"

/// <summary>
/// Used in packets and externally to handle network-related events.
/// </summary>
enum class EventType : NetworkByte
{
    Zero,
    
    // Client or server
    MobInputs,
    WorldSnapshot,

    // Server only
    SpawnPlayers,
    Map,
    
    Max
};

struct PacketData
{
    std::vector<json> events;
    std::vector<EventType> types;
};

namespace Serialization
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PacketData, events, types)
}