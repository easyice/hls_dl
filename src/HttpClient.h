#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H


#include <string>
#include "define.h"

class CDlBuffer;
class CHttpClient
{
public:
	CHttpClient(void);
public:
	~CHttpClient(void);

	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);

	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Get(const std::string & strUrl, std::string & strResponse);
	int Get(const std::string & strUrl, BYTE** pData,int& nLen);

	/**
	* @brief HTTPS POST请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* @brief HTTPS GET请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);
	int Gets(const std::string & strUrl, BYTE** pData,int& nLen, const char * pCaPath = NULL);


        int GetBin(const std::string & strUrl, BYTE** pData,int& nLen);
        int GetString(const std::string & strUrl, std::string & strResponse);
	void SetDebug(bool bDebug);

public:
        string m_strEffective_url;
private:
	static size_t OnRecvData(void *buffer, size_t size, size_t nmemb, void *stream);
	static size_t OnRecvString(void* buffer, size_t size, size_t nmemb, void* lpVoid);
private:
	bool m_bDebug;
	CDlBuffer* m_pDlBuffer;

};


#endif

