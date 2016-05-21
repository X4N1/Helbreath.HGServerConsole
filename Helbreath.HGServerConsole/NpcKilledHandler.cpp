#include "NpcKilledHandler.h"



void NpcKilledHandler::Handle(short sAttackerH, char cAttackerType, int iNpcH, short sDamage)
{
	short  sAttackerWeapon;
	int    * ip, i, iQuestIndex, iExp, iConstructionPoint, iWarContribution;
	double dTmp1, dTmp2, dTmp3;
	char   * cp, cData[120], cQuestRemain, cTxt[120];

	if (m_pNpcList[iNpcH] == NULL) {
		return;
	}
	if (m_pNpcList[iNpcH]->m_bIsKilled == TRUE) {
		return;
	}

	if (cAttackerType == OWNERTYPE_PLAYER) {
		m_pNpcList[iNpcH]->m_killerh = sAttackerH;
	}

	m_pNpcList[iNpcH]->m_bIsKilled = TRUE;
	m_pNpcList[iNpcH]->m_iHP = 0;
	m_pNpcList[iNpcH]->m_iLastDamage = sDamage;

	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;

	RemoveFromTarget(iNpcH, OWNERTYPE_NPC);

	ReleaseFollowMode(iNpcH, OWNERTYPE_NPC);

	m_pNpcList[iNpcH]->m_iTargetIndex = NULL;
	m_pNpcList[iNpcH]->m_cTargetType = NULL;

	if (cAttackerType == OWNERTYPE_PLAYER) {
		sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
	}

	else sAttackerWeapon = 1;

	SendEventToNearClient_TypeA(iNpcH, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTDYING, sDamage, sAttackerWeapon, NULL);

	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(/*10,*/ iNpcH, OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetDeadOwner(iNpcH, OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

	m_pNpcList[iNpcH]->m_cBehavior = BEHAVIOR_DEAD;

	m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;

	m_pNpcList[iNpcH]->m_dwDeadTime = timeGetTime();


	if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cType == MAPTYPE_NOPENALTY_NOREWARD) return;


	NpcDeadItemGenerator(iNpcH, sAttackerH, cAttackerType);


	if ((m_pNpcList[iNpcH]->m_bIsSummoned != TRUE) && (cAttackerType == OWNERTYPE_PLAYER) &&
		(m_pClientList[sAttackerH] != NULL)) {


		iExp = (m_pNpcList[iNpcH]->m_iExp / 3);
		if (m_pNpcList[iNpcH]->m_iNoDieRemainExp > 0)
			iExp += m_pNpcList[iNpcH]->m_iNoDieRemainExp;


		if (m_pClientList[sAttackerH]->m_iAddExp != NULL) {
			dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
			dTmp2 = (double)iExp;
			dTmp3 = (dTmp1 / 100.0f)*dTmp2;
			iExp += (int)dTmp3;
		}


		if (m_bIsCrusadeMode == TRUE) {
			if ((strcmp(m_pClientList[sAttackerH]->m_cMapName, sideMap[NEUTRAL]) != 0)
				|| (strcmp(m_pClientList[sAttackerH]->m_cMapName, "arefarm") != 0)
				|| (strcmp(m_pClientList[sAttackerH]->m_cMapName, "elvfarm") != 0))
				if (iExp > 10) iExp = iExp / 4;
		}

		//m_pClientList[sAttackerH]->m_iExpStock += iExp;

		if (m_pClientList[sAttackerH]->m_iLevel > 100) {
			switch (m_pNpcList[iNpcH]->m_sType) {
			case NPC_RABBIT:
			case NPC_CAT:
				iExp = 0;
				break;
			default: break;
			}
		}

		GetExp(sAttackerH, iExp);

		iQuestIndex = m_pClientList[sAttackerH]->m_iQuest;
		if (iQuestIndex != NULL) {
			if (m_pQuestConfigList[iQuestIndex] != NULL) {
				switch (m_pQuestConfigList[iQuestIndex]->m_iType) {
				case QUESTTYPE_MONSTERHUNT:
					if ((m_pClientList[sAttackerH]->m_bQuestMatchFlag_Loc == TRUE) &&
						(m_pQuestConfigList[iQuestIndex]->m_iTargetType == m_pNpcList[iNpcH]->m_sType)) {
						m_pClientList[sAttackerH]->m_iCurQuestCount++;
						cQuestRemain = (m_pQuestConfigList[m_pClientList[sAttackerH]->m_iQuest]->m_iMaxCount - m_pClientList[sAttackerH]->m_iCurQuestCount);
						SendNotifyMsg(NULL, sAttackerH, NOTIFY_QUESTCOUNTER, cQuestRemain, NULL, NULL, NULL);
						_bCheckIsQuestCompleted(sAttackerH);
					}
					break;
				}
			}
		}
	}
	if ((m_bHeldenianMode)
		&& (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_bIsHeldenianMap == TRUE))
	{
		if ((cAttackerType == OWNERTYPE_NPC)
			&& (m_pNpcList[sAttackerH] != NULL)
			&& (m_pNpcList[sAttackerH]->m_bIsSummoned == TRUE)
			&& (m_pNpcList[sAttackerH]->m_cFollowOwnerType == OWNERTYPE_PLAYER)
			&& (m_pNpcList[sAttackerH]->m_bIsKilled == FALSE)
			&& (m_pClientList[m_pNpcList[sAttackerH]->m_iFollowOwnerIndex] != NULL)
			&& (m_pClientList[m_pNpcList[sAttackerH]->m_iFollowOwnerIndex]->m_bIsKilled == FALSE))
		{
			m_pClientList[m_pNpcList[sAttackerH]->m_iFollowOwnerIndex]->m_iWarContribution += m_pNpcList[iNpcH]->m_iHitDice;
		}
	}
	/*if (cAttackerType == OWNERTYPE_PLAYER) {
	switch (m_pNpcList[iNpcH]->m_sType) {
	case 32: // Unicorn
	//m_pClientList[sAttackerH]->m_reputation -= 5;
	break;

	case 33: // Demon

	break;
	}
	}*/

	if (m_bIsCrusadeMode)
	{
		iConstructionPoint = 0;
		switch (m_pNpcList[iNpcH]->m_sType)
		{
		case 1:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 2:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 3:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 4:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 5:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 6:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 36: iConstructionPoint = 700; iWarContribution = 4000; break;
		case 37: iConstructionPoint = 700; iWarContribution = 4000; break;
		case 38: iConstructionPoint = 500; iWarContribution = 2000; break;
		case 39: iConstructionPoint = 500; iWarContribution = 2000; break;
		case 40: iConstructionPoint = 1500; iWarContribution = 5000; break;
		case 41: iConstructionPoint = 5000; iWarContribution = 10000; break;
		case 43: iConstructionPoint = 500; iWarContribution = 1000; break;
		case 44: iConstructionPoint = 1000; iWarContribution = 2000; break;
		case 45: iConstructionPoint = 1500; iWarContribution = 3000; break;
		case 46: iConstructionPoint = 1000; iWarContribution = 2000; break;
		case 47: iConstructionPoint = 1500; iWarContribution = 3000; break;
		case 51: iConstructionPoint = 800; iWarContribution = 1500; break;

		case 64:
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum();
			break;
		}

		if (iConstructionPoint != NULL) {
			switch (cAttackerType)
			{
			case OWNERTYPE_PLAYER:
				if (m_pClientList[sAttackerH]->m_side != m_pNpcList[iNpcH]->m_side) {

					m_pClientList[sAttackerH]->m_iConstructionPoint += iConstructionPoint;

					if (m_pClientList[sAttackerH]->m_iConstructionPoint > MAXCONSTRUCTIONPOINT)
						m_pClientList[sAttackerH]->m_iConstructionPoint = MAXCONSTRUCTIONPOINT;

					m_pClientList[sAttackerH]->m_iWarContribution += iWarContribution;
					if (m_pClientList[sAttackerH]->m_iWarContribution > MAXWARCONTRIBUTION)
						m_pClientList[sAttackerH]->m_iWarContribution = MAXWARCONTRIBUTION;

					SendNotifyMsg(NULL, sAttackerH, NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, NULL, NULL);
				}
				else {

					/*
					m_pClientList[sAttackerH]->m_iWarContribution   -= (iWarContribution*2);
					if (m_pClientList[sAttackerH]->m_iWarContribution < 0)
					m_pClientList[sAttackerH]->m_iWarContribution = 0;
					*/


					m_pClientList[sAttackerH]->m_iWarContribution = 0;

					wsprintf(cTxt, "WarContribution: Friendly Npc Killed by player(%s)! ", m_pClientList[sAttackerH]->m_cAccountName);
					PutLogFileList(cTxt, EVENT_LOGFILE);

					SendNotifyMsg(NULL, sAttackerH, NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, NULL, NULL);
				}
				break;

			case OWNERTYPE_NPC:
				if (m_pNpcList[sAttackerH]->m_iGuildGUID != NULL) {
					if (m_pNpcList[sAttackerH]->m_side != m_pNpcList[iNpcH]->m_side) {
						for (i = 1; i < MAXCLIENTS; i++)
							if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_iGuildGUID == m_pNpcList[sAttackerH]->m_iGuildGUID) &&
								(m_pClientList[i]->m_iCrusadeDuty == 3))
							{
								m_pClientList[i]->m_iConstructionPoint += iConstructionPoint;
								if (m_pClientList[i]->m_iConstructionPoint > MAXCONSTRUCTIONPOINT)
									m_pClientList[i]->m_iConstructionPoint = MAXCONSTRUCTIONPOINT;

								SendNotifyMsg(NULL, i, NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, NULL, NULL);
								goto NKH_GOTOPOINT1;
							}

						ZeroMemory(cData, sizeof(cData));
						cp = (char *)cData;
						*cp = GSM_CONSTRUCTIONPOINT;
						cp++;
						ip = (int*)cp;
						*ip = m_pNpcList[sAttackerH]->m_iGuildGUID;
						cp += 4;
						ip = (int*)cp;
						*ip = iConstructionPoint;
						cp += 4;
						bStockMsgToGateServer(cData, 9);
					}
				}
				break;
			}
		}
	}

