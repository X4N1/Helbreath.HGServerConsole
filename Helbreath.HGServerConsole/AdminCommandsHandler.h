#pragma once

#include "char\Client.h"
#include "map\Map.h"
#include "char\Npc.h"

class AdminCommandsHandler
{
public:
	void KillAllMonsters(int clientId, CClient *player, CNpc **npcList, CMap **mapsList);
	AdminCommandsHandler();
	~AdminCommandsHandler();
};

