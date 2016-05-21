#pragma once

#include "char\Npc.h"

class NpcKilledHandler
{
public:
	void Handle(short sAttackerH, char cAttackerType, int iNpcH, short sDamage);
	NpcKilledHandler();
	~NpcKilledHandler();
};

