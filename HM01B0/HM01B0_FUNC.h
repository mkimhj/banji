/*
 * HM01B0_FUNC.h
 *
 *  Created on: Nov 15, 2018
 *      Author: Ali
 */

#ifndef HM01B0_FUNC_H_
#define HM01B0_FUNC_H_

#include <stdbool.h>

#include "i2c.h"
#include "HM01B0Regs.h"

//FUNCTIONS----------------------------------------------------------------------------------------------------------------------------//
void hm01b0_init_fixed_rom_qvga_fixed(void);
void hm01b0_init_fixed_rom_qvga_fixed_acc(void);
void hm01b0_init_fixed_rom_qvga_fixed_maxfps(void);
void hm01b0_init_brighter(void);
void hm01b0_init_datasheet_default(void);

#endif /* HM01B0_FUNC_H_ */
