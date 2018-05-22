#pragma once
#include <thread>
#include <memory>
using namespace std;

#define FIFO_SIZE 1024 * 1024 * 10


class CUDPServer
{
public:
	CUDPServer(const char *IpAddress, int port) : m_port(port)
	{
		Init();
		strcpy(m_ipAddress, IpAddress);
#if USE_FIFO
		Fifo = (uint8_t *)malloc(FIFO_SIZE);
#endif 
		m_writeIndex = 0;
		m_readIndex = 0;

	}

	CUDPServer(int port) : m_port(port)
	{
		Init();

	}
	~CUDPServer();

	int Setup(bool allInterface);
	void Start();
	void Stop();
	void Pause();

#define RECEIVE_SIZE 10 * 1024
	
private:
	WSADATA			   wsaData;
	char			   m_ipAddress[100];
	SOCKET             ReceivingSocket;
	int                m_port;
	uint8_t			   ReceiveBuf[RECEIVE_SIZE];
	uint8_t			   FrameBuf[RECEIVE_SIZE];
	int                BufLength;
	int                SenderAddrSize;
	int                ByteReceived;	
	int				   ErrorCode;
	bool			   m_running;

	int				   m_writeIndex;
	int				   m_readIndex;
	uint8_t			   *Fifo;

	shared_ptr<thread> m_receiveThread;
	shared_ptr<thread> m_frameThread;

private:
	void ReceiveThread();
	void FrameThread();
	int recvfromTimeOutUDP(SOCKET socket, long sec, long usec);
	void PrintError(const char *msg);
	const wchar_t *GetWC(const char *c);
	void ProcessMessage(int SelectTiming);

	void Init()
	{
		
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		m_running = false;
		BufLength = RECEIVE_SIZE;
	}

	void Enqueue(uint8_t *buffer, int size);
	int Dequeue();

	int GetFifoSize();

};

