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

#include "cdownloader.h"
#include "HttpClient.h"
#include <string.h>
#include "cm3u8parse.h"
#include "creadline.h"
#include <stdio.h>
#include "utils.h"
#include "cudpsend.h"



extern "C"
{
        #include <curl/curl.h>
}


CDownloader::CDownloader():m_eventOnTsAdd("CDOWNLOADER")
{
        m_pHttpClient = new CHttpClient();
        pthread_mutex_init(&m_mutex,NULL);
        m_eventOnTsAdd.Reset();
        m_bStop = false;
        m_pSaveFile = new CSaveFile();
        m_duration = 0;
        m_thread = 0;
        m_pUdpSend = new CUdpSend();
}

CDownloader::~CDownloader()
{
        delete m_pHttpClient;
        pthread_mutex_destroy(&m_mutex);
        delete m_pSaveFile;
        delete m_pUdpSend;
}

void CDownloader::Run(const INIT_PARAM_T& init_param)
{
        m_stInitparam = init_param;
        
        string m3u8_content;
        if (m_pHttpClient->GetString(init_param.url,m3u8_content) != 0)
        {
                printf("download m3u8 index error! init_param.url=%s\n",init_param.url.c_str());
                return;
        }
        printf("m3u8_content=%s\n",m3u8_content.c_str());
        printf("Effective_url=%s\n",m_pHttpClient->m_strEffective_url.c_str());

        //m_sys.m3u8 = init_param.url;
        //跟随301
        m_sys.m3u8 = m_pHttpClient->m_strEffective_url;
        
        if (!SelectStream(m_sys,m_hls,m3u8_content))
        {
                return;
        }
        m_pSaveFile->m_segment_count = m_hls.segment_count;
        m_pSaveFile->m_m3u8name = m_hls.url.substr( m_hls.url.rfind("/")+1);
        m_pSaveFile->m_m3u8name = removeparam(m_pSaveFile->m_m3u8name);
        if (m_pSaveFile->m_m3u8name.empty())
        {
                cout << " m3u8 name removeparam faild" << endl;
                return;
        }
        m_duration = m_hls.duration;
        if (m_duration < 1) 
        {
                m_duration = 1;
        }
        if (m_duration > 10)
        {
                m_duration = 4;
        }

        m_pSaveFile->SetParam(init_param);
        m_pSaveFile->Init();

        printf("%s Playlist HLS protocol version: %d,m_duration:%d\n",m_sys.b_live ? "Live": "VOD", m_hls.version,m_duration);

        if (m_sys.b_live)
        {
                //启动UDP发送模块
                if (!init_param.udp_send_addr.empty())
                {
                        m_pUdpSend->InitSend(init_param.udp_send_addr,init_param.udp_port,init_param.udp_local_addr);
                }
                
                
                list< segment_t >::const_iterator it = m_lstOldSegments.begin();
                for (;it != m_lstOldSegments.end();++it)
                {
                        m_dqTsUrl.push_back(it->url);
                }
                m_eventOnTsAdd.Set();
                pthread_create(&m_thread,NULL,ReloadThread,this);
        }
        else
        {
                m_pSaveFile->OnRecvM3u8(ReplaceM3u8(m_strOldM3u8Content));
                list< segment_t >::const_iterator it = m_lstOldSegments.begin();
                for (;it != m_lstOldSegments.end();++it)
                {
                        m_dqTsUrl.push_back(it->url);
                }
                m_eventOnTsAdd.Set();
                
        }

        MainLoop();
        
        if (m_thread != 0)
        {
                pthread_join(m_thread,NULL);
        }
}

bool CDownloader::SelectStream(stream_sys_t& sys, hls_stream_t& hls,const string& m3u8_content)
{
        CM3u8Parse parse;
        if (!parse.isHTTPLiveStreaming(m3u8_content))
        {
                printf("not m3u8 file!\n");
                return false;
        }
        
        if (parse.isMeta(m3u8_content))
        {
                sys.b_meta = true;
                if (!parse.ParseMeta(sys,m3u8_content))
                {
                        printf("meta parse error!\n");
                        return false;
                }
                hls = parse.m_lstStreams.front();
                
                cout << "meta list:" << endl;
                list<hls_stream_t>::iterator it = parse.m_lstStreams.begin();
                for (; it != parse.m_lstStreams.end(); ++it)
                {
                        cout << it->id << "--" << it->url << endl;
                }

                cout << "selected stream:" << hls.url << endl;
                sys.m3u8 = hls.url;
                
                //download meta and parse
                string new_m3u8;
                if (m_pHttpClient->GetString(sys.m3u8,new_m3u8) != 0)
                {
                        printf("download m3u8 index error!\n");
                        return false;
                }
                //跟随301
                sys.m3u8 = m_pHttpClient->m_strEffective_url;
                return SelectStream(sys,hls,new_m3u8);
        }
        else
        {
                if (!parse.ParseStream(sys,&hls,m3u8_content))
                {
                        printf("m3u8 parse error!\n");
                        return false;
                }
                m_lstOldSegments = parse.m_lstSegments;
                m_strOldM3u8Content = m3u8_content;
        }
        return true;
}

void* CDownloader::ReloadThread(void* pParam)
{
        CDownloader* lpthis = (CDownloader*) pParam;
        lpthis->ReloadFun();

        return NULL;
}

