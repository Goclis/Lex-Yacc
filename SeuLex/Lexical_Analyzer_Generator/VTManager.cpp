#include "stdafx.h"


VTManager::VTManager(void)
{
}


VTManager::~VTManager(void)
{
}


VTManager* VTManager::Instance()
{
	static VTManager instance;
	return &instance;
}