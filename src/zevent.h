#ifndef ZEVENT_H
#define ZEVENT_H

#include <pthread.h>

#include <string>
using namespace std;



/**
 * 多线程事件处理
 * 使用条件变量实现
 */
class ZEvent
{
    public:
        ZEvent(const string Name,bool bInitSet = false,bool bWaitAutoReset = true);
        ~ZEvent();
        
        void Set();
        void Reset();
        bool Wait(int Timeout_ms = 0);

    protected:
        


        pthread_mutex_t m_hMutex;
        pthread_cond_t m_hCond;
        bool m_bSeted;
        bool m_bWaitAutoReset;
        int m_Counter_Set;
        int m_Counter_Reset;
        int m_Counter_Wait;
        string m_Name;

};

#endif
