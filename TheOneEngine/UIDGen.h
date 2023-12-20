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
		/* Historn: MAYBE THE ONE NOW IT DOESN'T WORK SO I LET HERE THE OTHER WAY */

		//// Seed with a real random value, if available
		//pcg_extras::seed_seq_from<std::random_device> seed_source;

		//// Make a random number engine
		//pcg32 rng(seed_source);

		//uint32 uid = -1;

		//std::uniform_int_distribution<int> uniform_dist(100000000, 999999999);

		//uid = uniform_dist(rng);

		pcg32 rng(std::random_device{}());

		return rng();
	}

};