#include "module_cacher.h"

void CacheModule::Init()
{
	if (!info->e_cache)
	{
		info->e_cache = new field();
		info->e_cache->Init(info->e->GetLen());

	}
	if (!info->h_cache)
	{
		info->h_cache = new field();
		info->h_cache->Init(info->h->GetLen());
	}

	//ZeroMemory(e_cache->data, sizeof(e_cache->data[0]) * e_cache->GetLen());
	//ZeroMemory(h_cache->data, sizeof(h_cache->data[0]) * h_cache->GetLen());
}

void CacheModule::PostCalc(int time)
{
	if (time < 0) return;

	for (int i = 0; i < info->e->GetLen(); ++i)
	{
		info->e_cache->data[i] = ((info->e_cache->data[i] * ExperimentNum) + info->e->data[i]) / (ExperimentNum + 1);
		info->h_cache->data[i] = ((info->h_cache->data[i] * ExperimentNum) + info->h->data[i]) / (ExperimentNum + 1);
	}
}