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

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"			// the common stuff

/* %include "gsm_constants.h" */
%include "SDRconfigurationStruct.h"
#define NPGSM_UDP_PORT 29653
%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include <gnuradio/py_feval.h>
#include "gsm_receiver_cf.h"
#include "gsm_receiver_cf2.h"
#include "SDRconfigurationStruct.h"
#include <stdexcept>
/* #include "gsm_constants.h" */
%}

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(gsm,receiver_cf);

gsm_receiver_cf_sptr gsm_make_receiver_cf ( gr::feval_dd *tuner, gr::feval_dd *synchronizer, int osr, int c0pos, std::string ma, int maio, int hsn, std::string key, std::string configuration, bool primary);

class gsm_receiver_cf : public gr::block
{
private:
  gsm_receiver_cf ( gr::feval_dd *tuner, gr::feval_dd *synchronizer, int osr, int c0pos, std::string ma, int maio, int hsn, std::string key, std::string configuration, bool primary);
};

GR_SWIG_BLOCK_MAGIC(gsm,receiver_cf2);

gsm_receiver_cf2_sptr gsm_make_receiver_cf2 (gr::feval_dd *tuner, gr::feval_dd *synchronizer, int osr, std::string key, std::string configuration, SDRconfiguration sdr_config);
class gsm_receiver_cf2 : public gr::block
{
private:
  gsm_receiver_cf2 ( gr::feval_dd *tuner, gr::feval_dd *synchronizer, int osr, std::string key, std::string configuration, SDRConfiguration sdr_config);
};

