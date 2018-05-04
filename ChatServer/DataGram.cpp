#include "stdafx.h"
#include "datagram.h"
#include <cstring>

const data_gram KeepAlivePacket()
{
	static data_gram keep_alive;
	if (keep_alive.version != DATAGRAM_KEEPALIVE)
	{
		memset(keep_alive.data, 0, sizeof(data_gram));
		keep_alive.version = DATAGRAM_KEEPALIVE;
	}
	return keep_alive;
}

void LoginPacket(data_gram &ret, LPCWSTR pswd, LPCWSTR nick)
{
	memset(ret.data, 0x00, sizeof(data_gram));
	ret.version = DATAGRAM_LOGIN;
	wcscpy_s(ret.challenge.md5_password, pswd);
	wcscpy_s(ret.challenge.nickname, nick);
}

void msgqueue::add(const data_gram &data)
{
	data_gram *newdg = new data_gram;
	memcpy(newdg, &data, sizeof(data));
	push(*newdg);
}
