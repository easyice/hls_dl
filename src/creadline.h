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

#ifndef CREADLINE_H
#define CREADLINE_H
#include <iostream>
#include "define.h"

using namespace std;


class CReadLine
{
public:
        CReadLine();
        ~CReadLine();
        bool SetString(const string& str);
        string ReadLine();
private:
        char *ReadLine(uint8_t *buffer, uint8_t **pos, const size_t len);
private:
        string m_str;
        uint8_t *m_pRead;
        uint8_t *m_pBegin;
        uint8_t *m_pEnd;
};

#endif // CREADLINE_H
