
#include "SafeDeque.h"

CSafeDeque::CSafeDeque(int nUnitSize,int nCount)
{
	m_nUnitSize = nUnitSize;
	m_nUnitCount = nCount;
	m_pOutBuffer = new uint8_t[nUnitSize*nCount];
	memset(m_pOutBuffer,0,nUnitSize*nCount);
	for (int i = 0; i < nCount; i++)
	{
		m_dqFreeBuffer.push_back(m_pOutBuffer+i*nUnitSize);
	}

	pthread_mutex_init(&m_mutex,NULL);

	m_emOverflowTreatment = OVERFLOW_DROP;
}

CSafeDeque::~CSafeDeque(void)
{
	pthread_mutex_destroy(&m_mutex);
	delete [] m_pOutBuffer;
}

void CSafeDeque::SetOverFlowTreatment( OVERFLOW_TREATMENT emType)
{
	m_emOverflowTreatment = emType;
}

uint8_t* CSafeDeque::PopFront()
{
	uint8_t* pData = NULL;

	pthread_mutex_lock(&m_mutex);
	if (!m_dqData.empty())
	{
		pData = m_dqData[0];
		m_dqData.pop_front();
	}
	pthread_mutex_unlock(&m_mutex);

	return pData;
}

void CSafeDeque::ReleaseBuffer(uint8_t* pBuffer)
{
	pthread_mutex_lock(&m_mutex);
	m_dqFreeBuffer.push_back(pBuffer);
	pthread_mutex_unlock(&m_mutex);
}

int CSafeDeque::PushBack(uint8_t* pBuffer)
{
	uint8_t* pBuf = NULL;
	int ret = 0;

	pthread_mutex_lock(&m_mutex);
	if (!m_dqFreeBuffer.empty())
	{
		pBuf = m_dqFreeBuffer[0];
		m_dqFreeBuffer.pop_front();
		m_dqData.push_back(pBuf);
		memcpy(pBuf,pBuffer,m_nUnitSize);		
	}
	else if (m_emOverflowTreatment == OVERFLOW_DROP)
	{
		pBuf = m_dqData[0];
		m_dqData.pop_front();
		m_dqData.push_back(pBuf);
		memcpy(pBuf,pBuffer,m_nUnitSize);
		ret = 1;
	}
	else
	{
		ret = 2;
	}
	pthread_mutex_unlock(&m_mutex);

	return ret;
}

const deque<uint8_t*>& CSafeDeque::LockDataDq()
{
	pthread_mutex_lock(&m_mutex);
	return m_dqData;
}

void CSafeDeque::UnlockDataDq()
{
	pthread_mutex_unlock(&m_mutex);
}

bool CSafeDeque::Epmty()
{
	bool ret;

	pthread_mutex_lock(&m_mutex);
	ret = m_dqData.empty();
	pthread_mutex_unlock(&m_mutex);

	return ret;
}


int CSafeDeque::GetDataSize()
{
	int size;

	pthread_mutex_lock(&m_mutex);
	size = m_dqData.size();
	pthread_mutex_unlock(&m_mutex);

	return size;
}


int CSafeDeque::GetDqSize()
{
	return m_nUnitCount;
}
