#!/usr/bin/env python
# Copyright 2014 Loki Coyote <loki@lokkju.com>
# Copyright 2012 Dimitri Stolnikov <horiz0n@gmx.net>
# Copyright 2012 Steve Markgraf <steve@steve-m.de>

# Adjust the center frequency (-f) and gain (-g) according to your needs.
# Use left click in Wideband Spectrum window to roughly select a GSM carrier.
# In Wideband Spectrum you can also tune by 1/4 of the bandwidth by clicking on
# the rightmost/leftmost spectrum side.
# Use left click in Channel Spectrum windows to fine tune the carrier by
# clicking on the left or right side of the spectrum.


import sys
from gnuradio import gr, blocks, filter
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import osmosdr

try:
    for extdir in ['../build/swig']:
        if extdir not in sys.path:
            sys.path.append(extdir)
    import npgsm_swig as npgsm
except:
    import npgsm


class tuner(gr.feval_dd):
    def __init__(self, top_block):
        gr.feval_dd.__init__(self)
        self.top_block = top_block

    def eval(self, freq_offet):
        print 'Set Frequency', freq_offet
        self.top_block.set_center_frequency(-freq_offet)
        return freq_offet


class synchronizer(gr.feval_dd):
    def __init__(self, top_block):
        gr.feval_dd.__init__(self)
        self.top_block = top_block

    def eval(self, timing_offset):
        self.top_block.set_timing(timing_offset)
        return freq_offet


def get_freq_from_arfcn(chan, region):
    # P/E/R-GSM 900
    if chan >= 0 and chan <= 124:
        freq = 890 + 0.2 * chan + 45

    #GSM 850
    elif chan >= 128 and chan <= 251:
        freq = 824.2 + 0.2 * (chan - 128) + 45

    #GSM 450
    elif chan >= 259 and chan <= 293:
        freq = 450.6 + 0.2 * (chan - 259) + 10

    #GSM 480
    elif chan >= 306 and chan <= 340:
        freq = 479 + 0.2 * (chan - 306) + 10

    #DCS 1800
    elif region is "e" and chan >= 512 and chan <= 885:
        freq = 1710.2 + 0.2 * (chan - 512) + 95

    #DCS 1900
    elif region is "u" and chan >= 512 and chan <= 810:
        freq = 1850.2 + 0.2 * (chan - 512) + 80

    #E/R-GSM 900
    elif chan >= 955 and chan <= 1023:
        freq = 890 + 0.2 * (chan - 1024) + 45

    else:
        freq = 0

    return freq * 1e6


def get_arfcn_from_freq(freq, region):
    freq /= 1e6
    arfcn = 0
    # GSM 450
    if freq <= 450.6 + 0.2 * (293 - 259) + 10:
        arfcn = ((freq - (450.6 + 10)) / 0.2) + 259
    # GSM 480
    elif freq <= 479 + 0.2 * (340 - 306) + 10:
        arfcn = ((freq - (479 + 10)) / 0.2) + 306
    # GSM 850
    elif freq <= 824.2 + 0.2 * (251 - 128) + 45:
        arfcn = ((freq - (824.2 + 45)) / 0.2) + 128
    # E/R-GSM 900
    elif freq <= 890 + 0.2 * (1023 - 1024) + 45:
        arfcn = ((freq - (890 + 45)) / -0.2) + 955
    # GSM 900
    elif freq <= 890 + 0.2 * 124 + 45:
        arfcn = (freq - (890 + 45)) / 0.2
    else:
        if region is "u":
            if freq > 1850.2 + 0.2 * (810 - 512) + 80:
                arfcn = 0
            else:
                arfcn = (freq - (1850.2 + 80) / 0.2) + 512
        elif region is "e":
            if freq > 1710.2 + 0.2 * (885 - 512) + 95:
                arfcn = 0
            else:
                arfcn = (freq - (1710.2 + 95) / 0.2) + 512
        else:
            arfcn = 0

    return arfcn


