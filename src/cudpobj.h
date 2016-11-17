/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  elva <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef CUDPOBJ_H
#define CUDPOBJ_H

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>


using namespace std;



typedef enum _UDP_OBJ_TYPE_T
{
	OBJ_UDP_CLIENT,
	OBJ_UDP_SERVER
}UDP_OBJ_TYPE_T;


typedef struct _UDP_OBJ_PARAM_T
{
	UDP_OBJ_TYPE_T emObjType;
	string ipAddr; // client 或 server ip地址，当为client并且是组播地址时，支持 ip@mulIP方式
	int port;	//client 或 server port
	string localAddr; //当ipAddr为组播地址时，本字段表示使用的本级ip地址;
}UDP_OBJ_PARAM_T;


typedef struct _UDP_OBJ_ATTRIBUTE_T
{
	int socket;
	struct sockaddr_in sock_addr_in;
	int addr_len;
}UDP_OBJ_ATTRIBUTE_T;



class CUdpObj
{
public:
	CUdpObj();
	~CUdpObj();
    
	void SetParam(const UDP_OBJ_PARAM_T& param);
	int CreateObj();
	const UDP_OBJ_ATTRIBUTE_T* GetObjAttr();
	const UDP_OBJ_PARAM_T* GetObjParam();
    
private:
	int CreateServerObj();
	int CreateClientObj();
private:
	UDP_OBJ_PARAM_T m_stParam;
	UDP_OBJ_ATTRIBUTE_T m_stAttr;
};

#endif // CUDPOBJ_H
