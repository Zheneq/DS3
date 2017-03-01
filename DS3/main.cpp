
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <ctime>
#include "global.h"

using namespace std;
default_random_engine *gen = NULL;
double eps = 1e-12; // Точность
char DumpPattern[64] = "%.12 %.12";


int main(int argc, char **argv)
{
	gen = new default_random_engine((unsigned int)time(0));


	// char DumpPatternPattern[] = "%%.%df, %%.%df\n";
	// int dig = (int)ceil(log(1 / (hs*hs)) / log(10)) + 4;
	// eps = pow(.1, dig) / 2 - 1e-10;
	// sprintf(DumpPattern, DumpPatternPattern, dig, dig);


	expCount = baseconfig.GetInteger("Data", "ExperimentCount", -1);
	expCount = overconfig.GetInteger("Data", "ExperimentCount", expCount);



	for (auto m : modules)
		m->Average();

	return 0;
}