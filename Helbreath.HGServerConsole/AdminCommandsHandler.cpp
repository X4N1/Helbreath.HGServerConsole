#include "AdminCommandsHandler.h"


void AdminCommandsHandler::KillAllMonsters(int clientId, CClient * player, CNpc ** npcList, CMap ** mapsList)
{
	int playerMapId = player->GetMapID();
	for (int i = 0; i < MAXNPCS; i++)
	{
		if (npcList[i]->GetMapID() == playerMapId && !npcList[i]->m_bIsKilled) {
			
		}
	}
	
}

AdminCommandsHandler::AdminCommandsHandler()
{
}


AdminCommandsHandler::~AdminCommandsHandler()
{
}