NKH_GOTOPOINT1:;

	if (m_pNpcList[iNpcH]->m_cSpecialAbility == 7) {
		m_pNpcList[iNpcH]->m_iMP = 100;
		m_pNpcList[iNpcH]->m_iMagicHitRatio = 100;
		m_pNpcList[iNpcH]->magicHandler(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 30);
	}
	else if (m_pNpcList[iNpcH]->m_cSpecialAbility == 8) {
		m_pNpcList[iNpcH]->m_iMP = 100;
		m_pNpcList[iNpcH]->m_iMagicHitRatio = 100;
		m_pNpcList[iNpcH]->magicHandler(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 61);
	}
	if (m_bHeldenianMode && (m_iHeldenianType == 1) && m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_bIsHeldenianMap)
	{
		if ((m_pNpcList[iNpcH]->m_sType == NPC_CT) || (m_pNpcList[iNpcH]->m_sType == NPC_AGC))
		{
			if (m_pNpcList[iNpcH]->m_side == 1)
			{
				m_iHeldenianAresdenLeftTower--;
				wsprintf(cTxt, "Aresden Tower Broken, Left TOWER %d", m_iHeldenianAresdenLeftTower);
				PutLogList(cTxt);
				UpdateHeldenianStatus();
				if (m_iHeldenianAresdenLeftTower == 0)	HeldenianEndWarNow(m_iHeldenianType, ELVINE);
			}
			else if (m_pNpcList[iNpcH]->m_side == 2)
			{
				m_iHeldenianElvineLeftTower--;
				wsprintf(cTxt, "Elvine Tower Broken, Left TOWER %d", m_iHeldenianElvineLeftTower);
				PutLogList(cTxt);
				UpdateHeldenianStatus();
				if (m_iHeldenianElvineLeftTower == 0)	HeldenianEndWarNow(m_iHeldenianType, ARESDEN);
			}
		}
	}
}



NpcKilledHandler::NpcKilledHandler()
{
}


NpcKilledHandler::~NpcKilledHandler()
{
}
