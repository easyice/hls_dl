#ifndef CUDPSEND_H
#define CUDPSEND_H

#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>
#include <iostream>
#include <pthread.h>
#include "SafeDeque.h"
#include "define.h"
#include "tspacket.h"
using namespace std;



class CUdpObj;
class CUdpSend
{
public:
	CUdpSend(void);
	~CUdpSend(void);

	////////////////////////////////////////////Íâ½çµ÷ÓÃ//////////////////////////////////////////////
	int InitSend(const string& sendip,int sendport,const string& loacladdr);


	////////////////////////////////////////////Encoderµ÷ÓÃ//////////////////////////////////////////////
	int PushData(uint8_t *buf, int buf_size);

private:
	static void* WorkThread(void* lpParam);
	int WorkFun();
	int Send(uint8_t *buf);

	void WaitForBuffered();
/*
	static inline void gettimeofday(struct timeval *tv)
	{
	  struct _timeb timebuffer;
	  _ftime(&timebuffer);
	  tv->tv_sec  = (long)timebuffer.time;
	  tv->tv_usec = timebuffer.millitm * 1000;
	}
*/
private:
	struct sockaddr_in  m_s_addr;
	//int                 m_sock;
        CUdpObj* m_pUdpObj;
	pthread_t              m_hWorkThread;

	uint8_t      *m_pbuf;
	int           m_pbuf_pos;
	CSafeDeque   *m_pdeque;
	CTsPacket    *m_ptsPacket;

	long long    m_pcr;
	long long    m_sys_clock;
};

#endif // CUDPSEND_H