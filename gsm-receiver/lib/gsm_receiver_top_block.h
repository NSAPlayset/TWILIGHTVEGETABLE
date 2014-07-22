/*
 * gsm_receiver_top_block.h
 *
 *  Created on: 2012��10��22��
 *      Author: hhh
 */

#ifndef GSM_RECEIVER_TOP_BLOCK_H_
#define GSM_RECEIVER_TOP_BLOCK_H_

// standard lib
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


// GNURadio lib
#include <gnuradio/top_block.h>
#include <gnuradio/uhd/usrp_source.h>
#include <gnuradio/blocks/file_sink.h>
#include <gnuradio/filter/freq_xlating_fir_filter_ccf.h>
#include <gnuradio/blocks/file_source.h>
#include <gnuradio/filter/fractional_interpolator_cc.h>
#include <gnuradio/feval.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/blocks/vector_to_stream.h>
#include <gnuradio/blocks/deinterleave.h>
#include <gnuradio/filter/fir_filter_ccf.h>
#include <gnuradio/blocks/null_sink.h>
#include <uhd/usrp/subdev_spec.hpp>
#include <complex.h>

// gsm receiver lib
#include <gsm_receiver_cf.h>

// SDR configuration lib
#include <SDRini.h>
#include <SDRconfiguration.h>


#include "libbase64cc/base64.h"

#define FREQ_OFFSET 0



//declear some class
class gsm_receiver_top_block;


// gsm receiver top class
typedef boost::shared_ptr<gsm_receiver_cf> gsm_receiver_cf_sptr;

//typedef boost::shared_ptr<gsm_receiver_top> gsm_receiver_top_sptr;

typedef boost::shared_ptr<gsm_receiver_top_block> gsm_receiver_top_block_sptr;


gsm_receiver_top_block_sptr make_gsm_receiver_top_block(
									struct SDRconfiguration sdr_config
												  );


class gsm_receiver_top_block : public gr::top_block {
	public:
		gsm_receiver_top_block(struct SDRconfiguration sdr_config);
        gr::block_sptr d_head;
        gr::block_sptr d_dst;
		~gsm_receiver_top_block();


	    struct SDRconfiguration d_sdr_config;
		gsm_receiver_cf_sptr gsm_receiver;


        gr::block_sptr set_sink(const std::string &outpufile);
        uhd::usrp::subdev_spec_t	str_to_subdev(std::string spec_str);
        gr::blocks::file_source::sptr set_source(const std::string &inputfile);
        gr::uhd::usrp_source::sptr set_usrp_source(void);
        gr::filter::freq_xlating_fir_filter_ccf::sptr set_filter(void);
        gr::filter::fractional_interpolator_cc::sptr set_interpolator(void);
        gr::blocks::vector_to_stream::sptr set_convert(void);

		bool timing(float center_freq);
		bool set_usrp_freq(double chan0_freq, double chan1_freq, double offset, class gsm_receiver_top_block *p);

		double get_freq_from_arfcn(int chan);
		bool set_double_chan_center_frequency(float center_freq);
		bool set_double_filter(void);

		bool scan_bts_next_arfcn(class gsm_receiver_top_block *p);



		gsm_receiver_cf_sptr set_gsm_receiver_cf(
                                                        gr::feval_dd *tuner,
                                                        gr::feval_dd *synchronizer,
														std::string key,
														std::string configuration,
														struct SDRconfiguration sdr_config
														);


	private:

		struct _rates_element{
			float input_rate;
			float clock_rate;
			float gsm_symb_rate;
			float sps;
		}rates_element;
		struct _filter_parameter{
			float filter_cutoff;
			float filter_t_width;
			float offset;
			std::vector<float> fliter_taps;
            gr::filter::freq_xlating_fir_filter_ccf::sptr _filter;
		}filter_parameter;

		std::string grc_config;
        gr::uhd::usrp_source::sptr usrp_source;
        gr::blocks::file_source::sptr file_source[2];
        gr::blocks::deinterleave::sptr deinter;
        gr::filter::fir_filter_ccf::sptr my_swdecim[2];
        gr::filter::freq_xlating_fir_filter_ccf::sptr filter[2];
        gr::filter::fractional_interpolator_cc::sptr interpolator[2];

        db_base::sptr subdev[2];
		class synchronizer *synchronizer_p;
		class tuner *tuner_p;


};


//callback class

class tuner : public gr::feval_dd
{
private:
	class  gsm_receiver_top_block *_p;

public:
	tuner(class gsm_receiver_top_block *p);
	double eval(double x);
};

class synchronizer : public gr::feval_dd
{
private:
	class gsm_receiver_top_block *_p;

public:
	synchronizer(class gsm_receiver_top_block *p);
	double eval(double x);
};



#endif /* GSM_RECEIVER_TOP_BLOCK_H_ */



