#include "HellclawEvent.h"

extern class CMap	**	g_mapList;
extern class CGame *   g_gameCopy;

void HellclawEvent::Start()
{
	// summon hellclaw on specific map, just once
	this->SummonHellclaw();	
	// notify users
	this->NotifyPlayers();
	// notify when it ends (that hellclaw was killed)
}

HellclawEvent::HellclawEvent()
{

}


HellclawEvent::~HellclawEvent()
{
}

CMap * HellclawEvent::GetMapByName(char * mapName)
{
	for (int i = 0; i < MAXMAPS; i++) {
		if (g_mapList[i] != NULL) {
			if (memcmp(g_mapList[i]->m_cName, mapName, 10) == 0) {
				return g_mapList[i];
			}
		}
	}
}

void HellclawEvent::SummonHellclaw()
{
	printf("SummonHellclaw invoked \n");
	char *cNpcName = "Hellclaw";
	char cName_Master[10];
	char cWaypoint[11];
	int iNamingValue = 0;
	int pX, pY;
	bool bSummoned;
	CMap *map;

	map = this->GetMapByName("middleland");

	if (map == NULL) {
		PutLogList("Unexpected null SummonHellclaw() method\n");
		return;
	}

	iNamingValue = map->iGetEmptyNamingValue();

	if (iNamingValue != -1) {
		ZeroMemory(cName_Master, sizeof(cName_Master));
		wsprintf(cName_Master, "XX%d", iNamingValue);
		cName_Master[0] = '_';
		cName_Master[1] = 1 + 65;

		pX = 192;
		pY = 228;

		g_gameCopy->bCreateNewNpc(cNpcName, cName_Master,
			"middleland", 0,
			MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL,
			-1, FALSE, FALSE, FALSE);
	}
}

void HellclawEvent::NotifyPlayers()
{
	int shortCutIndex = 0;
	while (int index = g_gameCopy->m_iClientShortCut[shortCutIndex++])
	{
		g_gameCopy->SendNotifyMsg(NULL, index, NOTIFY_EVENTSTART, ET_HELLCLAW_SUMMON_ML, NULL, NULL, NULL);
	}
}
