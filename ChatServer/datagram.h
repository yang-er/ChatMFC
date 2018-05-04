#pragma once
#include <WinSock2.h>
#include <queue>

#define SERVER_PORT 4000
#define DATAGRAM_LOGIN 1
#define DATAGRAM_KEEPALIVE 2
#define DATAGRAM_LOGOUT 4
#define DATAGRAM_CHALLENGE 8
#define DATAGRAM_EXCHANGE 16

struct data_challenge
{
	short version;
	wchar_t md5_password[17];
	wchar_t nickname[21];
	IN_ADDR ip;
};

struct data_exchange
{
	short version;
	int to;
	wchar_t message[100];
};

struct data_keepalive
{
	short version;
	int next_messages;
	int id;
};

typedef union
{
	short version;
	data_challenge challenge;
	data_exchange exchange;
	data_keepalive keepalive;
	char data[MAX_PATH];
} data_gram;

class msgqueue : public std::queue<data_gram>
{
public:
	void add(const data_gram &);
};

const data_gram KeepAlivePacket();
void LoginPacket(data_gram &ret, LPCWSTR pswd, LPCWSTR nick);
