/*
 * SDRconfiguration.h
 *
 *  Created on: 2012��10��21��
 *      Author: hhh
 */

#ifndef SDRCONFIGURATION_H_
#define SDRCONFIGURATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string.h>
#include "SDRconfigurationStruct.h"

extern int ini_handler(void* user, const char* section, const char* name, const char* value);

extern void print_sdrconf(struct SDRconfiguration sdr_config);


#endif /* SDRCONFIGURATION_H_ */


