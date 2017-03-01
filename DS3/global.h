#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <vector>
#include <random>
#include "module.h"
#include "field.h"
#include "../Inih/cpp/INIREADER.h"

extern default_random_engine *gen;
extern double eps;
extern char DumpPattern[64];

struct Layer
{
	double left, right, dc;
};

FILE *GetFile(const char *name);