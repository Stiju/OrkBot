#include <windows.h>
#include <cstdio>
#include "Memory.h"
#include "Constants.h"
#include "Hook.h"
#include "Packet.h"
#include "Hotkeys.h"
#include "Tibia.h"

CTibia::CTibia(void)
{
}

CTibia::~CTibia(void)
{
}

unsigned int CTibia::GetPlayer(void)
{
	unsigned int *playerID = (unsigned int*)Address::PLAYER_ID, *cID;
	for(unsigned int i = Address::BATTLELIST_BEGIN; i < Address::BATTLELIST_END; i += sizeof(SCreature))
	{
		cID = (unsigned int*)i;
		if(*playerID == *cID)
		{
			return i;
		}
	}
	return 0;
}

unsigned int CTibia::GetPlayer(unsigned int nID, bool OnScreen)
{
	if(nID == 0)
		return 0;
	SCreature *c;
	int *playerX = (int*)Address::PLAYER_X;
	int *playerY = (int*)Address::PLAYER_Y;
	int *playerZ = (int*)Address::PLAYER_Z;
	for(unsigned int i = Address::BATTLELIST_BEGIN; i < Address::BATTLELIST_END; i += sizeof(SCreature))
	{
		c = (SCreature*)i;
		if(c->CreatureID == 0)
			break;
		if(nID == c->CreatureID)
		{
			if(OnScreen == true)
			{
				if(c->IsVisible == 0)
				continue;
				if(c->Z != *playerZ)
					continue;
				if(c->Health <= 0)
					continue;
				if(c->X >= *playerX-7 && c->X <= *playerX+7 && 
						c->Y >= *playerY-5 && c->Y <= *playerY+5)
					return i;
			}
			else
				return i;
		}
	}
	return 0;
}

unsigned int CTibia::GetPlayer(char *lpName)
{
	SCreature *c;
	for(unsigned int i = Address::BATTLELIST_BEGIN; i < Address::BATTLELIST_END; i += sizeof(SCreature))
	{
		c = (SCreature*)i;
		if(stricmp(c->Name, lpName) == 0)
		{
			return i;
		}
	}
	return 0;
}

unsigned int CTibia::GetFriend(unsigned int nMinHealth)
{
	SCreature *c, *leastHp = 0;
	unsigned int ret = 0;
	int *playerX = (int*)Address::PLAYER_X;
	int *playerY = (int*)Address::PLAYER_Y;
	int *playerZ = (int*)Address::PLAYER_Z;
	for(unsigned int i = Address::BATTLELIST_BEGIN; i < Address::BATTLELIST_END; i += sizeof(SCreature))
	{
		c = (SCreature*)i;
		if(c->CreatureID == 0)
			break;
		if(c->IsVisible == 0)
			continue;
		if(c->CreatureID >= 0x40000000)
			continue;
		if(c->Z != *playerZ)
			continue;
		if(c->Health > nMinHealth)
			continue;
		for(int x = 0; x < AllyNameListCount; x++)
		{
			if(stricmp(c->Name, AllyNameList[x].Name) == 0)
			{
				if(c->X >= *playerX-7 && c->X <= *playerX+7 && 
					c->Y >= *playerY-5 && c->Y <= *playerY+5)
				{
					if(bLeastHealth == true)
					{
						if(leastHp == 0)
						{
							leastHp = (SCreature*)i;
							ret = i;
						}
						if(c->Health < leastHp->Health)
						{
							leastHp = (SCreature*)i;
							ret = i;
						}
						continue;
					}
					else
						return i;
				}
			}
		}
	}
	return ret;
}

