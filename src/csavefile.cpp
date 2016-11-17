/*
 * Copyright 2014 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#include "csavefile.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include "utils.h"



//ts老化时间(秒)
const int TS_EXPIRE_TIME = 60*3;



CSaveFile::CSaveFile()
{
        m_segment_count = 0;
        m_bFirstSave = true;
        m_fpOut = NULL;
}

CSaveFile::~CSaveFile()
{
        if (m_fpOut!=NULL)
        {
                fclose(m_fpOut);
        }
}

void CSaveFile::SetParam(const INIT_PARAM_T& param)
{
        m_stParam = param;
}

void CSaveFile::OnCheckTsExpire()
{

        list<string> file_lst;
        findprocess(file_lst,m_path.c_str());
        
        if (file_lst.empty())
        {
                cout << "dest path is empty,OnCheckTsExpire nothing to do" << endl;
                return ;
        }
        

        list<string>::iterator it = file_lst.begin();
        for (; it != file_lst.end(); ++it)
        {
                if (it->rfind(".") == string::npos)
                {
                        continue;
                }
                string ext_name = it->substr(it->rfind("."));
 
                if (strncasecmp(ext_name.c_str(),".ts",3) != 0)
                {
                        continue;
                }
                
                time_t cur_time = time(NULL);
                struct stat statbuf;
                if ( stat ( it->c_str(),&statbuf ) != 0 )
                {
                        cout << "CheckOldThread stat error " << endl;
                        continue;
                }
                if (difftime(cur_time,statbuf.st_mtime) > TS_EXPIRE_TIME)
                {
                        cout << "remove file:" << *it << endl; 
                        unlink(it->c_str());
                }
                cout << "difftime=" << difftime(cur_time,statbuf.st_mtime) << endl;
        }

}

bool CSaveFile::IsExist(const string& url)
{
        string tsname = url.substr(url.rfind("/")+1);
        tsname = removeparam(tsname);
        if (tsname.empty())
        {
                cout << "removeparam faild" << endl;
                return false;
        }

        string pathname = m_path + tsname;
        
        if( access(pathname.c_str(), F_OK) == 0 )
        {
                return true;
        }
        
        return false;
}

void CSaveFile::OnRecvTs(const string& url,const BYTE* pData, int nLen)
{
        if (m_bFirstSave)
        {
                if (m_stParam.realtime_mode)
                {
                        OnCheckTsExpire();
                }
                m_bFirstSave = false;
        }
        if (!m_stParam.savefile.empty())
        {
                if (m_fpOut == NULL)
                {
                        cout << "open:" << m_stParam.savefile << endl;
                        m_fpOut = fopen(m_stParam.savefile.c_str(),"wb");
                }
                fwrite(pData,1,nLen,m_fpOut);
                return;
        }
        string tsname = url.substr(url.rfind("/")+1);
        tsname = removeparam(tsname);
        if (tsname.empty())
        {
                cout << "removeparam faild" << endl;
                return;
        }

        string pathname = m_path + tsname;
        
        FILE* fp = fopen(pathname.c_str(),"wb");
        if (fp == NULL)
        {
                printf("open file error:%s\n",pathname.c_str());
                return;
        }
        int writed = fwrite(pData,1,nLen,fp);
        fclose(fp);
        
        printf("writefile:%s,size:%d,write:%d,err=%d\n",pathname.c_str(),nLen,writed,errno);
        
        //针对ffmpeg的循环生成文件方式
        list<string>::iterator it = m_lstTsHistory.begin();
        for(; it != m_lstTsHistory.end(); ++it)
        {
                if (*it == pathname)
                {
                        m_lstTsHistory.erase(it);
                        break;
                }
        }
        m_lstTsHistory.push_back(pathname);
        
        //保留TS数量
        int leave_count = m_segment_count*3;
        if (leave_count < 20)
        {
                leave_count = 20;
        }
        
        //删除旧的
        if (m_stParam.realtime_mode && m_lstTsHistory.size() > leave_count)//多保留一会
        {
                printf("remove ts:%s\n",m_lstTsHistory.begin()->c_str());
                unlink(m_lstTsHistory.begin()->c_str());
                m_lstTsHistory.pop_front();
        }
        
}

void CSaveFile::OnRecvM3u8(const string& m3u8)
{
        if (!m_stParam.savefile.empty())
        {
                cout << "暂时不支持保存单个文件" << endl;
                return;
        }
        
        FILE* fp = fopen(m_m3u8pathname.c_str(),"w");
        if (fp == NULL)
        {
                printf("open file error:%s\n",m_m3u8pathname.c_str());
                return;
        }
        fwrite(m3u8.c_str(),1,m3u8.size(),fp);
        fclose(fp);
}

bool CSaveFile::Init()
{
        if (m_stParam.outname.empty())
        {
                m_path = m_stParam.savepath + m_m3u8name + "/";
        }
        else
        {
                m_path = m_stParam.savepath + m_stParam.outname + "/";
        }
        
        CreateMulPath(m_path.c_str());
        //mkdir( m_path.c_str(), 0777 );
        
        m_m3u8pathname = m_path + m_m3u8name;
        
        return true;
}
