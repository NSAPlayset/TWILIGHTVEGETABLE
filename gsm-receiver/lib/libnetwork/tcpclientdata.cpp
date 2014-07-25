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
boost::asio::ip::udp::endpoint remote_endpoint(boost::asio::ip::address::from_string(NPGSM_RAW_ADDR), NPGSM_RAW_PORT);

int iconnectserver()
{
    boost::system::error_code error;

    udpsocket.open(boost::asio::ip::udp::v4(), error);
    if (!error)
    {
        udpsocket.set_option(boost::asio::ip::udp::socket::broadcast(true));
        udpsocket.set_option(boost::asio::ip::multicast::enable_loopback(true));
        udpsocket.set_option(boost::asio::ip::multicast::hops(1));
        udpsocket.set_option(boost::asio::ip::multicast::outbound_interface(boost::asio::ip::address_v4::loopback()));
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
    unsigned char btimeslot ;
	unsigned short warfcn ;
	unsigned char abdata[200] ;
	static unsigned char tmp  = 0;

    btimeslot = itimeslot & 0xff ;
	warfcn = iarfcn & 0xffff ; 
    //gsmtap_send (struct gsmtap_inst *gti, uint16_t arfcn, uint8_t ts, uint8_t chan_type, uint8_t ss, uint32_t fn, int8_t signal_dbm, uint8_t snr, const uint8_t *data, unsigned int len)
    //gsmtap_send(gti, warfcn, btimeslot, chan_type, ss, ifn, 0, 0, pbburstdata, iburstdatalen);
	memset(abdata , 0 , sizeof(abdata)) ; 
	abdata[0] = btimeslot ;
	memcpy(&abdata[1] , &ifn , 4) ; 
	memcpy(&abdata[5] , &warfcn , 2 ) ; 
    memcpy(&abdata[7] , pbburstdata ,  iburstdatalen) ;

    boost::system::error_code ignored_error;
    udpsocket.send_to(boost::asio::buffer((char *)abdata, 7+iburstdatalen), remote_endpoint, 0, ignored_error);
    return 0 ;
}


void disconnectserver()
{
}