unsigned int CTibia::GetEnemy(unsigned int nMinHealth)
{
	SCreature *c, *leastHp = 0;
	unsigned int ret = 0;
	int *playerX = (int*)Address::PLAYER_X;
	int *playerY = (int*)Address::PLAYER_Y;
	int *playerZ = (int*)Address::PLAYER_Z;
	for(unsigned int i = Address::BATTLELIST_BEGIN; i < Address::BATTLELIST_END; i += sizeof(SCreature))
	{
		c = (SCreature*)i;
		if(c->CreatureID == 0)
			break;
		if(c->IsVisible == 0)
			continue;
		if(c->CreatureID >= 0x40000000)
			continue;
		if(c->Z != *playerZ)
			continue;
		if(c->Health > nMinHealth)
			continue;
		for(int x = 0; x < EnemyNameListCount; x++)
		{
			if(stricmp(c->Name, EnemyNameList[x].Name) == 0)
			{
				if(c->X >= *playerX-7 && c->X <= *playerX+7 && 
					c->Y >= *playerY-5 && c->Y <= *playerY+5)
				{
					if(bLeastHealth == true)
					{
						if(leastHp == 0)
						{
							leastHp = (SCreature*)i;
							ret = i;
						}
						if(c->Health < leastHp->Health)
						{
							leastHp = (SCreature*)i;
							ret = i;
						}
						continue;
					}
					else
						return i;
				}
			}
		}
	}
	return ret;
}

//bool CTibia::IsOnline(void)
//{
//	unsigned int *isOnline = (unsigned int*)Address::ISONLINE;
//	return (*isOnline == 8);
//}
//
//bool CTibia::IsPlayerState(PlayerStates ps)
//{
//	unsigned int *playerStates = (unsigned int*)Address::PLAYER_STATUS;
//	return ((*playerStates & ps) > 0);
//}

bool CTibia::IsOnline(void)
{
	return (*(unsigned int*)Address::ISONLINE == 8);
}

bool CTibia::IsPlayerState(PlayerStates ps)
{
	return ((*(unsigned int*)Address::PLAYER_CONDITION & ps) > 0);
}

bool CTibia::FindItem(int nItemID, SContainerInfo *cInfo)
{
	int x = 0;
	SContainer *c;
	for(unsigned int container = Address::CONTAINER_BEGIN; container < Address::CONTAINER_END; container += sizeof(SContainer), x++)
	{
		c = (SContainer*)container;
		if(c->IsOpen == 1)
		{
			for(int i = 0; i < c->Count; i++)
			{
				if(c->Items[i].ItemID == nItemID)
				{
					cInfo->Container = x;
					cInfo->Count = c->Items[i].Count;
					cInfo->Position = i;
					return true;
				}
			}
		}
	}
	return false;
}

