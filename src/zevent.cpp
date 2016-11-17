#include "zevent.h"


#include <sys/time.h>
#include <errno.h>

#define ZTHREAD_NS 1000000000LL



ZEvent::ZEvent(const string Name,bool bInitSet,bool bWaitAutoReset)
{
    m_bSeted = bInitSet;
    m_bWaitAutoReset = bWaitAutoReset;
    m_Name = Name;
    
    pthread_cond_init(&m_hCond,NULL);
    pthread_mutex_init(&m_hMutex,NULL);

//     ZREGISTER_ADD("EVENT",this);
}


ZEvent::~ZEvent()
{
//     ZREGISTER_DEL("EVENT",this);

    //在析构时先中断所有的等待线程??
    
    //
    pthread_cond_destroy(&m_hCond);
    pthread_mutex_destroy(&m_hMutex);

}

void ZEvent::Set()
{
    pthread_mutex_lock(&m_hMutex);
    m_bSeted = true;
    pthread_cond_signal(&m_hCond);
    m_Counter_Set ++;
    pthread_mutex_unlock(&m_hMutex);

}
void ZEvent::Reset()
{
    pthread_mutex_lock(&m_hMutex);
    m_bSeted = false;
    m_Counter_Reset ++;
    pthread_mutex_unlock(&m_hMutex);	
}

bool ZEvent::Wait(int Timeout_ms)
{
    bool retval = true;
    pthread_mutex_lock(&m_hMutex);

    if(!m_bSeted)
    {
        if(Timeout_ms == 0)
        {
            pthread_cond_wait(&m_hCond,&m_hMutex);
            retval = true;
        }
        else
        {
            timeval now;
            timespec  timeout;
            gettimeofday(&now,NULL);
            long long ns_count = (long long )now.tv_sec * ZTHREAD_NS + now.tv_usec*1000;
            ns_count += ((long long)Timeout_ms) *1000000;
                            
            timeout.tv_sec = ns_count/ZTHREAD_NS;
            timeout.tv_nsec =ns_count %ZTHREAD_NS;
            if(pthread_cond_timedwait(&m_hCond,&m_hMutex,&timeout) == ETIMEDOUT)
                retval = false;
            else
                retval = true;
        }
    }
    
    if(m_bWaitAutoReset)
        m_bSeted = false;
	
    m_Counter_Wait ++;	

    pthread_mutex_unlock(&m_hMutex);
    return retval;
}

// void ZEvent::GetXmlInfo(stringstream &InfoStream) 
// {
// //    ZFilePathName fpn;
// //    fpn.SetFullPathName(mi.pLastLock_SrcFile);
//     InfoStream<<"<EVENT id=\""<<this<<"\">"
//             <<"<SET_FLAG>"<<m_bSeted<<"</SET_FLAG>"
//             <<"<SET_COUNTER>"<<m_Counter_Set<<"</SET_COUNTER>"
//             <<"<RESET_COUNTER>"<<m_Counter_Reset<<"</RESET_COUNTER>"
//             <<"<WAIT_COUNTER>"<<m_Counter_Wait<<"</WAIT_COUNTER>"
//             <<"</EVENT>";
//     
// }
