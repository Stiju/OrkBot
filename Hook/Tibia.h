#pragma once
#include "Constants.h"

struct SCreature {
	unsigned int CreatureID;
	char Name[32];
	unsigned int X;
	unsigned int Y;
	unsigned int Z;
	signed int MoveX;
	signed int MoveY;
	unsigned int unknown1;
	unsigned int unknown2;
	unsigned int unknown3;
	unsigned int unknown4;
	unsigned int unknown5;
	unsigned int IsWalking;
	unsigned int Direction;
	unsigned int WalkDirection;
	unsigned int unknown6;
	unsigned int unknown7;
	unsigned int OutfitType;
	unsigned int OutfitData[4];
	unsigned int Addon;
	unsigned int MountType;
	unsigned int Light;
	unsigned int LightColor;
	unsigned int SquareColor;
	unsigned int Square;
	unsigned int Health;
	unsigned int Speed;
	unsigned int IsVisible;
	unsigned int PKFlag;
	unsigned int PARTYFlag;
	unsigned int unknown8;
	unsigned int GUILDWARFlag;
	unsigned int IsBlocking;
};

struct SContainerItem{
	unsigned int ItemID;
	unsigned int Count;
	unsigned int unknown1;
};

struct SContainer{
	unsigned int IsOpen;
	unsigned int Icon;
	unsigned int unknown1;
	unsigned int unknown2;
	char Name[32];
	unsigned int Size;
	unsigned int HigherContainer;
	unsigned int Count;
	SContainerItem Items[36];
};

struct SContainerInfo {
	int Container;
	int Position;
	int Count;
};

// TODO: Complete GameScreen structure
struct SGameScreen {
	int u1;
	int X;
	int Y;
	int Width;
	int u2;
	int u3;
	int u4;
	int u5;
	int X2;
	int Y2;
	int u6;
	int Height;
};

struct SEquipment {
	SContainerItem Items[10];
};

class CTibia
{
public:
	CTibia(void);
	~CTibia(void);
	static unsigned int GetPlayer(void);
	static unsigned int GetPlayer(unsigned int nID, bool OnScreen);
	static unsigned int GetPlayer(char *lpName);
	static unsigned int GetFriend(unsigned int nMinHealth);
	static unsigned int GetEnemy(unsigned int nMinHealth);
	static bool IsOnline(void);
	static bool IsPlayerState(PlayerStates ps);
	static bool FindItem(int nItemID, SContainerInfo *cInfo);
	static int AutoSelfHeal(void);
	static int AntiParalyze(void);
	static int AutoAllyHeal(void);
	static void MagwallTimer(void);
	static void SpellTimers(void);
	static void ShowInvisibleCreature(void);
	static void XRay(bool bEnable);
	static void LevelSpy(void);
	static void LevelSpy(bool bLookUp);
	static void ShootMagwall(int nTarget, int nSqm);
	static void DrinkManaPotion(int nItemID);
	static void HotkeyShoot(int nItemID, int nTarget, int nMinHealth);
	static void Loot(void);
	static void EquipItem(int nItemID, int nToPos);
	static void AttackTarget(int nTarget);
	static void FullLight();
};