class top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        # Variables
        options = self.get_options()

        self.src = osmosdr.source(options.args)
        self.src.set_center_freq(options.frequency)
        self.src.set_freq_corr(options.ppm)
        self.src.set_sample_rate(int(options.sample_rate))

        if options.gain is None:
            self.src.set_gain_mode(1)
        else:
            self.src.set_gain_mode(0)
            self.src.set_gain(options.gain)

        # may differ from the requested rate
        sample_rate = self.src.get_sample_rate()
        sys.stderr.write("sample rate: %d\n" % sample_rate)

        gsm_symb_rate = 1625000.0 / 6.0
        sps = sample_rate / gsm_symb_rate / options.osr

        # configure channel filter
        filter_cutoff = 145e3  # 135,417Hz is GSM bandwidth
        filter_t_width = 10e3
        offset = 0.0

        filter_taps = filter.firdes.low_pass(1.0, sample_rate, filter_cutoff, filter_t_width, filter.firdes.WIN_HAMMING)

        self.gr_null_sink = blocks.null_sink(568) # gr.sizeof_gr_complex*1

        self.filter = filter.freq_xlating_fir_filter_ccf(1, filter_taps, offset, sample_rate)
        self.interpolator = filter.fractional_resampler_cc(0, sps)
        self.tuner_callback = tuner(self)
        self.synchronizer_callback = synchronizer(self)

        print ">>>>>Input rate: ", sample_rate

        sdr_config = npgsm.SDRconfiguration()
        sdr_config.decim = 96      #set fgpa decimation rate to DECIM
        sdr_config.freq = 891000000  #set input frequency to FREQ
        sdr_config.osr = 4         #set oversample rate
        sdr_config.ch0_infile = "/home/nick/my_project/gnuradio/cfile/TD-60-32-20120901-145641_d.cfile"
        sdr_config.ch1_infile =  "/home/nick/my_project/gnuradio/cfile/TD-60-32-20120901-145641_d.cfile"
        sdr_config.ch0_arfcn = 297 #set ch0_arfcn
        sdr_config.rxa_gain = 30
        sdr_config.rxb_gain = 30

        sdr_config.fcch_ch = 0

        sdr_config.is_double_downlink = 0 # using two downlink
        sdr_config.realtime = 1
        sdr_config.which_board = 0


        # for bts scanner settings
        sdr_config.sch_timeout_boundary = 10
        sdr_config.fcch_timeout_boundary = 50
        sdr_config.width8 = 0
        sdr_config.ch0_upload =1
        sdr_config.ch1_upload =1
        sdr_config.server = "127.0.0.1"
        sdr_config.port = 8000

        self.receiver = npgsm.receiver_cf2(
            self.tuner_callback,
            self.synchronizer_callback,
            options.osr,
            options.key.replace(' ', '').lower(),
            options.configuration.upper(),
            sdr_config)

        self.connect((self.src, 0), (self.filter, 0), (self.interpolator, 0), (self.receiver, 0))

    def set_center_frequency(self, center_freq):
        self.filter.set_center_freq(center_freq)
        pass

    def set_timing(self, timing_offset):
        pass

    def set_freq_frequency(self, freq):
        #TODO: for wideband processing, determine if the desired freq is within our current sample range.
        #		If so, use the frequency translator to tune.  Tune the USRP otherwise.
        #		Maybe have a flag to force tuning the USRP?
        if self.print_status:
            print >> sys.stderr, "Setting filter center freq to offset: ", self.offset, "\n"
        print self.offset

        self.filter.set_center_freq(self.offset)
        #print "set_freq......."
        return True

    ####################

    @staticmethod
    def get_options():
        parser = OptionParser(option_class=eng_option)
        parser.add_option("-a", "--args", type="string", default="",
                          help="gr-osmosdr device arguments")
        parser.add_option("-s", "--sample-rate", type="eng_float", default=1000000,
                          help="set receiver sample rate (default 1000000)")
        parser.add_option("-f", "--frequency", type="eng_float", default=891.0e6,
                          help="set receiver center frequency (default: 891Mhz)")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set receiver gain")
        parser.add_option("--ppm", type="eng_float", default=0, help="set ppm correction")

        # channel hopping related settings
        parser.add_option("-r", "--osr", type="int", default=4, help="Oversampling ratio [default=%default]")



        # demodulator related settings
        parser.add_option("-o", "--output-file", type="string", default="cfile2.out", help="specify the output file")
        parser.add_option("-v", "--verbose", action="store_true", default=False, help="dump demodulation data")
        parser.add_option("-k", "--key", type="string", default="00 00 00 00 00 00 00 00", help="KC session key")
        parser.add_option("-c", "--configuration", type="string", default="0B", help="Decoder configuration")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        return (options)


if __name__ == '__main__':
    tb = top_block()
    tb.run()
