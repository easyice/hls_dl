#pragma once
#ifndef CSAFEDEQUE_H
#define CSAFEDEQUE_H

#include <deque>
#include "define.h"
using namespace std;


//发生上溢时处理方式
typedef enum _OVERFLOW_TREATMENT
{
	OVERFLOW_DROP, //扔掉旧的数据
	OVERFLOW_WAIT //等待
}OVERFLOW_TREATMENT;

/**
 * 上溢:缓冲区已满又继续添加数据
*/

///@brief 线程安全的双队列
class CSafeDeque
{
public:

	/**
	  * @param nUnitSize 缓冲单元大小
	  * @param nCount 单元数量
	 */
	CSafeDeque(int nUnitSize,int nCount);

	~CSafeDeque(void);

	/**
	  *@brief 设置上溢处理方式
	 */
	void SetOverFlowTreatment( OVERFLOW_TREATMENT emType);

	/**
	  * @brief 从队列取一幅图像,使用完后需由调用方执行 ReleaseBuffer
	  * @return NULL 没有数据，否则返回数据指针
	 */
	 uint8_t* PopFront();


	 /**
	  * @brief 将一个缓冲指针放到空闲缓冲队列 (m_dqFreeBuffer)
	 */
	 void ReleaseBuffer(uint8_t* pBuffer);


	 /**
	  * @brief 向队列追加数据
	  * @return 0,成功;1缓冲已满，旧的数据被覆盖;2.缓冲已满，数据添加失败
	 */
	 int PushBack(uint8_t* pBuffer);


	 /**
	  * @brief 获取只读的数据队列
	  * @return 数据队列
	 */

	 const deque<uint8_t*>& LockDataDq();

	 void UnlockDataDq();

	 //是否为空
	 bool Epmty();

	 //获取数据元素数量
	 int GetDataSize();

	 //获取队列大小
	 int GetDqSize();
private:

	///缓冲指针
	uint8_t* m_pOutBuffer;

	///数据队列
	deque<uint8_t*> m_dqData;

	///空闲缓冲队列
	deque<uint8_t*> m_dqFreeBuffer;


	///发生上溢时处理方式
	OVERFLOW_TREATMENT m_emOverflowTreatment;

	pthread_mutex_t m_mutex;

	int m_nUnitSize;

	int m_nUnitCount;
};



#endif


