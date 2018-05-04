#include "stdafx.h"
#include "LoginDialog.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "../ChatServer/datagram.h"

void send_messages(CChatClientApp *App)
{
	if (App->MessageQueue.empty())
	{
		data_gram dg;
		memset(dg.data, 0x00, sizeof(data_gram));
		dg.keepalive.id = App->CurrentId;
		dg.keepalive.next_messages = App->MessageQueue.size();
		App->MessageQueue.push(dg);
	}

	while (!App->MessageQueue.empty())
	{
		int Ret = send(App->ClientSocket, App->MessageQueue.front().data, sizeof(data_gram), 0);
		App->MessageQueue.pop();
		if (Ret == SOCKET_ERROR)
		{
			DWORD err = GetLastError();
			if (err == 0x2746)
			{
				App->Log(2, L"Server exit");
				App->Thread->ExitInstance();
			}
			else
			{
				App->Log(2, L"Recv Info Error", err);
			}
		}
	}
}

void parse_messages(CChatClientApp *App)
{
	data_gram RecvBuffer;
	memset(RecvBuffer.data, 0x00, sizeof(RecvBuffer));
	int Ret = recv(App->ClientSocket, RecvBuffer.data, sizeof(RecvBuffer), 0);
	if (Ret == SOCKET_ERROR)
	{
		DWORD err = GetLastError();
		if (err == 0x2746)
		{
			App->Log(2, L"Server exit");
			App->Thread->ExitInstance();
		}
		else
		{
			App->Log(2, L"Recv Info Error", err);
		}
	}

	wchar_t logsss[100];
	switch (RecvBuffer.version)
	{
	case DATAGRAM_CHALLENGE:
		swprintf_s(logsss, 100, L"%d : %s logged in.", RecvBuffer.exchange.to, RecvBuffer.exchange.message);
		App->Log(5, logsss);
		break;

	case DATAGRAM_EXCHANGE:
		swprintf_s(logsss, 100, L"%d said : %s", RecvBuffer.exchange.to, RecvBuffer.exchange.message);
		App->Log(5, logsss);
		break;

	case DATAGRAM_KEEPALIVE:
		App->CurrentId = RecvBuffer.keepalive.id;
		for (int i = 0; i < RecvBuffer.keepalive.next_messages; i++)
		{
			parse_messages(App);
		}
		break;

	case DATAGRAM_LOGOUT:
		if (RecvBuffer.exchange.to == App->CurrentId)
		{
			App->Log(2, L"Logout with Error ", RecvBuffer.exchange.message);
			App->ExitInstance();
		}
		else
		{
			swprintf_s(logsss, 100, L"%d logged out.", RecvBuffer.exchange.to);
			App->Log(5, logsss);
		}
		break;

	default:
		break;
	}
}

UINT CChatClientApp::ClientThread(LPVOID param)
{
	CChatClientApp *App = (CChatClientApp*)param;
	WSADATA Ws;
	SOCKET &ClientSocket = App->ClientSocket;
	struct sockaddr_in ServerAddr;
	int Ret = 0;
	int AddrLen = 0;
	data_gram RecvBuffer;

	__try
	{
		if (WSAStartup(MAKEWORD(2, 2), &Ws) != 0)
		{
			App->Log(2, L"Init Windows Socket Failed", GetLastError());
			return -1;
		}

		ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ClientSocket == INVALID_SOCKET)
		{
			App->Log(2, L"Create Socket Failed", GetLastError());
			return -1;
		}

		ServerAddr.sin_family = AF_INET;
		union ip_got { unsigned char list[4]; DWORD p; } wtf;
		wtf.p = App->TargetIP;
		char ipaddr[16];
		sprintf_s(ipaddr, "%u.%u.%u.%u", wtf.list[3], wtf.list[2], wtf.list[1], wtf.list[0]);
		ServerAddr.sin_addr.s_addr = inet_addr(ipaddr);
		ServerAddr.sin_port = htons(SERVER_PORT);
		memset(ServerAddr.sin_zero, 0x00, 8);

		Ret = connect(ClientSocket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
		if (Ret == SOCKET_ERROR)
		{
			App->Log(2, L"Connect Error", GetLastError());
			return -1;
		}
		else
		{
			App->Log(0, L"Connect Succeeded");
		}

		LoginPacket(RecvBuffer, App->Password, App->Nickname);
		App->MessageQueue.push(RecvBuffer);

		while (true)
		{
			send_messages(App);
			parse_messages(App);
			Sleep(2000);
		}
	}
	__finally
	{
		closesocket(ClientSocket);
		WSACleanup();
	}

	return 0;
}
