#pragma once

#include "pcg_random.hpp"
#include "Defs.h"

#include <random>
#include <sstream>

class UIDGen
{
public:

	//Historn: Maybe try adding hexadecimal UUID
	static uint32 GenerateUID()
	{
		// Seed with a real random value, if available
		pcg_extras::seed_seq_from<std::random_device> seed_source;

		// Make a random number engine
		pcg32 rng(seed_source);

		uint32 uid = -1;

		std::uniform_int_distribution<int> uniform_dist(100000000, 999999999);

		uid = uniform_dist(rng);

		return uid;
	}

};