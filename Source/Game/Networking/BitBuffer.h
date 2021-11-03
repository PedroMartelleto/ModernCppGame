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
	T bits = 0;
	
	BitBuffer() {}
	BitBuffer(T bits) : bits(bits) {}

	inline bool Get(int index) const
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

		return std::string(stream.data());
	}
private:
};

typedef BitBuffer<uint8_t> BitBuffer8;
typedef BitBuffer<uint16_t> BitBuffer16;
typedef BitBuffer<uint32_t> BitBuffer32;

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint8_t>, bits)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint16_t>, bits)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BitBuffer<uint32_t>, bits)
}

namespace bitsery
{
	template <typename S>
	void serialize(S& s, BitBuffer8& bitBuffer) { s.value1b(bitBuffer.bits); }

	template <typename S>
	void serialize(S& s, BitBuffer16& bitBuffer) { s.value2b(bitBuffer.bits); }

	template <typename S>
	void serialize(S& s, BitBuffer32& bitBuffer) { s.value4b(bitBuffer.bits); }
}
