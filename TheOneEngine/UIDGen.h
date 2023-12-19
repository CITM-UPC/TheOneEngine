#pragma once

#include "pcg_random.hpp"
#include "Defs.h"

#include <random>
#include <sstream>

using UUID32 = uint32_t;

class UIDGen
{
public:

	//Historn: Maybe try adding hexadecimal UUID
	static UUID32 GenerateUID()
	{
		pcg32 rng(std::random_device{}());

		return rng();
	}

};