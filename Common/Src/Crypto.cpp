#include <stdio.h>

#include "Crypto.hpp"
#include "Crypto/SHA256.hpp"

namespace Crypto
{
	const u8* Encrypt(CryptoType Type, char* In, u32 Size, u8* Out)
	{
		switch(Type)
		{
			case CryptoType::SHA256:
			{
				SHA256::Start();
				SHA256::Update(In, Size);
				SHA256::Finish(Out);
				return Out;
			}
			break;
			default:
				printf("We don't support Encrypting that type!\n");
			break;
		}
	}
}
