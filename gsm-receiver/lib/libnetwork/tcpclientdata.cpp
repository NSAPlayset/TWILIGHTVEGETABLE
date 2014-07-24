#include "tcpclientdata.h"
#include <time.h>

//#include "getmd5fromprog.h"

struct sockaddr_in g_server_sin;
struct hostent *g_host;
int g_sock_fd;
unsigned char g_abkey[128] ; 
int g_ikey_fn ; 
int g_ikey_timeslot ; 

/**
 * Return none 0 if connect successed
 */
boost::asio::io_service ioservice;
boost::asio::ip::udp::socket udpsocket(ioservice);
boost::asio::ip::udp::endpoint remote_endpoint(boost::asio::ip::address_v4::loopback(), NPGSM_UDP_PORT);

int iconnectserver()
{
    boost::system::error_code error;

    udpsocket.open(boost::asio::ip::udp::v4(), error);
    if (!error)
    {
        udpsocket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    } else {
        return 1;
    }
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
    unsigned char abcompressdata[20] ;
	int icompresslen ; 
	unsigned char btimeslot ;
	unsigned short warfcn ;
	unsigned char abdata[200] ;
	static unsigned char tmp  = 0;

	memset(abcompressdata , 0 , sizeof(abcompressdata)) ;
	icompresslen = 20 ;
	compressburstdata8_1(pbburstdata , iburstdatalen , abcompressdata , icompresslen) ; 

	btimeslot = itimeslot & 0xff ;
	warfcn = iarfcn & 0xffff ; 

	memset(abdata , 0 , sizeof(abdata)) ; 
	abdata[0] = btimeslot ;
	memcpy(&abdata[1] , &ifn , 4) ; 
	memcpy(&abdata[5] , &warfcn , 2 ) ; 
	memcpy(&abdata[7] , abcompressdata ,  icompresslen) ;

    boost::system::error_code ignored_error;
    udpsocket.send_to(boost::asio::buffer((char *)abdata, 24+sizeof(double)), remote_endpoint, 0, ignored_error);
    return 0 ;
}


void disconnectserver()
{
}


int compressburstdata8_1(unsigned char *pbburstdata , int iburstdatalen , unsigned char *pbcompressdata , int &icompressdatalen)
{
	if((iburstdatalen&7)==0)
	{
		icompressdatalen = iburstdatalen>>3 ; 
	}
	else
	{
		icompressdatalen = (iburstdatalen>>3) + 1; 
	}
	
	int i , j ; 
	int iwz = 0 ; 
	unsigned char btemp ; 
	for(i=0;i<icompressdatalen-1;i++)
	{
		btemp = 0 ; 
		for(j=0;j<8;j++)
		{
			btemp = (btemp<<1)|pbburstdata[iwz] ; 
			iwz ++ ; 
		}
		pbcompressdata[i] = btemp ; 
	}
	btemp = 0 ; 
	for(j=0;j<8;j++)
	{
		if(iwz<iburstdatalen)
		{
			btemp = (btemp<<1)|pbburstdata[iwz] ; 
			iwz ++ ; 
		}
		else
		{
			break ; 
		}
	}
	pbcompressdata[i] = btemp ; 
	
	return 0 ; 
}

int idecompressburstdata1_8(unsigned char *pbcompress , unsigned char *pbburstdata)
{
        memset(pbburstdata , 0 , sizeof(pbburstdata)) ;
                int i , j ;
                int iwz = 0 ;
                for(i=0;i<14;i++)
                {
                        for(j=0;j<8;j++)
                        {
                                pbburstdata[iwz] = (pbcompress[i]>>(7-j))&1 ;
                                iwz ++ ;
                        }
                }
                for(j=0;j<4;j++)
                {
                        pbburstdata[iwz] = (pbcompress[i]>>(3-j))&1 ;
                        iwz ++ ;
                }
                return 0 ;
}


