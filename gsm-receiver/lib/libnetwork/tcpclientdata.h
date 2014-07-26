#ifndef _TCPCLIENTDATA_H
#define _TCPCLIENTDATA_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <zmq.hpp>

#define BUF_SIZE 1024
#define NPGSM_RAW_PORT 29653
#define NPGSM_RAW_ADDR "239.255.0.1"

// ���ӷ����� 0�ɹ� 1ʧ��
int iconnectserver() ;

// ����BURST���
int isenddata(int itimeslot , int ifn , int iarfcn , unsigned char *pbburstdata , int iburstdatalen );

// ��������Ͽ�����
void disconnectserver() ; 

#endif
