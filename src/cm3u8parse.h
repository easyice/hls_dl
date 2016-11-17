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

#ifndef CM3U8PARSE_H
#define CM3U8PARSE_H
#include <iostream>
#include "define.h"
#include <list>

class CM3u8Parse
{
public:
        CM3u8Parse();
        ~CM3u8Parse();
        
        /**
         * @param [in/out] sys
         */
        //bool Parse(stream_sys_t& sys,hls_stream_t* init_hls,const string& m3u8_content);

        bool isHTTPLiveStreaming(const string& m3u8_content);
        bool isMeta(const string& m3u8_content);
        
        bool ParseMeta(stream_sys_t& sys,const string& m3u8_content);
        bool ParseStream(stream_sys_t& sys,hls_stream_t* init_hls,const string& m3u8_content);
private:
        
        
        int parse_AddSegment(hls_stream_t *hls, const int duration, const char *uri);
        
public:

        
public:
        //meta情况下解析结果保存在此处
        list<hls_stream_t> m_lstStreams;
        
        //非meta情况下解析结果保存在此
        //hls_stream_t* m_pHls;
        list<segment_t> m_lstSegments;
};

#endif // CM3U8PARSE_H
