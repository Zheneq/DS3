#pragma once

class Configurer
{
	// TODO: private?
public:
	INIReader *base, *over;

public:
	Configurer(const char *baseinifile, const char *overrideinifile) : base(nullptr), over(nullptr)
	{
		if (!baseinifile) throw "Configurer: No INI file.";
		base = new INIReader(baseinifile);

		if (overrideinifile)
		{
			over = new INIReader(overrideinifile);
		}

		if (base->ParseError() < 0 || over && over->ParseError() < 0)
		{
			throw("Configurer: Can't load ini file");
		}
	}

	~Configurer()
	{
		if (base) delete base;
		if (over) delete over;
	}

	int GetInteger(const char* name, int def)
	{
		if (base) def = base->GetInteger("Data", name, def);
		if (over) def = over->GetInteger("Data", name, def);
		return def;
	}

	double GetReal(const char* name, double def)
	{
		if (base) def = base->GetReal("Data", name, def);
		if (over) def = over->GetReal("Data", name, def);
		return def;
	}
};