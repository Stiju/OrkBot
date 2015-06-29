#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "Constants.h"
#include "Packet.h"

unsigned int adler(unsigned char *data, unsigned int len) {
	unsigned int a = 1, b = 0;
	while(len != 0) {
		a = (a + *data++) % 65521;
		b = (b + a) % 65521;
		len--;
	}
	return (b << 16) | a;
}
void encipher(unsigned int num_rounds, unsigned int v[2], unsigned int const key[4]) {
	unsigned int i;
	unsigned int v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
	for(i = 0; i < num_rounds; i++) {
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
	}
	v[0] = v0; v[1] = v1;
}

int XTEA(unsigned char* key, unsigned char* packet) {
	int size = *(short*)packet + 2;
	int numberOfBlocks = size / 8;
	if(size % 8 != 0) {
		numberOfBlocks++;
	}
	for(int i = 0; i < numberOfBlocks; ++i) {
		encipher(32, (unsigned int*)(packet + (i * 8)), (unsigned int*)key);
	}
	return numberOfBlocks * 8;
}

template<int ArraySize>
class Packet {
	unsigned char buffer[(ArraySize / 8 + (ArraySize % 8 != 0 ? 1 : 0)) * 8 + 6];
	short position;
public:
	Packet() : position(8) {}
	template<typename T>
	void Add(T t) {
		assert(sizeof(T) + position <= sizeof(buffer));
		*(T*)(buffer + position) = t;
		position += sizeof(T);
	}
	template<>
	void Add(const char* text) {
		short length = static_cast<short>(strlen(text));
		assert(length + 2 + position <= sizeof(buffer));
		Add(length);
		memcpy(buffer + position, text, length);
		position += length;
	}
	template<>
	void Add(char* text) {
		Add<const char*>(text);
	}
	const unsigned char* Buffer() const {
		return buffer;
	}
	void PrepareForBattle() {
		*(short*)(buffer + 6) = (short)position - 8;
		short size = (short)XTEA((unsigned char*)Address::XTEA_KEY, buffer + 6);
		*(short*)buffer = size + 4;
		*(unsigned int*)(buffer + 2) = adler(buffer + 6, size);
	}
};

CPacket::CPacket(void)
{
}

CPacket::~CPacket(void)
{
}

void CPacket::SendPacket(const unsigned char *packet)
{
	SOCKET *socket;
	_send mysend;
	mysend = *(_send*)Address::SEND_ADR;
	socket = (SOCKET*)((*(unsigned int*)Address::SOCKET_PTR)+4);
	mysend(*socket, (const char*)packet, *(unsigned short*)(packet)+2, 0);
}

void CPacket::SayMessage(char *msg)
{
	Packet<256> packet;
	packet.Add<char>(0x96);
	packet.Add<char>(0x01);
	packet.Add(msg);
	packet.PrepareForBattle();
	CPacket::SendPacket(packet.Buffer());
}

void CPacket::UseOnTarget(short itemId, short container, char pos, int targetId)
{
	Packet<15> packet;
	packet.Add<char>(0x84);
	packet.Add<short>(0xFFFF);
	packet.Add(container);
	packet.Add(pos);
	packet.Add(itemId);
	packet.Add(pos);
	packet.Add(targetId);
	packet.PrepareForBattle();
	CPacket::SendPacket(packet.Buffer());
}

void CPacket::UseOnXYZ(short itemId, short container, char pos, short x, short y, char z)
{
	Packet<19> packet;
	packet.Add<char>(0x83);
	packet.Add<short>(0xFFFF);
	packet.Add(container);
	packet.Add(pos);
	packet.Add(itemId);
	packet.Add(pos);
	packet.Add(x);
	packet.Add(y);
	packet.Add(z);
	packet.Add<short>(0x63);
	packet.Add<char>(0x01);
	packet.PrepareForBattle();
	CPacket::SendPacket(packet.Buffer());
}

void CPacket::MoveItem(short itemId, short moveFrom, char fromPos, short moveTo, char toPos, char count)
{
	Packet<17> packet;
	packet.Add<char>(0x78);
	packet.Add<short>(0xFFFF);
	packet.Add(moveFrom);
	packet.Add(fromPos);
	packet.Add(itemId);
	packet.Add(fromPos);
	packet.Add<short>(0xFFFF);
	packet.Add(moveTo);
	packet.Add(toPos);
	packet.Add(count);
	packet.PrepareForBattle();
	CPacket::SendPacket(packet.Buffer());
}

void CPacket::Attack(int targetID)
{
	Packet<7> packet;
	packet.Add<char>(0xA1);
	packet.Add(targetID);
	packet.PrepareForBattle();
	CPacket::SendPacket(packet.Buffer());
}

void CPacket::Move(int move)
{
	Packet<3> packet;
	packet.Add<char>(0x65 + move);
	packet.PrepareForBattle();
	CPacket::SendPacket(packet.Buffer());
}