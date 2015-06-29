#pragma once

typedef int (WINAPI *_send)(SOCKET s, const char* buf, int len, int flags);

class CPacket
{
public:
	CPacket(void);
public:
	~CPacket(void);

	static void SendPacket(const unsigned char* packet);
	static void SayMessage(char *msg);
	static void UseOnTarget(short itemId, short container, char pos, int targetId);
	static void UseOnXYZ(short itemId, short container, char pos, short x, short y, char z);
	static void MoveItem(short itemId, short moveFrom, char fromPos, short moveTo, char toPos, char count);
	static void Attack(int targetID);
	static void Move(int move);
};
