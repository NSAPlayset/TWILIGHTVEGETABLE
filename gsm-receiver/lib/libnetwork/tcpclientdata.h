#ifndef _TCPCLIENTDATA_H
#define _TCPCLIENTDATA_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define NPGSM_UDP_PORT 29653
#define NPGSM_MC_IP "239.13.37.1"

#include <boost/asio.hpp>

struct gsm_frame_raw
{
    int itimeslot;
    int ifn;
    int iarfcn;
    unsigned char *pbburstdata;
    int iburstdatalen;
};

// ���ӷ����� 0�ɹ� 1ʧ��
int iconnectserver() ;

// ����BURST���
int isenddata(int itimeslot , int ifn , int iarfcn , unsigned char *pbburstdata , int iburstdatalen );

// ��ѹ��BURST��ݣ�8--->1
int compressburstdata8_1(unsigned char *pbburstdata , int iburstdatalen , unsigned char *pbcompressdata , int &icompressdatalen) ; 

// ��������Ͽ�����
void disconnectserver() ; 

#endif
