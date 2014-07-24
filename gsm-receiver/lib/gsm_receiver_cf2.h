/* -*- c++ -*- */
/*
 * @file
 * @author Piotr Krysik <pkrysik@stud.elka.pw.edu.pl>
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_GSM_RECEIVER_CF2_H
#define INCLUDED_GSM_RECEIVER_CF2_H

#include <vector>
#include <list>
#include <gnuradio/block.h>
#include <gnuradio/types.h>
#include <gnuradio/feval.h>
#include <gsm_constants.h>
#include <gsm_receiver_config.h>
#include "libnetwork/tcpclientdata.h"
#include <SDRconfiguration.h>

#include <decoder/gsmstack.h> //TODO: remember to remove this line in the future!
#include "GSML1FEC.h" //!!
#include <string>//!!
#include <map>//!!
#define double_downlike 0
#define single_downlike 1
#define N_TCH_DECODER 7 /* for TS1..TS7 */
#define NORM_CORR_THRESHOLD 0.80
#define MAX_CORR_DIST   7	// 4 + 3 =  1/2 GUARD + TAIL



class gsm_receiver_cf2;

typedef boost::shared_ptr<gsm_receiver_cf2> gsm_receiver_cf2_sptr;
typedef std::vector<gr_complex> vector_complex;


struct SCAN_BTS
{
	int scan_flag;

	int fcch_found;
	int fcch_timeout;
	int fcch_count;
	int fcch_unfcount;

	int sch_found;
	int sch_timeout;
	int sch_count;
	int sch_unfcount;

	int scan_count;

	int d_freq_overflow;
	float d_freq_offset_p;


};



DLL_PUBLIC gsm_receiver_cf2_sptr gsm_make_receiver_cf2(gr::feval_dd *tuner, gr::feval_dd *synchronizer, int osr, \
		std::string key, std::string configuration, SDRconfiguration sdr_config
		);

/** GSM Receiver GNU Radio block
 *
 * GSM Receiver class supports frequency correction, synchronisation and
 * MLSE (Maximum Likelihood Sequence Estimation) estimation of synchronisation
 * bursts and normal bursts.
 * \ingroup block
 */

class gsm_receiver_cf2 : public gr::block
{
  private:

	//struct RECconfiguration SDRconfig;

    std::map<char,int> d_hex_to_int;
    uint8_t d_KC[8]; //!!
//    GSM::TCHFACCHL1Decoder *d_tch_decoder[N_TCH_DECODER]; //!!
    bool d_trace_sch;

    bool d_write_bursts;

    enum {
        TM_NONE,
        TM_SPEECH_FR,
        TM_SPEECH_EFR,
    } d_tch_mode;

    int server_fd;
    enum {
    	downlink, uplink, none
    } d_channel2_type;

    /**@name Configuration of the receiver */
    //@{
    const int d_OSR; ///< oversampling ratio
    const int d_chan_imp_length; ///< channel impulse length
    //@}

    gr_complex d_sch_training_seq[N_SYNC_BITS]; ///<encoded training sequence of a SCH burst
    gr_complex d_norm_training_seq[TRAIN_SEQ_NUM][N_TRAIN_BITS]; ///<encoded training sequences of a normal bursts and dummy bursts

    gr::feval_dd *d_tuner; ///<callback to a python object which is used for frequency tunning
    gr::feval_dd *d_synchronizer; ///<callback to a python object which is used to correct offset of USRP's internal clock

    /** Countes samples consumed by the receiver
     *
     * It is used in beetween find_fcch_burst and reach_sch_burst calls.
     * My intention was to synchronize this counter with some internal sample
     * counter of the USRP. Simple access to such USRP's counter isn't possible
     * so this variable isn't used in the "synchronized" state of the receiver yet.
     */
    unsigned d_counter;
    unsigned d_symbol_counter;

    /**@name Variables used to store result of the find_fcch_burst fuction */
    //@{
    unsigned d_fcch_start_pos; ///< position of the first sample of the fcch burst
    float d_freq_offset; ///< frequency offset of the received signal
    //@}
    std::list<double> d_freq_offset_vals;

    /**@name Identifiers of the BTS extracted from the SCH burst */
    //@{
    int d_ncc; ///< network color code
    int d_bcc; ///< base station color code
    //@}
   // float	corr_train_seq[10][N_TRAIN_BITS];
    int d_ch1_arfcn;
    int d_arfcn;
    int d_timeslot;
    int d_mfn;
    int d_fn;
    long sb_e,nb_e;
    int last_burst_start;
    std::string d_net_ip;
    int d_net_port;
    SDRconfiguration d_sdr_config;

    const unsigned char		*d_corr_pattern;
	float 			d_corr_max;

    /**@name Internal state of the gsm receiver */
    //@{
    enum states {
      first_fcch_search, next_fcch_search, sch_search, // synchronization search part
      synchronized // receiver is synchronized in this state
    } d_state;
    //@}

    /**@name Variables which make internal state in the "synchronized" state */
    //@{
    burst_counter d_burst_nr; ///< frame number and timeslot number
    channel_configuration d_channel_conf; ///< mapping of burst_counter to burst_type
    //@}
    
    unsigned d_failed_sch; ///< number of subsequent erroneous SCH bursts
    