void CDownloader::ReloadFun()
{
        while(1)
        {
                printf("reloading m3u8...\n");
                string m3u8_content;
                int ret = m_pHttpClient->GetString(m_hls.url,m3u8_content);
                if (ret != CURLE_OK)
                {
                        printf("download m3u8 index error!\n");
                        usleep(10000);
                        continue;
//                         if (ret ==  CURLE_OPERATION_TIMEOUTED)
//                         {
//                                 continue;
//                         }
//                        //break;
                }
                if (m3u8_content == m_strOldM3u8Content)
                {
                        //wait
                        sleep(m_duration/2);
                        continue;
                }
                
                
                
                CM3u8Parse parse;
                if (!parse.ParseStream(m_sys,&m_hls,m3u8_content))
                {
                        printf("m3u8 parse error!\n");
                        continue;
                        //break;
                }
                
                OnNewM3u8(parse.m_lstSegments);
                
                //等ts下载完再更新m3u8
                while(1)
                {
                        usleep(50000);
                        pthread_mutex_lock(&m_mutex);
                        bool bempty = m_dqTsUrl.empty();
                        pthread_mutex_unlock(&m_mutex);
                        if (bempty)
                        {
                                break;
                        }
                }
                
                
                //
                m_pSaveFile->OnRecvM3u8(ReplaceM3u8(m3u8_content));
                
                m_strOldM3u8Content = m3u8_content;
                m_lstOldSegments = parse.m_lstSegments;
                
                
        }
        
        m_bStop = true;
}

void CDownloader::OnNewM3u8(const list< segment_t >& lst)
{
        list< segment_t >::const_iterator it = lst.begin();
        for (;it != lst.end();++it)
        {
                bool bfind = false;
                list<segment_t>::iterator it_old = m_lstOldSegments.begin();
                for(; it_old != m_lstOldSegments.end();++it_old)
                {
                        if (it->url == it_old->url)
                        {
                                bfind = true;
                                break;
                        }
                }
                if (!bfind)
                {
                        pthread_mutex_lock(&m_mutex);
                        m_dqTsUrl.push_back(it->url);
                        pthread_mutex_unlock(&m_mutex);
                        m_eventOnTsAdd.Set();
                }
        }
        
        
}

void CDownloader::MainLoop()
{
        while(!m_bStop)
        {
                pthread_mutex_lock(&m_mutex);
                bool bempty = m_dqTsUrl.empty();
                pthread_mutex_unlock(&m_mutex);
                
                if (bempty)
                {
                        if (m_sys.b_live) 
                        {
                                m_eventOnTsAdd.Wait(); 
                                continue;
                        }
                        else
                        {
                                return;
                        }
                        
                }
                
                
                pthread_mutex_lock(&m_mutex);
                string url = *m_dqTsUrl.begin();
                pthread_mutex_unlock(&m_mutex);
                
                if (m_stInitparam.continue_mode && m_pSaveFile->IsExist(url))
                {
                        pthread_mutex_lock(&m_mutex);
                        m_dqTsUrl.pop_front();
                        pthread_mutex_unlock(&m_mutex);
                        continue;
                }
                
                BYTE* p;
                int nLen;
                int ret = m_pHttpClient->GetBin(url,&p,nLen);
                if ( ret != CURLE_OK)
                {
                        printf("download ts error!retrying.. ret=%d,url=%s\n",ret,url.c_str());
                        if (ret ==  CURLE_OPERATION_TIMEOUTED)
                        {
                                continue;
                        }
                        else //忽略当前分片
                        {
                                _exit(1);
//                                 pthread_mutex_lock(&m_mutex);
//                                 m_dqTsUrl.pop_front();
//                                 pthread_mutex_unlock(&m_mutex);
//                                 continue;
                        }

                }
                pthread_mutex_lock(&m_mutex);
                m_dqTsUrl.pop_front();
                pthread_mutex_unlock(&m_mutex);
                
                m_pSaveFile->OnRecvTs(url,p,nLen);
                
                if (!m_stInitparam.udp_send_addr.empty())
                {
                        m_pUdpSend->PushData(p,nLen);
                }
                
                pthread_mutex_lock(&m_mutex);
                int remain = m_dqTsUrl.size();
                pthread_mutex_unlock(&m_mutex);
                printf("remain:%d\n",remain);
        }
}

string CDownloader::ReplaceM3u8(const string& m3u8_content)
{
        string rst;
        CReadLine readline;
        readline.SetString(m3u8_content);
        do 
        {
                /* Next line */
                string line = readline.ReadLine();
                
                if (line.empty())
                {
                        break;
                }
                
                line += "\n";
                if (line[0] == '#')
                {
                        rst.append(line);
                        continue;
                }
                else
                {
                        if (line.rfind("/") != string::npos)
                        {
                                string tsname = line.substr(line.rfind("/")+1);
                                tsname = removeparam(tsname);
                                if (tsname.empty())
                                {
                                        cout << "tsname removeparam in ReplaceM3u8 faild"<<endl;
                                        continue;
                                }
                                if (tsname[tsname.length()-1] != '\n')
                                {
                                        tsname += "\n";
                                }
                                rst.append(tsname);
                        }
                        else
                        {
                                rst.append(line);
                        }
                }
                
        }while(1);
        
        return rst;
}


