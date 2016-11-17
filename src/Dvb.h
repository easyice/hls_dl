#pragma once
// #include <zapp.h>
// #include <ztypes.h>
typedef   unsigned long long  __int64 ;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef WORD  PID;
#define INVALID_PID 0xffff
#define TS_PACKET_IDENT	0x47

typedef   unsigned long long  PCR;
#define INVALID_PCR 0x0
#define MAX_PCR	0x40000000000


/*

#define TS_PACKET_LENGTH_188	188
#define TS_PACKET_LENGTH_204	204
#define TS_PACKET_LENGTH_STANDARD	TS_PACKET_LENGTH_188
*/


#define TS_REF_CLOCK	27000000

//ȱʡPCRʱ������
#define DEFAULT_TS_PCR_INTERVAL (TS_REF_CLOCK / 20)

#define MS_TO_PCR(time_ms) (time_ms * 27000)

typedef WORD PROGRAM_NUMBER;
typedef BYTE SECTION_NUMBER;

#define GET_TS_PID(pBuf) (WORD)((((*(pBuf)) & 0x1f)<<8) | (*(pBuf+1)))

#define GET_PCR_INTERVAL(from,to) ((from <= to) ? (to-from) : (MAX_PCR - from + to))


typedef enum _TS_PACKET_LENGTH_TYPE
{
	TS_PACKET_LENGTH_STANDARD =188,
	TS_PACKET_LENGTH_188 =188,
	TS_PACKET_LENGTH_204 = 204,
	TS_PACKET_LENGTH_INVALID = 0

}TS_PACKET_LENGTH_TYPE;

