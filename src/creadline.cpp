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

#include "creadline.h"
#include "define.h"
#include <assert.h>

#include <string.h>
#include <stdlib.h>

char *strdup (const char *str)
{
    size_t len = strlen (str) + 1;
    char *res = (char*)malloc (len);
    if (res)
        memcpy (res, str, len);
    return res;
}

char *strndup (const char *str, size_t max)
{
    size_t len = strnlen (str, max);
    char *res = (char*)malloc (len + 1);
    if (res)
    {
        memcpy (res, str, len);
        res[len] = '\0';
    }
    return res;
}


CReadLine::CReadLine()
{

}

CReadLine::~CReadLine()
{

}

bool CReadLine::SetString(const string& str)
{
        if (str.empty())
        {
                return false;
        }
        m_str = str;
        
        m_pRead = NULL;
        m_pBegin = (uint8_t*)m_str.c_str();
        m_pEnd = m_pBegin + m_str.length();
        
        return true;
}

string CReadLine::ReadLine()
{
        if (m_pBegin >= m_pEnd)
        {
                return "";
        }
        char *line = ReadLine(m_pBegin, &m_pRead, m_pEnd - m_pBegin);
        if (line == NULL)
        {
                return "";
        }
        m_pBegin = m_pRead;
        
        string str = line;
        free(line);
        return str;
}

char *CReadLine::ReadLine(uint8_t *buffer, uint8_t **pos, const size_t len)
{
    assert(buffer);

    char *line = NULL;
    uint8_t *begin = buffer;
    uint8_t *p = begin;
    uint8_t *end = p + len;

    while (p < end)
    {
        if ((*p == '\r') || (*p == '\n') || (*p == '\0'))
            break;
        p++;
    }

    /* copy line excluding \r \n or \0 */
    line = strndup((char *)begin, p - begin);

    while ((*p == '\r') || (*p == '\n') || (*p == '\0'))
    {
        if (*p == '\0')
        {
            *pos = end;
            break;
        }
        else
        {
            /* next pass start after \r and \n */
            p++;
            *pos = p;
        }
    }

    return line;
}
