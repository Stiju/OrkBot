#pragma once

extern "C" int __stdcall XTEA(BOOL, BYTE*, BYTE*, BYTE*);

typedef int (WINAPI *_send)(SOCKET s, const char* buf, int len, int flags);

class CPacket
{
public:
	CPacket(void);
public:
	~CPacket(void);

	static void SendPacket(unsigned char* packet);
	static void SayMessage(char *msg);
	static void UseOnTarget(int itemId, int container, int pos, int targetId);
	static void UseOnXYZ(int itemId, int container, int pos, int x, int y, int z);
	static void MoveItem(int itemId, int moveFrom, int fromPos, int moveTo, int toPos, int count);
	static void Attack(int targetID);
	static void Move(int move);
};
