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

#ifndef CSAVEFILE_H
#define CSAVEFILE_H
#include "define.h"
#include <deque>
#include <list>

//两个线程分别写ts和m3u8,注意线程安全
class CSaveFile
{
public:
        CSaveFile();
        ~CSaveFile();
        void SetParam(const INIT_PARAM_T& param);
        void OnRecvM3u8(const string& m3u8);
        void OnRecvTs(const string& url,const BYTE* pData,int nLen);
        bool IsExist(const string& url);
        bool Init();
public:
        int m_segment_count;
        string m_m3u8name;
private:
        void OnCheckTsExpire();
private:
        INIT_PARAM_T m_stParam;

        string m_m3u8pathname;
        string m_path;//保存文件的绝对路径
        list<string> m_lstTsHistory;
        
        bool m_bFirstSave;
        FILE* m_fpOut;
};

#endif // CSAVEFILE_H
