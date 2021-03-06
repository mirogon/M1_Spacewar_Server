#pragma once
#include <time.h>
#include <stdlib.h>
namespace m1 {
	class C_Random {

	public:

		//Random class constructor
		C_Random();

		//Random class deconstructor
		virtual ~C_Random() = default;

		//Return a random number from min to max
		int randomNumber(int min, int max);

	};
}