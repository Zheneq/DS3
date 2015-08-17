
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <vector>
#include <random>
#include "../Inih/cpp/INIREADER.h"

using namespace std;

INIReader *config = NULL;

void Load(int argc, char **argv)
{
	const unsigned len = 512;
	char Path[len];

	// Если не задан файл конфигурации, ищем дефолтный
	if (argc == 1)
	{
		strcpy(Path, argv[0]); //strcpy_s(Path, strlen(argv[0]), argv[0]);
		for (int i = strlen(argv[0]); i; --i) if (Path[i] == '\\' || Path[i] == '/') { Path[i] = '\0'; break; }
		strcat(Path, "\\default.ini");
	}
	else
	{
		strcpy(Path, argv[1]);
	}
	printf("%s\n", Path);

	if (config) delete config;
	config = new INIReader(Path);

	if (config->ParseError() < 0) {
		throw("Can't load ini file");
	}
	
	//std::cout << "Config loaded from 'test.ini': version="
	//	<< reader.GetInteger("protocol", "version", -1) << ", name="
	//	<< reader.Get("user", "name", "UNKNOWN") << ", email="
	//	<< reader.Get("user", "email", "UNKNOWN") << ", pi="
	//	<< reader.GetReal("user", "pi", -1) << ", active="
	//	<< reader.GetBoolean("user", "active", true) << "\n";
}

void Init(int argc, char **argv)
{}

int main(int argc, char **argv)
{
	try
	{
		Load(argc, argv);
		Init(argc, argv);
	}
	catch (char *error)
	{
		cerr << error << endl;
		return 1;
	}

	return 0;
}