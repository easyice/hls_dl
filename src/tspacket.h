#pragma once
#include "Dvb.h"


#pragma warning (disable:4800)

#define MPEG_FRAME_TYPE_I	1
#define MPEG_FRAME_TYPE_P	2
#define MPEG_FRAME_TYPE_B	3
// #include <zapp.h>

class CTsPacket
{
public:
	CTsPacket(void);
	~CTsPacket(void);

	 bool SetPacket(BYTE *pPacket);

//������Ϣ
	 void Set_PID(PID pid);
	 bool Set_PCR(PCR pcr);
	 void Set_continuity_counter(BYTE Counter);
	 void RemoveAfField();
//������Ϣ
	 bool Get_transport_error_indicator();
	 bool Get_payload_unit_start_indicator();
	 bool Get_transport_priority();
	 WORD Get_PID();
	 BYTE Get_transport_scrambling_control();
	 BYTE Get_adaptation_field_control();
	 BYTE Get_continuity_counter();

//�����ֶ���Ϣ
	 BYTE * Get_adaptation_field(BYTE &Size);
	 BYTE Get_adaptation_field_length(BYTE *pAF_Data);
	 bool Get_discontinuity_indicator(BYTE *pAF_Data);
	 bool Get_random_access_indicator(BYTE *pAF_Data);
	 bool Get_elementary_stream_priority_indicator(BYTE *pAF_Data);
	 bool Get_PCR_flag(BYTE *pAF_Data);
	 bool Get_PCR_flag();
	 bool Get_OPCR_flag(BYTE *pAF_Data);
	 bool Get_splicing_point_flag(BYTE *pAF_Data);
	 bool Get_transport_private_data_flag(BYTE *pAF_Data);
	 bool Get_adaptation_field_extension_flag(BYTE *pAF_Data);
	 __int64 Get_program_clock_reference_base(BYTE *pAF_Data);
	 WORD Get_program_clock_reference_extension(BYTE *pAF_Data);
	 PCR  Get_PCR(BYTE *pAF_Data);
	 PCR  Get_PCR();

	 __int64 Get_original_program_clock_reference_base(BYTE *pAF_Data);
	 WORD Get_original_program_clock_reference_extension(BYTE *pAF_Data);
	 PCR  Get_OPCR(BYTE *pAF_Data);
	 BYTE Get_splice_countdown(BYTE *pAF_Data);
	 BYTE Get_transport_private_data_length(BYTE *pAF_Data);
	 BYTE *Get_private_data_byte(BYTE *pAF_Data,int &size);
	 BYTE Get_adaptation_field_extension_length(BYTE *pAF_Data);
	 bool Get_ltw_flag(BYTE *pAF_Data);
	 bool Get_piecewise_rate_flag(BYTE *pAF_Data);
	 bool Get_seamless_splice_flag(BYTE *pAF_Data);
	 bool Get_ltw_valid_flag(BYTE *pAF_Data);
	 WORD Get_ltw_offset(BYTE *pAF_Data);
	 DWORD Get_piecewise_rate(BYTE *pAF_Data);
	 __int64 Get_DTS_next_AU(BYTE *pAF_Data);
	 BYTE *Get_stuffing_byte(BYTE *pAF_Data,BYTE &Size);

//��ȡPES GOP��Ϣ
	 bool Get_PES_stream_id(BYTE &stream_id);
	 bool	Get_PES_GOP_INFO(DWORD &TimeCode/*ʱ����Ϣ*/);
	 bool	Get_PES_PIC_INFO(BYTE &PictureType/*ʱ����Ϣ*/);
	 bool H264_Get_slice_type(BYTE &SliceType/*10:IDR */);
 	 bool	Get_Sequenec_head_code();
//��ȡ��Ч����
	 BYTE * Get_Data(BYTE &Size);
	
//��ȡ����
	BYTE *m_pPacket;
};

/*	======================================================
*	˽�жνṹ��������
*/

class CPrivate_Section
{
public:
	CPrivate_Section();
	~CPrivate_Section();
	
	bool SetPacket(CTsPacket &Packet);
	
//ͨ��˽�ж�
	BYTE Get_table_id();
	bool Get_Section_sytax_indicator();
	WORD Get_Section_length();
//��׼˽�ж��﷨����
	WORD Get_table_id_extension();
	BYTE Get_version_number();
	bool Get_current_next_indicator();
	BYTE Get_section_number();
	BYTE Get_last_section_number();

//˽�зֶ�����
	BYTE * Get_private_data_byte(WORD &Size);
//CRC����
	DWORD Get_CRC_32();
	
private:
    BYTE *m_pSection;
};

/**
* ��װ��������Ϣ����
*/

class CDescriptor
{
public:
	CDescriptor(){};
	~CDescriptor(){};
	void SetDescriptor(BYTE *Descriptor);
	BYTE Get_descriptor_tag();
	BYTE Get_descriptor_length();
private:
	BYTE *m_pDescriptor;
};
