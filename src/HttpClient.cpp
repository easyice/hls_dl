#include "HttpClient.h"
#include "DlBuffer.h"

extern "C"
{
	#include <curl/curl.h>
}


#define CONN_TIME_OUT 30
#define MAX_TIME_OUT 30


CHttpClient::CHttpClient(void)
{
	m_bDebug = false;
	m_pDlBuffer = new CDlBuffer();
}

CHttpClient::~CHttpClient(void)
{
	delete m_pDlBuffer;
}

void CHttpClient::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if(itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	return 0;
}


int CHttpClient::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRecvString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Get(const std::string & strUrl, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRecvString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
        
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);  
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONN_TIME_OUT);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, MAX_TIME_OUT);
        
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
        
	res = curl_easy_perform(curl);
        
        int response_code = -1;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        
        char *pbuff = NULL;
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL,&pbuff);//获取最终请求的url地址
        m_strEffective_url = pbuff;
        
	curl_easy_cleanup(curl);
        
        if (response_code != 200)
        {
                printf("response error:%d\n",response_code);
                return -1001;
        }
        if (res != CURLE_OK)
        {
                printf("curl_easy_perform:%d\n",res);
        }
	return res;
}

int CHttpClient::Get(const std::string & strUrl, BYTE** pData,int& nLen)
{
	m_pDlBuffer->ResetData();

	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRecvData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONN_TIME_OUT);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, MAX_TIME_OUT);
        
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);  
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
        
	res = curl_easy_perform(curl);
        
        int response_code = -1;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        
        curl_easy_cleanup(curl);

        if (m_pDlBuffer->m_bError)
        {
                nLen = 0;
                return -1000;
        }
        if (response_code != 200)
        {
                printf("response error:%d\n",response_code);
                nLen = 0;
                return -1001;
        }
        if (res != CURLE_OK)
        {
                printf("curl_easy_perform:%d\n",res);
                return res;
        }
        
        *pData = m_pDlBuffer->m_pBuffer;
        nLen = m_pDlBuffer->m_nDataLen;

        
        return res;
}

int CHttpClient::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRecvString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRecvString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);  
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONN_TIME_OUT);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, MAX_TIME_OUT);
	res = curl_easy_perform(curl);
        
	int response_code = -1;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        
        curl_easy_cleanup(curl);
        
        if (response_code != 200)
        {
                printf("response error:%d\n",response_code);
                return -1001;
        }
        if (res != CURLE_OK)
        {
                printf("curl_easy_perform:%d\n",res);
        }
        return res;
}

int CHttpClient::Gets(const std::string & strUrl, BYTE** pData,int& nLen, const char * pCaPath )
{
	m_pDlBuffer->ResetData();

	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRecvData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);  
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONN_TIME_OUT);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, MAX_TIME_OUT);
	res = curl_easy_perform(curl);
        
        int response_code = -1;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        
	curl_easy_cleanup(curl);

	if (m_pDlBuffer->m_bError)
	{
		nLen = 0;
		return -1000;
	}
	if (response_code != 200)
        {
                printf("response error:%d\n",response_code);
                nLen = 0;
                return -1001;
        }
        if (res != CURLE_OK)
        {
                printf("curl_easy_perform:%d\n",res);
                return res;
        }
	
	*pData = m_pDlBuffer->m_pBuffer;
	nLen = m_pDlBuffer->m_nDataLen;

        
	return res;
}


size_t CHttpClient::OnRecvData(void *buffer, size_t size, size_t nmemb, void *stream)
{
	CHttpClient *lpthis = (CHttpClient*)stream;

	lpthis->m_pDlBuffer->Append((BYTE*)buffer,size*nmemb);
	return nmemb;
}

size_t CHttpClient::OnRecvString(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
	return nmemb;
}

int CHttpClient::GetBin(const std::string & strUrl, BYTE** pData,int& nLen)
{
        if (strncmp(strUrl.c_str(),"https",5) == 0)
        {
                return Gets(strUrl,pData,nLen);
        }
        return Get(strUrl,pData,nLen);
}

int CHttpClient::GetString(const std::string & strUrl, std::string & strResponse)
{
        if (strncmp(strUrl.c_str(),"https",5) == 0)
        {
                return Gets(strUrl,strResponse);
        }
        return Get(strUrl,strResponse);

        
}
