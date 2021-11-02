#pragma once

#include "../../Engine/Commons.h"

#define MAX_BIT_BUFFER_SIZE 32

/// <summary>
/// A buffer that packs up to 8 boolean values into one byte.
/// </summary>
/// <typeparam name="T">Template pattern.</typeparam>
template<typename T>
class BitBuffer
{
public:
	T data = 0;
	
	BitBuffer() {}

	inline bool Get(int index) const
	{
		return (data >> index) & 1;
	}

	inline void Set(int index, bool value)
	{
		if (value) data |= (1 << index);
		else data &= ~(1 << index);
	}
private:
};

typedef BitBuffer<uint8_t> BitBuffer8;
typedef BitBuffer<uint16_t> BitBuffer16;
typedef BitBuffer<uint32_t> BitBuffer32;

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint8_t>, data)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint16_t>, data)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint32_t>, data)
}
