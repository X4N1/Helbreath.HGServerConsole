#pragma once

#include "map\Map.h"
#include "..\Shared\common.h"

class HellclawEvent
{
public:
	void Start();
	HellclawEvent();
	~HellclawEvent();
private:
	class CMap* GetMapByName(char *mapName);
	void SummonHellclaw();
	void NotifyPlayers();
};

