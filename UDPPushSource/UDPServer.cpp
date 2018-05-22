#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>
#include <thread>
#include "UDPServer.h"
#include <wxdebug.h>
#include "LiveSource.h"
 
using namespace std;

extern CLiveSourceStream* g_pOutputPin;

CUDPServer::~CUDPServer()
{

	m_running = false;
	if (Fifo != NULL)
	{
		delete Fifo;
		Fifo = NULL;
	}
}

void CUDPServer::ReceiveThread()
{
	int SelectTiming;
	while (m_running)
	{
		SelectTiming = recvfromTimeOutUDP(ReceivingSocket, 1, 0);
		ProcessMessage(SelectTiming);
	}

}


void CUDPServer::FrameThread()
{
	int size;
	while (m_running)
	{
		if ((size = Dequeue()) > 0)
			g_pOutputPin->AddFrame((BYTE *)FrameBuf, size);

	}

}


int CUDPServer::GetFifoSize()
{

	if (m_writeIndex == m_readIndex)
		return 0;

	if (m_writeIndex > m_readIndex)
		return m_writeIndex - m_readIndex;

	return (FIFO_SIZE - m_readIndex) + m_writeIndex;
}

void CUDPServer::Enqueue(uint8_t *buffer, int size)
{
	int empty = FIFO_SIZE - GetFifoSize();
	while (empty < size)
	{
		Sleep(2);
	}

	if (m_writeIndex >= m_readIndex)
	{
		memcpy(Fifo + m_writeIndex, buffer, size);
		m_writeIndex += size;
	}
	else {
		printf("f");
	}
}

int CUDPServer::Dequeue()
{
	int size = GetFifoSize();
	if (size == 0)
	{
		return 0;
	}

	if (m_writeIndex > m_readIndex)
	{
		memcpy(FrameBuf , Fifo + m_readIndex, size);
		m_readIndex += size;
	}
	else {
		printf("f");
	}
	return size;
}


void CUDPServer::ProcessMessage(int SelectTiming)
{
	SOCKADDR_IN        SenderAddr;
	SenderAddrSize = sizeof(SenderAddr);

	switch (SelectTiming)
	{
		case 0:
			// Timed out, do whatever you want to handle this situation
			printf("Server: Timeout lor while waiting you bastard client!...\n");
		break;
		case -1:
			// Error occurred, maybe we should display an error message?
			// Need more tweaking here and the recvfromTimeOutUDP()...
			printf("Server: Some error encountered with code number: %ld\n", WSAGetLastError());
		break;
		default:
		{
			   while (m_running)
			   {

				   // Call recvfrom() to get it then display the received data...
				   ByteReceived = ::recvfrom(ReceivingSocket, (char *)ReceiveBuf, RECEIVE_SIZE, 0, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
				   if (ByteReceived > 0)
				   {				 
					 
#if USE_FIFO
					   if (GetFifoSize() < FIFO_SIZE)
					   {
						   Enqueue(ReceiveBuf, ByteReceived);
					   }
#else 
					   g_pOutputPin->AddFrame((BYTE *)ReceiveBuf, ByteReceived);
#endif 
					  

				   }
				   else if (ByteReceived <= 0)
				   {
					   PrintError("Server: Connection closed with error code: \n");
				   }
				   else
					   PrintError("Server: recvfrom() failed with error code: \n");
				 
			  }
		}

	}
}

// A sample of the select() return value

int CUDPServer::recvfromTimeOutUDP(SOCKET socket, long sec, long usec)

{

	// Setup timeval variable

	struct timeval timeout;

	struct fd_set fds;



	timeout.tv_sec = sec;

	timeout.tv_usec = usec;

	// Setup fd_set structure

	FD_ZERO(&fds);

	FD_SET(socket, &fds);

	// Return value:

	// -1: error occurred

	// 0: timed out

	// > 0: data ready to be read

	return ::select(0, &fds, 0, 0, &timeout);

}

const wchar_t *CUDPServer::GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

void CUDPServer::PrintError(const char *msg)
{

	char buffer[100];
	sprintf(buffer, "%s %d", msg, WSAGetLastError());

	DbgLog((LOG_TRACE, 3, buffer));
}

int CUDPServer::Setup(bool allInterface)
{

	// Create a new socket to receive datagrams on.
	ReceivingSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ReceivingSocket == INVALID_SOCKET)
	{
		printf("Server: Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// The IPv4 family
	SOCKADDR_IN        ReceiverAddr;

	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(m_port);
	
	DWORD ip = inet_addr(m_ipAddress); 
	if (allInterface == false)
		ReceiverAddr.sin_addr.s_addr = htonl(ip);
	else 
		ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

 
	int res = ::bind(ReceivingSocket, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr));
	if (res == SOCKET_ERROR)
	{
 		 
		char string[25];
		itoa(WSAGetLastError(), string, 10);
		char msg[100];
		sprintf(msg, "Server bind error %d", WSAGetLastError());

	 
		::MessageBox(NULL, GetWC(msg), L"UDP Source filter", 0);
		// Close the socket
		closesocket(ReceivingSocket);
		// Do the clean up
		WSACleanup();
		// and exit with error
		return -1;
	}
		
	// Some info on the receiver side...
	getsockname(ReceivingSocket, (SOCKADDR *)&ReceiverAddr, (int *)sizeof(ReceiverAddr));
	 

	printf("Server: Receiving IP(s) used: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
	printf("Server: Receiving port used: %d\n", htons(ReceiverAddr.sin_port));
	printf("Server: I\'m ready to receive a datagram...\n");
	return 1;

}

void CUDPServer::Pause()
{

}
void CUDPServer::Stop()
{
	m_running = false;
		 
}
void CUDPServer::Start()
{
	if (m_running == true)
		return;

	m_running = true;
	m_receiveThread = make_shared<thread>(&CUDPServer::ReceiveThread, this);
#if USE_FIFO
	m_frameThread = make_shared<thread>(&CUDPServer::FrameThread, this);
#endif 
}