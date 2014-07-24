#ifndef SDRCONFIGURATIONSTRUCT_H_
#define SDRCONFIGURATIONSTRUCT_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string.h>

struct SDRconfiguration
{
    int decim;
    int freq;
    int osr;
    std::string ch0_infile;
    std::string ch1_infile;
    int ch0_arfcn;
    int ch1_arfcn;
    int rxa_gain;
    int rxb_gain;
    bool is_double_downlink;
    bool realtime;
    int which_board;
    int fcch_ch;

    bool ch0_upload;
    bool ch1_upload;
    std::string net_ip;
    int net_port;

    int print_burst;

    int scanBTSmode;
    int sch_timeout_boundary;
    int fcch_timeout_boundary;

    int scan_bts_chanstart;
    int scan_bts_chanstop;

    /*
    int wideband_mode;
    int wb_bandwith;

    int wb_cfreq_arfcn;
    int wb_fcch_chan;

    int wb_chan1;
*/
    int width8;


};

#endif /* SDRCONFIGURATIONSTRUCT_H_ */
