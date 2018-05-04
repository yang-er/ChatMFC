#include "stdafx.h"
#include <winsock2.h>
#include "datagram.h"
#include "context.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int id = 0;

CChatClientContext::CChatClientContext(LPVOID context, SOCKET sock, const sockaddr_in &in)
{
	Window = ((CChatServerContext*)context)->Window;
	ClientSocket = sock;
	ClientAddr = in;
	ID = id++;
	Exited = FALSE;
	wcscpy_s(Password, ((CChatServerContext*)context)->Password);
}

UINT CChatServerContext::ServerFunc(LPVOID param)
{
	CChatServerContext *context = (CChatServerContext*)param;

	WSADATA  Ws;
	SOCKET &ServerSocket = context->ServerSocket;
	struct sockaddr_in LocalAddr, ClientAddr;
	int Ret = 0;
	int AddrLen = 0;

	__try
	{
		if (WSAStartup(MAKEWORD(2, 2), &Ws) != 0)
		{
			context->Log(2, L"Init Windows Socket Failed", GetLastError());
			return -1;
		}

		ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ServerSocket == INVALID_SOCKET)
		{
			context->Log(2, L"Create Socket Failed", GetLastError());
			return -1;
		}

		memset(LocalAddr.sin_zero, 0x00, 8);
		LocalAddr.sin_family = AF_INET;
		LocalAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
		LocalAddr.sin_port = htons(SERVER_PORT);

		Ret = bind(ServerSocket, (struct sockaddr*)&LocalAddr, sizeof(LocalAddr));
		if (Ret != 0)
		{
			context->Log(2, L"Bind Socket Failed", GetLastError());
			return -1;
		}

		Ret = listen(ServerSocket, 10);
		if (Ret != 0)
		{
			context->Log(2, L"Listen Socket Failed", GetLastError());
			return -1;
		}

		context->Log(0, L"Server launched");

		while (true)
		{
			AddrLen = sizeof(ClientAddr);
			SOCKET ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddr, &AddrLen);
			if (ClientSocket == INVALID_SOCKET)
			{
				DWORD lastErr = GetLastError();
				if (lastErr != 0x2714)
					context->Log(1, L"Accept Failed", lastErr);
				break;
			}

			context->Log(1, L"Client connected :: ", ClientAddr);
			context->ActivateClient(ClientSocket, ClientAddr);
		}
	}
	__finally
	{
		if (ServerSocket != 3435973836)
			closesocket(ServerSocket);
		for (size_t i = 0; i < context->ThreadPool.size(); i++)
		{
			if (context->ThreadPool[i]->Exited == FALSE)
			{
				closesocket(context->ThreadPool[i]->ClientSocket);
				context->ThreadPool[i]->ClientThread->ExitInstance();
			}
		}
		context->ThreadPool.clear();
		WSACleanup();
		context->Log(2, L"Server exit");
	}

	return 0;
}

void CChatServerContext::ActivateClient(SOCKET sock, sockaddr_in in)
{
	CChatClientContext *client = new CChatClientContext(this, sock, in);
	client->ClientThread = AfxBeginThread(ClientFunc, client);
	client->parent = this;
	ThreadPool.push_back(client);
}

void send_keepalive(CChatClientContext *context);
int recv_keepalive(CChatClientContext *context);
int parse_message(CChatClientContext *context);

UINT CChatServerContext::ClientFunc(LPVOID param)
{
	CChatClientContext *context = (CChatClientContext*)param;
	SOCKET &ClientSocket = context->ClientSocket;
	int Ret = 0;

	__try
	{
		while (true)
		{
			if (parse_message(context) == -1) return 0;
		}
	}
	__finally
	{
		closesocket(ClientSocket);
		context->Exited = TRUE;
		context->parent->Log(4, context);
		for (int i = 0; i < context->parent->ThreadPool.size(); i++)
		{
			if (context->parent->ThreadPool[i]->Exited == FALSE)
			{
				data_gram *dg = new data_gram;
				memset(dg, 0x00, sizeof(data_gram));
				dg->version = DATAGRAM_LOGOUT;
				dg->exchange.to = context->ID;
				context->parent->ThreadPool[i]->MessageQueue.push(*dg);
			}
		}
	}
	return 0;
}