int CTibia::AutoSelfHeal(void)
{
	unsigned int *health = (unsigned int*)Address::PLAYER_HEALTH;
	unsigned int *mana = (unsigned int*)Address::PLAYER_MANA;
	unsigned int *playerID = (unsigned int*)Address::PLAYER_ID;
	SContainerInfo cInfo;
	char buf[32];
	GetWindowText(hAutoHealDelay, buf, 32);
	unsigned int dwDelay = atoi(buf);
	unsigned int dwTime = timeGetTime();
	if(SpellLo.Use == true)
	{
		if(dwTime - dwSpellTime >= dwDelay)
		{
			if(*health <= SpellLo.Health)
			{
				if(*mana >= SpellLo.Mana)
				{
					CPacket::SayMessage(SpellLo.Spell);
					if(bDoubleHeal == true)
						dwSpellTime = timeGetTime();
					else
					{
						dwSpellTime = dwItemTime = timeGetTime();
						return 0;
					}
				}
			}
		}
	}
	if(HealItem.Use == true)
	{
		if(dwTime - dwItemTime >= dwDelay)
		{
			if(*health <= HealItem.Health)
			{
				bool bUsed = false;
				if(bShootFromClosedBp == true)
				{
					CPacket::UseOnTarget(HealItem.ItemID, 0, 0, *playerID);
					bUsed = true;
				}
				else if(FindItem(HealItem.ItemID, &cInfo) == true)
				{
					CPacket::UseOnTarget(HealItem.ItemID, 0x40 + cInfo.Container, cInfo.Position, *playerID);
					bUsed = true;
				}
				//else
				//{
				//	SetWindowText(g_hWnd, "Healing Item not found!");
				//}
				if(bUsed == true)
				{
					if(bDoubleHeal == true)
						dwItemTime = timeGetTime();
					else
					{
						dwSpellTime = dwItemTime = timeGetTime();
						return 0;
					}
				}
				//return 0;
			}
		}
	}
	if(SpellHi.Use == true)
	{
		if(dwTime - dwSpellTime >= dwDelay)
		{
			if(*health <= SpellHi.Health)
			{
				if(*mana >= SpellHi.Mana)
				{
					CPacket::SayMessage(SpellHi.Spell);
					if(bDoubleHeal == true)
						dwSpellTime = timeGetTime();
					else
					{
						dwSpellTime = dwItemTime = timeGetTime();
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

int CTibia::AntiParalyze(void)
{
	unsigned int *mana = (unsigned int*)Address::PLAYER_MANA;
	char buf[32];
	GetWindowText(hAutoHealDelay, buf, 32);
	unsigned int dwDelay = atoi(buf);
	unsigned int dwTime = timeGetTime();
	if(IsPlayerState(PlayerStates::Slowed) == true)
	{
		if(dwTime - dwSpellTime >= dwDelay)
		{
			if(*mana >= SpellHi.Mana)
			{
				CPacket::SayMessage(SpellHi.Spell);
				dwSpellTime = timeGetTime();
			}
		}
	}
	return 0;
}

int CTibia::AutoAllyHeal(void)
{
	if(AllyNameListCount <= 0)
		return 0;
	SCreature *c;
	SContainerInfo cInfo;
	int *playerX = (int*)Address::PLAYER_X;
	int *playerY = (int*)Address::PLAYER_Y;
	int *playerZ = (int*)Address::PLAYER_Z;
	
	char buf[32];
	GetWindowText(hAutoHealDelay, buf, 32);
	unsigned int dwDelay = atoi(buf);
	unsigned int dwTime = timeGetTime();
	for(unsigned int i = Address::BATTLELIST_BEGIN; i < Address::BATTLELIST_END; i += sizeof(SCreature))
	{
		c = (SCreature*)i;
		if(c->CreatureID == 0)
			break;
		if(c->IsVisible == 0)
			continue;
		if(c->CreatureID >= 0x40000000)
			continue;
		if(c->Z != *playerZ)
			continue;
		if(c->Health > AllySpell.Health)
			continue;
		for(int x = 0; x < AllyNameListCount; x++)
		{
			if(stricmp(c->Name, AllyNameList[x].Name) == 0)
			{
				if(c->X >= *playerX-7 && c->X <= *playerX+7 && 
					c->Y >= *playerY-5 && c->Y <= *playerY+5)
				{
					if(AllySpell.Use == true)
					{
						if(dwTime - dwSpellTime >= dwDelay)
						{
							sprintf_s(AllySpell.Spell, 256, "Exura Sio \"%s", c->Name);
							CPacket::SayMessage(AllySpell.Spell);
							if(bDoubleHeal == true)
								dwSpellTime = timeGetTime();
							else
								dwSpellTime = dwItemTime = timeGetTime();
							return 0;
						}
					}
					if(AllyHealItem.Use == true)
					{
						if(dwTime - dwItemTime >= dwDelay)
						{
							bool bUsed = false;
							if(bShootFromClosedBp == true)
							{
								CPacket::UseOnTarget(AllyHealItem.ItemID, 0, 0, c->CreatureID);
								bUsed = true;
							}
							else if(FindItem(HealItem.ItemID, &cInfo) == true)
							{
								CPacket::UseOnTarget(AllyHealItem.ItemID, 0x40 + cInfo.Container, cInfo.Position, c->CreatureID);
								bUsed = true;
							}
							//else
							//{
							//	SetWindowText(g_hWnd, "Healing Item not found!");
							//}
							if(bUsed == true)
							{
								if(bDoubleHeal == true)
									dwItemTime = timeGetTime();
								else
									dwSpellTime = dwItemTime = timeGetTime();
								return 0;
							}
						}
					}
				}
				break;
			}
		}
	}
	return 0;
}

/*
void CTibia::MagwallTimer(void)
{
	char buf[32];
	unsigned int *pointer = (unsigned int*)Address::GAMESCREENPTR;
	//unsigned int *pointer2 = (unsigned int*)(*pointer + 0x2C);
	unsigned int *pointer2 = (unsigned int*)(*pointer + 0x30);
	unsigned int *pointer3 = (unsigned int*)(*pointer2 + 0x04);
	SGameScreen *gs = (SGameScreen*)(*pointer3);
	SCreature *c = (SCreature*)CTibia::GetPlayer();

	unsigned int t = timeGetTime();
	for(int i = 0; i < MagwallArray; i++)
	{
		if(Magwalls[i].Time > t)
		{
			if(Magwalls[i].X >= c->X-7 && Magwalls[i].X <= c->X+7 && 
				Magwalls[i].Y >= c->Y-5 && Magwalls[i].Y <= c->Y+5 &&
				Magwalls[i].Z == c->Z)
			{
				//int ztemp = 7 - Magwalls[i].Z - c->Z;
				double sqmpx = (gs->Width - 2) / 15;
				//double sqmpx = (gs->Height - 2) / 11;
				int MoveX = (sqmpx / 32) * (abs(c->MoveX));
				int MoveY = (sqmpx / 32) * (abs(c->MoveY));
				MoveX = (c->MoveX > 0) ? -MoveX : MoveX;
				MoveY = (c->MoveY > 0) ? -MoveY : MoveY;
				int x = gs->X + (sqmpx * (7+ -(c->X - Magwalls[i].X))) + (sqmpx / 2) + MoveX;
				int y = gs->Y + (sqmpx * (5+ -(c->Y - Magwalls[i].Y))) + (sqmpx / 2) - 5 + MoveY;
				int time = (Magwalls[i].Time - t);
				sprintf_s(buf, 32, "%.1f", (float)time/1000);
				PrintText(1, (int)x, (int)y, 2, 239, 239, 0, buf, 1);
			}
		}
	}
}*/

void CTibia::MagwallTimer(void)
{
	char buf[32];
	unsigned int *pointer = (unsigned int*)Address::GAMESCREENPTR;
	//unsigned int *pointer2 = (unsigned int*)(*pointer + 0x2C);
	unsigned int *pointer2 = (unsigned int*)(*pointer + 0x30);
	unsigned int *pointer3 = (unsigned int*)(*pointer2 + 0x04);
	SGameScreen *gs = (SGameScreen*)(*pointer3);
	SCreature *c = (SCreature*)CTibia::GetPlayer();

	unsigned int t = timeGetTime();
	for(int i = 0; i < MagwallArray; i++)
	{
		if(Magwalls[i].Time > t)
		{
			if(Magwalls[i].X >= c->X-7 && Magwalls[i].X <= c->X+7 && 
				Magwalls[i].Y >= c->Y-5 && Magwalls[i].Y <= c->Y+5 &&
				Magwalls[i].Z == c->Z)
			{
				/*//int ztemp = 7 - Magwalls[i].Z - c->Z;
				double sqmpx = (gs->Width - 2) / 15;
				//double sqmpx = (gs->Height - 2) / 11;
				int MoveX = (sqmpx / 32) * (abs(c->MoveX));
				int MoveY = (sqmpx / 32) * (abs(c->MoveY));
				MoveX = (c->MoveX > 0) ? -MoveX : MoveX;
				MoveY = (c->MoveY > 0) ? -MoveY : MoveY;
				int wx = 0;
				int wy = 0; //c->Y - *(int*)Address::PLAYER_GOTOY;
				if(*(int*)Address::PLAYER_GOTOX != 0)
				{
					wx = c->X - *(int*)Address::PLAYER_GOTOX;
					wy = c->Y - *(int*)Address::PLAYER_GOTOY;
				}

				/*if(c->IsWalking == 1)
				{
					switch(c->WalkDirection)
					{
					case 0: wy = 1; break;
					case 1: wx = -1; break;
					case 2: wy = -1; break;
					case 3: wx = 1; break;
					}
				}* /
				int x = gs->X + (sqmpx * (8 + -(c->X - Magwalls[i].X) + wx)) - (sqmpx / 2) + MoveX;
				int y = gs->Y + (sqmpx * (6 + -(c->Y - Magwalls[i].Y) + wy)) - (sqmpx / 2) - 5 + MoveY;*/
				double sqmpx = (double)(gs->Width - 2) / 15;

				int MoveX = (int)((sqmpx / 32) * (abs(c->MoveX)));
				int MoveY = (int)((sqmpx / 32) * (abs(c->MoveY)));
				MoveX = (c->MoveX > 0) ? -MoveX : MoveX;
				MoveY = (c->MoveY > 0) ? -MoveY : MoveY;

				int x = gs->X + (int)((sqmpx * (8 + -((int)c->X - (int)Magwalls[i].X + *(int*)Address::PLAYER_MOVING_X))) - (sqmpx / 2)) + MoveX;
				int y = gs->Y + (int)((sqmpx * (6 + -((int)c->Y - (int)Magwalls[i].Y + *(int*)Address::PLAYER_MOVING_Y))) - (sqmpx / 2)) - 5 + MoveY;
				int time = (Magwalls[i].Time - t);
				sprintf_s(buf, 32, "%.1f", (float)time/1000);
				PrintText(1, (int)x, (int)y, 2, 239, 239, 0, buf, 1);
			}
		}
	}
}

void CTibia::SpellTimers(void)
{
	char buf[32];
	int time;
	unsigned int *pointer = (unsigned int*)Address::GAMESCREENPTR;
	//unsigned int *pointer2 = (unsigned int*)(*pointer + 0x2C);
	unsigned int *pointer2 = (unsigned int*)(*pointer + 0x30);
	unsigned int *pointer3 = (unsigned int*)(*pointer2 + 0x04);
	SGameScreen *gs = (SGameScreen*)(*pointer3);
	int y = gs->Y, x = gs->X + gs->Width - 5;
	unsigned int t = timeGetTime();
	if(isMagicShield > t)
	{
		time = isMagicShield - t;
		sprintf_s(buf, 32, "Magic Shield: %.2d:%.2d:%.1d", (time/1000/60), (time/1000 % 60), (time/100 % 10));
		PrintText(1, x, y, 2, 0, 255, 255, buf, 2);
		y += 12;
	}
	if(isInvisible > t)
	{
		time = isInvisible - t;
		sprintf_s(buf, 32, "Invisible: %.2d:%.2d:%.1d", (time/1000/60), (time/1000 % 60), (time/100 % 10));
		PrintText(1, x, y, 2, 255, 0, 255, buf, 2);
		y += 12;
	}
	if(isHaste > t)
	{
		time = isHaste - t;
		sprintf_s(buf, 32, "Haste: %.2d:%.2d:%.1d", (time/1000/60), (time/1000 % 60), (time/100 % 10));
		PrintText(1, x, y, 2, 0, 255, 0, buf, 2);
		y += 12;
	}
}

void CTibia::ShowInvisibleCreature(void)
{
	unsigned char x = 0xEB;
	//CMemory::WriteMemory(Address::INVISIBLE1, &x, 1);
	//CMemory::WriteMemory(Address::INVISIBLE2, &x, 1);
}

void CTibia::XRay(bool bEnable)
{
	if(bEnable == true)
	{
		CMemory::WriteMemory(Address::XRAY, (LPVOID)HardcoreBytes::XRayOn, 2);
	}
	else
	{
		CMemory::WriteMemory(Address::XRAY, (LPVOID)HardcoreBytes::XRayOff, 2);
	}
}

void CTibia::LevelSpy(void)
{
	if(bSpy1 == true)
	{
		CMemory::WriteMemory(Address::LEVELSPY3, (LPVOID)HardcoreBytes::LevelSpyOriginal, 6);
		CMemory::WriteMemory(Address::LEVELSPY2, (LPVOID)HardcoreBytes::LevelSpyOriginal, 6);
		CMemory::WriteMemory(Address::LEVELSPY1, (LPVOID)HardcoreBytes::LevelSpyOriginal, 6);
		if(bXRay == false)
			XRay(false);
		bSpy1 = false;
	}
	else
	{
		CMemory::Nop(Address::LEVELSPY1, 6);
		CMemory::Nop(Address::LEVELSPY2, 6);
		CMemory::Nop(Address::LEVELSPY3, 6);
		if(bXRay == false)
			XRay(true);
		//unsigned int *pointer1 = (unsigned int*)Address::LEVELSPYPTR;
		//unsigned int *pointer2 = (unsigned int*)(*pointer1 + 0x1C);
		//unsigned int *pointer3 = (unsigned int*)(*pointer2 + 0x25D8);
		int *playerZ = (int*)Address::PLAYER_Z;
		levelSpyPZ = *playerZ;
		if(levelSpyPZ <= 7)
			nSpy = 7 - levelSpyPZ;
		else
			nSpy = 2;

		nSpy2 = 0;
		bSpy1 = true;
		bSpy2 = false;
	}
}


/*
map > *pointer2
*/
void CTibia::LevelSpy(bool bLookUp)
{
	if(IsOnline() == false)
		return;
	unsigned int *pointer1 = (unsigned int*)Address::LEVELSPYPTR;
	unsigned int *pointer2 = (unsigned int*)(*pointer1 + 0x28);
	unsigned int *levelSpy = (unsigned int*)(*pointer2 + 0x5BC0);
	//unsigned int *pointer2 = (unsigned int*)(*pointer1 + 0x28);
	//unsigned int *levelSpy = (unsigned int*)(*pointer2 + 0x2A88);
	int *playerZ = (int*)Address::PLAYER_Z;
	char buf[256];
	if(!bSpy1)
		LevelSpy();
	if(bLookUp == true)
	{
		if(*playerZ <= 7 && nSpy < 7 || *playerZ >= 8 && nSpy < 4 )
		{
			if(bSpy2 == false)
			{
				bSpy2 = true;
				if(*levelSpy == 7 - levelSpyPZ || *playerZ >= 8)
				{
					nSpy++;
					nSpy2++;
				}
			}
			else if(nSpy2+1 == 0)
			{
				LevelSpy();
				nSpy2++;
			}
			else
			{
				nSpy++;
				nSpy2++;
			}
			*levelSpy = nSpy;
			sprintf_s(buf, 256, "Level Spy -> Level: %d", nSpy2);
			StatusMessage(buf, 0);
		}
		else
		{
			if(nSpy == 7 && nSpy2 == 0)
				LevelSpy();
			StatusMessage("Sorry, not possible.", 0);
		}
	}
	else
	{
		if(nSpy > 0 || bSpy2 == false && *levelSpy != 0)
		{
			if(bSpy2 == false)
			{
				bSpy2 = true;
				if(*levelSpy == 7 - levelSpyPZ || *playerZ >= 8)
				{
					if(nSpy > 0)
					{
						nSpy--;
						nSpy2--;
					}
				}
			}
			else if(nSpy2-1 == 0)
			{
				LevelSpy();
				nSpy2--;
			}
			else
			{
				nSpy--;
				nSpy2--;
			}
			*levelSpy = nSpy;
			sprintf_s(buf, 256, "Level Spy -> Level: %d", nSpy2);
			StatusMessage(buf, 0);
		}
		else
		{
			if(nSpy == 0 && nSpy2 == 0)
				LevelSpy();
			StatusMessage("Sorry, not possible.", 0);
		}
	}
}

void CTibia::ShootMagwall(int nTarget, int nSqm)
{
	//unsigned int *targetID = (unsigned int*)Address::PLAYER_ATTACK_ID;
	//SCreature *c = (SCreature*)GetPlayer(*targetID);
	//SCreature *c = 0;
	//unsigned int *targetID = (unsigned int*)Address::PLAYER_ATTACK_ID;
	//if(*targetID == 0)
	//	return;
	//SCreature *c = (SCreature*)GetPlayer(*targetID);

	SCreature *c = 0;
	switch(nTarget)
	{
	case HotkeyCreature::Self:
		c = (SCreature*)GetPlayer();
		break;
	case HotkeyCreature::Target:
		if(PlayerTarget == 0)
			return;
		c = (SCreature*)GetPlayer(PlayerTarget, true);
		break;
	case HotkeyCreature::Friend:
		c = (SCreature*)GetFriend(100); 
		break;
	case HotkeyCreature::Enemy:
		c = (SCreature*)GetEnemy(100); 
		break;
	}

	SContainerInfo cInfo;
	if(c > 0)
	{
		int x = c->X, y = c->Y;
		switch(c->Direction)
		{
		case Direction::North:
			y = y - nSqm;
			break;
		case Direction::East:
			x = x + nSqm;
			break;
		case Direction::South:
			y = y + nSqm;
			break;
		case Direction::West:
			x = x - nSqm;
			break;
		}
		if(bShootFromClosedBp == true)
		{
			CPacket::UseOnXYZ(Items::MagicwallRune, 0, 0, x, y, c->Z);
		}
		else if(FindItem(Items::MagicwallRune, &cInfo) == true)
		{
			CPacket::UseOnXYZ(Items::MagicwallRune, 0x40 + cInfo.Container, cInfo.Position, x, y, c->Z);
		}
	}
}

void CTibia::DrinkManaPotion(int nItemID)
{
	unsigned int *mana = (unsigned int*)Address::PLAYER_MANA;
	unsigned int *maxMana = (unsigned int*)Address::PLAYER_MANA_MAX;
	unsigned int *playerID = (unsigned int*)Address::PLAYER_ID;
	if(*mana == *maxMana)
		return;
	SContainerInfo cInfo;
	if(bShootFromClosedBp == true)
	{
		CPacket::UseOnTarget(nItemID, 0, 0, *playerID);
	}
	else if(FindItem(nItemID, &cInfo) == true)
	{
		CPacket::UseOnTarget(nItemID, 0x40 + cInfo.Container, cInfo.Position, *playerID);
	}
}

void CTibia::HotkeyShoot(int nItemID, int nTarget, int nMinHealth)
{
	SContainerInfo cInfo;
	SCreature *c = 0;
	switch(nTarget)
	{
	case HotkeyCreature::Self:
		{
			unsigned int *health = (unsigned int*)Address::PLAYER_HEALTH;
			unsigned int *maxHealth = (unsigned int*)Address::PLAYER_HEALTH_MAX;
			unsigned int *playerID = (unsigned int*)Address::PLAYER_ID;
			if(*health == *maxHealth)
				return;
			if(bShootFromClosedBp == true)
			{
				CPacket::UseOnTarget(nItemID, 0, 0, *playerID);
			}
			else if(FindItem(nItemID, &cInfo) == true)
			{
				CPacket::UseOnTarget(nItemID, 0x40 + cInfo.Container, cInfo.Position, *playerID);
			}
		}break;
	case HotkeyCreature::Target:
		{
			//unsigned int *targetID = (unsigned int*)Address::PLAYER_ATTACK_ID;
			if(PlayerTarget == 0)
				return;
			c = (SCreature*)GetPlayer(PlayerTarget, true);
			if(c == 0)
				return;
			if(c->Health > nMinHealth)
				return;
			if(bShootFromClosedBp == true)
			{
				CPacket::UseOnTarget(nItemID, 0, 0, c->CreatureID);
			}
			else if(FindItem(nItemID, &cInfo) == true)
			{
				CPacket::UseOnTarget(nItemID, 0x40 + cInfo.Container, cInfo.Position, c->CreatureID);
			}
		}break;
	case HotkeyCreature::Friend:
		{
			c = (SCreature*)GetFriend(nMinHealth);
			if(c == 0)
				return;
			if(bShootFromClosedBp == true)
			{
				CPacket::UseOnTarget(nItemID, 0, 0, c->CreatureID);
			}
			else if(FindItem(nItemID, &cInfo) == true)
			{
				CPacket::UseOnTarget(nItemID, 0x40 + cInfo.Container, cInfo.Position, c->CreatureID);
			}
		}break;
	case HotkeyCreature::Enemy:
		{
			c = (SCreature*)GetEnemy(nMinHealth); 
			if(c == 0)
				return;
			if(bShootFromClosedBp == true)
			{
				CPacket::UseOnTarget(nItemID, 0, 0, c->CreatureID);
			}
			else if(FindItem(nItemID, &cInfo) == true)
			{
				CPacket::UseOnTarget(nItemID, 0x40 + cInfo.Container, cInfo.Position, c->CreatureID);
			}
		}break;
	}
}

void CTibia::Loot(void)
{
	int x = 0;
	//SContainer *c;
	for(unsigned int container = Address::CONTAINER_BEGIN; container < Address::CONTAINER_END; container += sizeof(SContainer), x++)
	{
		SContainer *c = (SContainer*)container;
		if(c->IsOpen == 1)
		{
			if(strstr(c->Name, "Dead ") != 0 || strstr(c->Name, "Slain ") != 0)
			{
				for(int i = c->Count-1; i >= 0; i--)
				{
					for(int j = 0; j < LootItemsArray; j++)
					{
						if(c->Items[i].ItemID == LootItems[j].ItemID)
						{
							SContainer *c2 = (SContainer*)Address::CONTAINER_BEGIN + (LootItems[j].LootBP * sizeof(SContainer));
							int k = 0;
							for(k = c2->Count-1; k >= 0; k--)
							{
								if(c2->Items[k].ItemID == c->Items[i].ItemID)
								{
									if(c2->Items[k].Count < 100)
										break;
								}
							}
							if(k < 0)
								k = 0;
							CPacket::MoveItem(c->Items[i].ItemID, 0x40 + x, i, 0x40 + LootItems[j].LootBP, k, c->Items[i].Count);
							Sleep(200);
							break;
						}
					}
				}
			}
		}
	}
}

void CTibia::EquipItem(int nItemID, int nToPos)
{
	SContainerInfo cInfo;
	if(FindItem(nItemID, &cInfo) == true)
	{
		CPacket::MoveItem(nItemID, 0x40 + cInfo.Container, cInfo.Position, nToPos, 0, cInfo.Count);
	}
}

void CTibia::AttackTarget(int nTarget)
{
	SCreature *c = 0;
	switch(nTarget)
	{
	case HotkeyCreature::Self:
		c = (SCreature*)GetPlayer();
		break;
	case HotkeyCreature::Target:
		if(PlayerTarget == 0)
			return;
		c = (SCreature*)GetPlayer(PlayerTarget, true);
		break;
	case HotkeyCreature::Friend:
		c = (SCreature*)GetFriend(100); 
		break;
	case HotkeyCreature::Enemy:
		c = (SCreature*)GetEnemy(100); 
		break;
	}

	if(c > 0)
	{
		//CPacket::Attack(c->CreatureID);
		*(unsigned int*)Address::PLAYER_ATTACK_ID = c->CreatureID;
	}
}

struct VIPLIST
{
	DWORD ID;
	bool Online;
};

VIPLIST vip[100];
void CTibia::FullLight()
{
	for(int i = 0; i < 100; i++)
	{
		bool *status = (bool*)(0x622990 + (i * 44) + 34);
		int *icon = (int*)(0x622990 + (i * 44) + 40);
		DWORD *id = (DWORD*)(0x622990 + (i * 44));
		if(*icon == 2)
		{
			if(vip[i].ID == *id)
			{
				if(vip[i].Online == 0)
				{
					if(*status == 1)
					{
						vip[i].Online = *status;
						MessageBeep(MB_ICONEXCLAMATION);
					}
				}
				else
				{
					if(*status == 0)
					{
						vip[i].Online = *status;
					}
				}
			}
			else
			{
				vip[i].ID = *id;
				vip[i].Online = *status;
			}
		}
	}
}