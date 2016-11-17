#pragma once
#include "define.h"


///不能多线程调用
class CDlBuffer
{
public:
	CDlBuffer(void);
	~CDlBuffer(void);
	bool Append(BYTE* pData,int nLen);
	void ResetData();

public:
	BYTE* m_pBuffer;
	int m_nBufferSize;
	bool m_bError;
	int m_nDataLen;
private:
	
	
};
