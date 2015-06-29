//#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Constants.h"
#include "Packet.h"

unsigned int adler(unsigned char *data, unsigned int len)
{
    unsigned int a = 1, b = 0;
    while (len > 0) 
    {
        unsigned int tlen = len > 5552 ? 5552 : len;
        len -= tlen;
        do {
            a += *data++;
            b += a;
        } while (--tlen);
        a %= 65521;
        b %= 65521;
    }
    return (b << 16) | a; 
}

CPacket::CPacket(void)
{
}

CPacket::~CPacket(void)
{
}
// TODO: Recreate SendFunction
//void CPacket::SendPacket(unsigned char *packet)
//{
//	::SendPacket(GetCurrentProcessId(), packet, true, false);
//}

//void CPacket::SendPacket(unsigned char *packet)
//{
//	SOCKET *tSock = (unsigned int*)(*(unsigned int*)0x768C50) + 4;
//	unsigned char temp[16096];
//	unsigned char *key = (unsigned char*)0x768C7C;
//	int ret = XTEA(true, 0, key, packet, temp, false);
//	send(*tSock, (char*)temp, ret, 0);
//}

/*
key
07 78 CA 29 91 D7 90 5D B9 F1 2D DD BA 08 95 3C

move north
0C 00 14 04 1E 13 95 84 AF 79 90 15 AF 7E F8 50 99 6F DA 8E 09 A6 83 02 F4 CC 59 0A 09 B8 2E 43



*/
void CPacket::SendPacket(unsigned char *packet)
{
	SOCKET *socket;
	_send mysend;
	mysend = *(_send*)Address::SEND_ADR;//GetProcAddress(GetModuleHandle("ws2_32.dll"), "send");
	
	unsigned char temp[1024];
	socket = (SOCKET*)((*(unsigned int*)Address::SOCKET_PTR)+4);

	int ret = XTEA(true, (unsigned char*)Address::XTEA_KEY, packet, temp+4);
	*(unsigned short*)temp = *(unsigned short*)(temp+4)+4;
	*(unsigned int*)(temp+2) = adler(temp+6, ret-2);

	mysend(*socket, (char*)temp, *(unsigned short*)(temp)+2, 0);
}

void CPacket::SayMessage(char *msg)
{
	int len = (int)strlen(msg);
	unsigned char packet[256];
	packet[0] = len+4;
	packet[1] = 0x00;

	packet[2] = 0x96;
	packet[3] = 0x01;
	packet[4] = len;
	packet[5] = 0x00;
	CopyMemory(&packet[6], msg, len);

	CPacket::SendPacket(packet);
}
//0D 00 84 FF FF 00 00 00 58 0C 00 E8 47 00 10 
void CPacket::UseOnTarget(int itemId, int container, int pos, int targetId)
{
	unsigned char packet[15];
	packet[0] = 0x0D;
	packet[1] = 0x00;

	packet[2] = 0x84;
	packet[3] = 0xFF;
	packet[4] = 0xFF;
	packet[5] = container;
	packet[6] = 0x00;
	packet[7] = pos;
	packet[8] = itemId & 0xFF;
	packet[9] = itemId >> 8 & 0xFF;
	packet[10] = pos;
	packet[11] = targetId & 0xFF;
	packet[12] = targetId >> 8 & 0xFF;
	packet[13] = targetId >> 16 & 0xFF;
	packet[14] = targetId >> 24 & 0xFF;

	CPacket::SendPacket(packet);
}

void CPacket::UseOnXYZ(int itemId, int container, int pos, int x, int y, int z)
{
	unsigned char packet[19];
	packet[0] = 0x11;
	packet[1] = 0x00;
	packet[2] = 0x83;
	packet[3] = 0xFF;
	packet[4] = 0xFF;
	packet[5] = container;
	packet[6] = 0x00;
	packet[7] = pos;
	packet[8] = itemId & 0xFF;
	packet[9] = itemId >> 8 & 0xFF;
	packet[10] = pos;
	packet[11] = x & 0xFF;
	packet[12] = x >> 8 & 0xFF;
	packet[13] = y & 0xFF;
	packet[14] = y >> 8 & 0xFF;
	packet[15] = z & 0xFF;
	packet[16] = 0x63;
	packet[17] = 0x00;
	packet[18] = 0x01;

	CPacket::SendPacket(packet);
}

void CPacket::MoveItem(int itemId, int moveFrom, int fromPos, int moveTo, int toPos, int count)
{
	unsigned char packet[17];
	packet[0] = 0x0F;
	packet[1] = 0x00;
	packet[2] = 0x78;
	packet[3] = 0xFF;
	packet[4] = 0xFF;
	packet[5] = moveFrom;
	packet[6] = 0x00;
	packet[7] = fromPos;
	packet[8] = itemId & 0xFF;
	packet[9] = itemId >> 8;
	packet[10] = fromPos;
	packet[11] = 0xFF;
	packet[12] = 0xFF;
	packet[13] = moveTo;
	packet[14] = 0x00;
	packet[15] = toPos;
	packet[16] = count;

	CPacket::SendPacket(packet);
}

void CPacket::Attack(int targetID)
{
	unsigned char packet[7];
	packet[0] = 0x05;
	packet[1] = 0x00;
	packet[2] = 0xA1;
	packet[3] = targetID & 0xFF;
	packet[4] = targetID >> 8 & 0xFF;
	packet[5] = targetID >> 16 & 0xFF;
	packet[6] = targetID >> 24 & 0xFF;

	CPacket::SendPacket(packet);
}

void CPacket::Move(int move)
{
	unsigned char packet[3];
	packet[0] = 0x05;
	packet[1] = 0x00;
	packet[2] = 0x65 + move;

	CPacket::SendPacket(packet);
}