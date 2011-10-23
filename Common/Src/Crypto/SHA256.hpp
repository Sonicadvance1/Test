#include "Common.hpp"

namespace SHA256
{
	void Start();
	void Update(char *In, u32 Size);
	void Finish(u8 *Out);
}
