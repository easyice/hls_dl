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

#ifndef CDOWNLOADER_H
#define CDOWNLOADER_H
#include "define.h"
#include <pthread.h>
#include <deque>
#include "zevent.h"
#include "csavefile.h"
#include <list>


class CUdpSend;
class CHttpClient;
class CDownloader
{
public:
        CDownloader();
        ~CDownloader();
        void Run(const INIT_PARAM_T& init_param);
        
private:
        //m3u8 reload thread
        static void* ReloadThread(void* pParam);
        void ReloadFun();
        void MainLoop();
        
        /**
         *@param [in/out] sys
         * @param [in/out] hls
         */
        bool SelectStream(stream_sys_t &sys,hls_stream_t& hls,const string& m3u8_content);
        
        //判断新增segment，加入待下载列表
        void OnNewM3u8(const list<segment_t>& lst);
        
        string ReplaceM3u8(const string& m3u8_content);
private:

        INIT_PARAM_T m_stInitparam;
        
        CHttpClient* m_pHttpClient;
        CSaveFile* m_pSaveFile;
        CUdpSend* m_pUdpSend;
        
        pthread_t m_thread;
        pthread_mutex_t m_mutex;
        deque<string> m_dqTsUrl;//待下载TS
        
        stream_sys_t m_sys;
        hls_stream_t m_hls;
        ZEvent m_eventOnTsAdd;
        
        list<segment_t> m_lstOldSegments;
        string m_strOldM3u8Content;
        
        bool m_bStop;
        
        int m_duration;
        
};

#endif // CDOWNLOADER_H
