#include "module.h"

list<DataMapping> GenInputList()
{
	list<DataMapping> res;

	DataMapping Map[] =
	{
		{ "DumpPath", "%s", info.DumpPath, NULL },
		{ "FramesPerFile", "%d", &info.FramesPerFile, NULL },
		{ "FrameStep", "%d", &info.FrameStep, NULL },

		{ "ExperimentCount", "%d", &info.ExperimentCount, NULL },
		{ "ExperimentDumpAll", "%d", &info.ExperimentDumpAll, "0" },
		{ "ExperimentStep", "%d", &info.ExperimentStep, "1" }
	};

	for each(auto& m in Map)
	{
		res.push_back(m);
	}

	return res;
}