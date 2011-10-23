#include "Common.hpp"

enum class CryptoType
{
	SHA256 = 0
};

namespace Crypto
{
	// Encrypts input array
	// Output is Out and what we return
	const u8* Encrypt(CryptoType Type, char* In, u32 Size, u8* Out);
}
