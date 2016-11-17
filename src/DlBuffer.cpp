
#include "DlBuffer.h"
#include <string.h>


static const int MAX_BUFFER_SIZE=3850240*10;


CDlBuffer::CDlBuffer(void)
{
	m_nBufferSize = 3850240*2;
	m_pBuffer = new BYTE[m_nBufferSize];
	m_nDataLen = 0;
	m_bError = false;
}

CDlBuffer::~CDlBuffer(void)
{
	delete [] m_pBuffer;
	m_nDataLen = 0;
	m_nBufferSize = 0;
}

bool CDlBuffer::Append(BYTE* pData,int nLen)
{
	if (m_bError)
	{
		return false;
	}

	if (m_nBufferSize - m_nDataLen < nLen)
	{
		//resize or error out
		int newsize = m_nBufferSize+3850240*2;
		if (nLen > 3850240*2)
		{
			newsize = m_nBufferSize + nLen;
		}
		if (newsize >= MAX_BUFFER_SIZE)
		{
			m_bError = true;
			cout <<"hls segment too big,ignored" << endl;
			return false;
		}
	

		BYTE* p_tmp = new BYTE[newsize];
		memcpy(p_tmp,m_pBuffer,m_nDataLen);
		memcpy(p_tmp+m_nDataLen,pData,nLen);
		delete [] m_pBuffer;
		m_pBuffer = p_tmp;
		m_nBufferSize = newsize;
		m_nDataLen += nLen;
	}
	else
	{
		memcpy(m_pBuffer+m_nDataLen,pData,nLen);
		m_nDataLen += nLen;
	}
	return true;
}

void CDlBuffer::ResetData()
{
	m_nDataLen = 0;
	m_bError = false;
}

