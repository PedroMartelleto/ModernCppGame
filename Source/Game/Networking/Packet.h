#pragma once

#include "BitBuffer.h"
#include "../../Engine/Commons.h"

enum class PacketType : uint8_t
{
    Zero,
    
    // Client or server
    MobEventBuffer,
    
    // Server only
    SpawnPlayer,
    Map,
    
    Max
};

namespace Packet
{
    void InsertType(PacketType type, NetworkBuffer& data);
}

namespace bitsery
{
    template <typename S>
    void serialize(S& s, Vec2f& v)
    {
        s.value4b(v.x);
        s.value4b(v.y);
    }
}