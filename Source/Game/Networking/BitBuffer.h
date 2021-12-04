#pragma once

#include "../../Engine/Commons.h"
#include <enet/enet.h>

typedef enet_uint8 NetworkByte;
using NetworkBuffer = std::vector<NetworkByte>;

#define MAX_BIT_BUFFER_SIZE 32

/// <summary>
/// A buffer that packs up to 8 boolean values into one byte.
/// </summary>
/// <typeparam name="T">Template pattern.</typeparam>
template<typename T>
class BitBuffer
{
public:
	T bits = 0;
	
	BitBuffer() {}
	BitBuffer(T bits) : bits(bits) {}

	inline bool GetBit(int index) const
	{
		return (bits >> index) & 1;
	}

	inline void Set(int index, bool value)
	{
		if (value)
		{
			bits |= (1 << index);
		}
		else
		{
			bits &= ~(1 << index);
		}
	}

	std::string ToString() const
	{
		std::array<char, sizeof(T)*8+1> stream;
		auto temp = bits;

		for (int i = 0; i < sizeof(T)*8; ++i)
		{
			stream[i] = temp % 2 ? '1' : '0';
			temp /= 2;
		}

		stream[sizeof(T)*8] = '\0';

		return std::string(stream.m_data());
	}
private:
};

typedef BitBuffer<NetworkByte> BitBuffer8;
typedef BitBuffer<int16_t> BitBuffer16;
typedef BitBuffer<int32_t> BitBuffer32;

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint8_t>, bits)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint16_t>, bits)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint32_t>, bits)
}