void send_keepalive(CChatClientContext *context)
{
	data_gram dg, tmp;
	dg.version = DATAGRAM_KEEPALIVE;
	dg.keepalive.next_messages = context->MessageQueue.size();
	dg.keepalive.id = context->ID;
	send(context->ClientSocket, dg.data, sizeof(dg), 0);

	while (!context->MessageQueue.empty())
	{
		send(context->ClientSocket, context->MessageQueue.front().data, sizeof(dg), 0);
		tmp = context->MessageQueue.front();
		context->MessageQueue.pop();
	}
}

int recv_keepalive(CChatClientContext *context)
{
	data_gram recvVal;
	memset(recvVal.data, 0x00, sizeof(recvVal));

	int ret = recv(context->ClientSocket, recvVal.data, sizeof(recvVal), 0);
	if (ret == 0 || ret == SOCKET_ERROR)
	{
		context->Log(1, L"Client exit");
		return -1;
	}

	ret = recvVal.keepalive.next_messages;
	for (int i = 0; i < ret; i++)
	{
		memset(recvVal.data, 0x00, sizeof(recvVal));
		context->parent->ThreadPool[recvVal.exchange.to]->MessageQueue.add(recvVal);
	}

	return ret;
}

int parse_message(CChatClientContext *context)
{
	data_gram RecvValue;
	memset(RecvValue.data, 0x00, sizeof(RecvValue));

	int ret = recv(context->ClientSocket, RecvValue.data, sizeof(RecvValue), 0);
	if (ret == 0 || ret == SOCKET_ERROR)
	{
		context->Log(1, L"Client exit");
		return -1;
	}

	switch (RecvValue.version)
	{
	case DATAGRAM_LOGIN:
		if (wcscmp(RecvValue.challenge.md5_password, context->Password))
		{
			context->Log(1, L"Password wrong... exit");
			data_gram SendValue;
			SendValue.version = DATAGRAM_LOGOUT;
			wcscpy_s(SendValue.exchange.message, L"Password is wrong.");
			send(context->ClientSocket, SendValue.data, sizeof(SendValue), 0);
			closesocket(context->ClientSocket);
			return -1;
		}
		else
		{
			context->Log(1, L"Welcome ", RecvValue.challenge.nickname);
			wcscpy_s(context->Nickname, RecvValue.challenge.nickname);
			context->parent->Log(3, context);
			for (int i = 0; i < context->parent->ThreadPool.size() - 1; i++)
			{
				if (context->parent->ThreadPool[i]->Exited == FALSE)
				{
					data_gram *dg = new data_gram;
					memset(dg, 0x00, sizeof(data_gram));
					dg->version = DATAGRAM_CHALLENGE;
					dg->exchange.to = context->ID;
					wcscpy_s(dg->exchange.message, context->Nickname);
					context->parent->ThreadPool[i]->MessageQueue.push(*dg);

					dg = new data_gram;
					memset(dg, 0x00, sizeof(data_gram));
					dg->version = DATAGRAM_CHALLENGE;
					dg->exchange.to = context->parent->ThreadPool[i]->ID;
					wcscpy_s(dg->exchange.message, context->parent->ThreadPool[i]->Nickname);
					context->MessageQueue.push(*dg);
				}
			}
		}
		break;

	case DATAGRAM_LOGOUT:
		context->Log(1, L"Logging out ", context->Nickname);
		context->parent->Log(4, context);
		return -1;
		break;

	case DATAGRAM_KEEPALIVE:
		ret = RecvValue.keepalive.next_messages;
		for (int i = 0; i < ret; i++)
			parse_message(context);
		break;

	case DATAGRAM_EXCHANGE:
		if (RecvValue.exchange.to == -1)
		{
			context->Log(1, L"Client message: ", RecvValue.exchange.message);
		}
		else
		{
			int to = RecvValue.exchange.to;
			RecvValue.exchange.to = context->ID;
			context->parent->ThreadPool[to]->MessageQueue.add(RecvValue);
		}
		break;

	default:
		break;
	}

	send_keepalive(context);
	return 0;
}
