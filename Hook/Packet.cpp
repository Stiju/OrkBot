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

void CPacket::SendPacket(unsigned char *packet)
{
	SOCKET *socket;
	_send mysend;
	mysend = *(_send*)Address::SEND_ADR;
	
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