    // GSM Stack
    GS_CTX d_gs_ctx;//TODO: remove it! it'a not right place for a decoder
    friend gsm_receiver_cf2_sptr gsm_make_receiver_cf2(gr::feval_dd *tuner, gr::feval_dd *synchronizer, int osr, std::string key, \
    		std::string configuration, SDRconfiguration sdr_config);

    gsm_receiver_cf2(gr::feval_dd *tuner, gr::feval_dd *synchronizer, int osr, std::string key, std::string configuration, \
    		SDRconfiguration sdr_config);

    /** Function whis is used to search a FCCH burst and to compute frequency offset before
     * "synchronized" state of the receiver
     *
     * TODO: Describe the FCCH search algorithm in the documentation
     * @param input vector with input signal
     * @param nitems number of samples in the input vector
     * @return
     */
    bool find_fcch_burst(const gr_complex *input, const int nitems);

    /** Computes frequency offset from FCCH burst samples
     *
     * @param input vector with input samples
     * @param first_sample number of the first sample of the FCCH busrt
     * @param last_sample number of the last sample of the FCCH busrt
     * @return frequency offset
     */
    double compute_freq_offset(const gr_complex * input, unsigned first_sample, unsigned last_sample);
    float correlate_pattern(const unsigned char *pattern,unsigned char *output_binary);

    /** Calls d_tuner's method to set frequency offset from Python level
     *
     * @param freq_offset absolute frequency offset of the received signal
     */
    void set_frequency(double freq_offset);


    // set scanbts flag call back
    void set_scanbts_flag(double freq_offset);

    /** Computes angle between two complex numbers
     *
     * @param val1 first complex number
     * @param val2 second complex number
     * @return
     */
    inline float compute_phase_diff(gr_complex val1, gr_complex val2);

    /** Function whis is used to get near to SCH burst
     *
     * @param nitems number of samples in the gsm_receiver's buffer
     * @return true if SCH burst is near, false otherwise
     */
    bool reach_sch_burst(const int nitems);

    /** Extracts channel impulse response from a SCH burst and computes first sample number of this burst
     *
     * @param input vector with input samples
     * @param chan_imp_resp complex vector where channel impulse response will be stored
     * @return number of first sample of the burst
     */
    int get_sch_chan_imp_resp(const gr_complex *input, gr_complex * chan_imp_resp);

    /** MLSE detection of a burst bits
     *
     * Detects bits of burst using viterbi algorithm.
     * @param input vector with input samples
     * @param chan_imp_resp vector with the channel impulse response
     * @param burst_start number of the first sample of the burst
     * @param output_binary vector with output bits
     */
    void detect_burst(const gr_complex * input, gr_complex * chan_imp_resp, int burst_start, unsigned char * output_binary);

    /** Encodes differentially input bits and maps them into MSK states
     *
     * @param input vector with input bits
     * @param nitems number of samples in the "input" vector
     * @param gmsk_output bits mapped into MSK states
     * @param start_point first state
     */
    void gmsk_mapper(const unsigned char * input, int nitems, gr_complex * gmsk_output, gr_complex start_point);

    /** Correlates MSK mapped sequence with input signal
     *
     * @param sequence MKS mapped sequence
     * @param length length of the sequence
     * @param input_signal vector with input samples
     * @return correlation value
     */
    gr_complex correlate_sequence(const gr_complex * sequence, int length, const gr_complex * input);

    /** Computes autocorrelation of input vector for positive arguments
     *
     * @param input vector with input samples
     * @param out output vector
     * @param nitems length of the input vector
     */
    inline void autocorrelation(const gr_complex * input, gr_complex * out, int nitems);

    /** Filters input signal through channel impulse response
     *
     * @param input vector with input samples
     * @param nitems number of samples to pass through filter
     * @param filter filter taps - channel impulse response
     * @param filter_length nember of filter taps
     * @param output vector with filtered samples
     */
    inline void mafi(const gr_complex * input, int nitems, gr_complex * filter, int filter_length, gr_complex * output);

    /**  Extracts channel impulse response from a normal burst and computes first sample number of this burst
     *
     * @param input vector with input samples
     * @param chan_imp_resp complex vector where channel impulse response will be stored
     * @param search_range possible absolute offset of a channel impulse response start
     * @param bcc base station color code - number of a training sequence
     * @return first sample number of normal burst
     */
    int get_norm_chan_imp_resp(const gr_complex * input, gr_complex * chan_imp_resp, int bcc);
    bool process_network_burst(int fn, int arfcn, int timeslot, unsigned char* burst_binary);
    void synchronize(const gr_complex *input,const gr_complex *in_1,int *to_consume);
    void process_normal_burst(burst_counter burst_nr, const unsigned char * burst_binary, bool first_burst);
    burst_type get_burst_type(int uchannel, int mfn, int timeslot);

    /**
     *
     */
    //void configure_receiver();
    
    void consume_each(int how_many_items);
    void diff_encode(const float *in,float *out,int length,float lastbit= 1.0);

  public:
	bool d_ts_ch0;
	bool d_ts_ch1;
    struct SCAN_BTS scan_bts_flag;// SCAN BTS value

    ~gsm_receiver_cf2();
    void forecast(int noutput_items, gr_vector_int &ninput_items_required);
    int general_work(int noutput_items,
                     gr_vector_int &ninput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GSM_RECEIVER_CF2_H */
