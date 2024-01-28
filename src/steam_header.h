#pragma once
#include <cstdint>

#pragma pack( push, 4 )
struct SteamParamStringArray_t
{
	const char ** m_ppStrings;
	uint32_t m_nNumStrings;
};
#pragma pack( pop )