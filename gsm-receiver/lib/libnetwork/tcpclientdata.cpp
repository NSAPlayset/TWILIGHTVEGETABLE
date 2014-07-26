#include "tcpclientdata.h"
#include <time.h>

//#include "getmd5fromprog.h"

int g_sock_fd;
unsigned char g_abkey[128] ; 
int g_ikey_fn ; 
int g_ikey_timeslot ; 

/**
 * Return none 0 if connect successed
 */

zmq::context_t context (1);
zmq::socket_t sink(context, ZMQ_PUSH);
zmq::message_t message(200);

int iconnectserver()
{
    sink.connect("tcp://localhost:5558");
    return 0;
}


void write_burst(unsigned char* u, int arfcn, int timeslot, int fn,double power) {



	FILE *pFILE = fopen((char*)"rec_raw_burst.txt", "a");

	fprintf(pFILE, "A: %d TS: %d  FN: %d mfn: %d \n", arfcn, timeslot, fn, fn%51);
	for(int k = 0;k < 116; k++) {
		fprintf(pFILE, "%d",*(u+k));
	}
	fprintf(pFILE, "\n\n");

	fclose(pFILE);
}


int isenddata(int itimeslot , int ifn , int iarfcn , unsigned char *pbburstdata , int iburstdatalen )
{
    unsigned char btimeslot ;
	unsigned short warfcn ;

    btimeslot = itimeslot & 0xff ;
	warfcn = iarfcn & 0xffff ; 

    message.rebuild(7+iburstdatalen);
    memcpy((char*)(message.data()), &btimeslot,1);
    memcpy((char*)(message.data())+1, &ifn,4);
    memcpy((char*)(message.data())+5, &warfcn,2);
    memcpy((char*)(message.data())+7, pbburstdata,iburstdatalen);
    sink.send(message);
    return 0 ;
}


void disconnectserver()
{
}
