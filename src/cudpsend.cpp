#include "cudpsend.h"
#include "cudpobj.h"
#include <sys/time.h>
#include <errno.h>

#define TS_PACKET_LEN     188
#define TS_NUMBER_IN_UDP  7
#define UDP_PACKET_SIZE  (TS_PACKET_LEN*TS_NUMBER_IN_UDP)
#define TS_BUFFERED_COUNT 1000 // buffered TS_BUFFERED_COUNT ts before send datas
#define TS_SEND_BUFFER_SIZE    20000 // must great than 2*TS_NUMBER_IN_UDP

CUdpSend::CUdpSend(void)
{
	//m_sock = -1;
	m_pbuf = NULL;
	m_pbuf_pos = 0;
	m_pdeque = NULL;
	m_ptsPacket = NULL;
	m_pcr = -1;
        m_pUdpObj = new CUdpObj();
}

CUdpSend::~CUdpSend(void)
{
	//if (m_sock != -1)
	//{
	//	closesocket(m_sock);
	//}
        delete m_pUdpObj;

	if (m_pdeque != NULL) 
	{
		delete m_pdeque;
	}

	if (m_pbuf != NULL) 
	{
		delete[] m_pbuf;
	}

	if (m_ptsPacket != NULL)
	{
		delete m_ptsPacket;
	}
}

int CUdpSend::InitSend(const string& sendip,int sendport,const string& loacladdr)
{
	m_pdeque = new CSafeDeque(TS_PACKET_LEN, TS_SEND_BUFFER_SIZE);
	if (NULL == m_pdeque)
	{
		cout << "new CSafeDeque failed." << errno<< endl;
		return -1;
	}

	m_pbuf = new uint8_t[TS_PACKET_LEN * TS_SEND_BUFFER_SIZE];
	if (NULL == m_pbuf)
	{
		cout << "new m_pbuf failed." << errno<< endl;
		return -1;
	}

	m_ptsPacket = new CTsPacket();
	if (NULL == m_ptsPacket)
	{
		cout << "new m_ptsPacket failed." << errno<< endl;
		return -1;
	}

        //init sock
        UDP_OBJ_PARAM_T param;
        param.emObjType = OBJ_UDP_SERVER;
        param.ipAddr = sendip;
        param.localAddr = loacladdr;
        param.port = sendport;
        m_pUdpObj->SetParam(param);
        
        if (m_pUdpObj->CreateObj() < 0)
        {
                cout << "Udp socket init faild!" << endl;
                return -1;
        }

 /*
	if((m_sock = socket(AF_INET,SOCK_DGRAM, 0)) == -1)
	{
		err = GetLastError();
		cout << "SendData socket Init failed." << errno<< endl;
		return -1;
	}

	m_s_addr.sin_family = AF_INET;
	m_s_addr.sin_port = htons(sendport);
	m_s_addr.sin_addr.s_addr = inet_addr(sendip.c_str());
*/

        pthread_create(&m_hWorkThread,NULL,WorkThread,this);

	return 0;
}

void* CUdpSend::WorkThread(void* lpParam)
{
	CUdpSend* lpthis = (CUdpSend*)lpParam;
    lpthis->WorkFun();

	return 0;
}

void CUdpSend::WaitForBuffered()
{
	while (1)
	{
		if (m_pdeque->GetDataSize() >= TS_BUFFERED_COUNT)
		{
			break;
		}
		else
		{
			usleep(5000);
		}
	}
}

int CUdpSend::WorkFun()
{
	struct timeval tv_now;
	uint8_t *p_ts = NULL;

	WaitForBuffered();

	while (1)
	{
		if (m_pdeque->Epmty())
		{
			usleep(5000);
			continue;
		}

		while (!m_pdeque->Epmty())
		{
			if (m_pbuf_pos < TS_PACKET_LEN * TS_SEND_BUFFER_SIZE)
			{
				p_ts = m_pdeque->PopFront();
				memcpy(m_pbuf + m_pbuf_pos, p_ts, TS_PACKET_LEN);
				m_pdeque->ReleaseBuffer(p_ts);

				m_ptsPacket->SetPacket(m_pbuf + m_pbuf_pos);

				m_pbuf_pos += TS_PACKET_LEN;
			}
			else 
			{
				cout << "发送缓冲满" << endl;
				continue;
			}

			if (m_pbuf_pos < UDP_PACKET_SIZE)
			{
				continue;
			}
			if (m_ptsPacket->Get_PCR_flag())
			{
				// 初始化PCR和系统时钟采样
				if (-1 == m_pcr)
				{
					gettimeofday(&tv_now,NULL);
					m_pcr = m_ptsPacket->Get_PCR();
					m_sys_clock = (long long)tv_now.tv_sec * 1000 + (long long)tv_now.tv_usec / 1000;
					
					continue;
				}

				while (1) 
				{
					gettimeofday(&tv_now,NULL);
					long long cur_pcr = m_ptsPacket->Get_PCR();
					long long cur_sys_clock = (long long)tv_now.tv_sec * 1000 + (long long)tv_now.tv_usec / 1000;

					// PCR变小，重新设置采样
					if (cur_pcr < m_pcr) 
					{
						m_pcr = cur_pcr;
						m_sys_clock = cur_sys_clock;
					}

					if (cur_pcr == m_pcr ||
						(cur_pcr - m_pcr)/27000-80 <= cur_sys_clock - m_sys_clock)//提前80ms发送
					{
						// 发送数据
						int send_pos = 0;
						while (m_pbuf_pos - send_pos >= UDP_PACKET_SIZE)
						{
							Send(m_pbuf + send_pos);
							send_pos += UDP_PACKET_SIZE;
						}

						// 移动剩余的小于7的ts包
						if (m_pbuf_pos > send_pos)
						{
							memmove(m_pbuf, m_pbuf + send_pos, m_pbuf_pos - send_pos);
						}

						m_pbuf_pos -= send_pos;
						break;
					}
					else
					{
						usleep(1000);
					}
				}
			}
		}
	}

	return 0;
}

int CUdpSend::PushData(uint8_t *buf, int buf_size)
{
	int ret;

	if (buf_size % TS_PACKET_LEN != 0)
	{
		cout << "TS 包长不正确，抛弃数据" << endl;
		return -1;
	}

	for (int i = 0; i < buf_size; i += TS_PACKET_LEN)
	{
		ret = m_pdeque->PushBack(buf + i);
		if (ret != 0)
		{
			cout << "发送缓冲上溢" << endl;
		}
	}

    return 0;
}

int CUdpSend::Send(uint8_t *buf)
{
	int send_len;
        const UDP_OBJ_ATTRIBUTE_T* pAttr = m_pUdpObj->GetObjAttr();
	send_len = sendto(pAttr->socket, (char*)buf, TS_PACKET_LEN * TS_NUMBER_IN_UDP, 0, (struct sockaddr *)&m_s_addr, sizeof(m_s_addr));

	if (send_len < UDP_PACKET_SIZE) 
	{
		cout << "Send data failed." << errno<< endl;
		return -1;
	}

	return 0;
}

