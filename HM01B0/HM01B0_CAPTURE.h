/*
 * HM01B0_CAPTURE.h
 *
 *  Created on: Nov 25, 2018
 *      Author: Ali Najafi
 */

#ifndef HM01B0_CAPTURE_H_
#define HM01B0_CAPTURE_H_
//


#include "HM01B0_GPIO.h"
#include "HM01B0_FUNC.h"
#include "HM01B0_CLK.h"
#include "HM01B0_SPI.h"
#include "HM01B0_BLE_DEFINES.h"
#include "gpio.h"

extern uint32_t line_count;
extern uint8_t image_rd_done;

extern uint32_t ble_bytes_sent_counter;

void cameraPowerEnable(void);

void hm01b0_init(void);

void hm_peripheral_uninit(void);

void hm_peripheral_init(void);
void hm_peripheral_connected_init(void);

void hm_single_capture(void);


void hm_single_capture_spi(void);

void hm_single_capture_spi_832(void);

void hm_single_capture_spi_832_stream(void);

void hm_single_capture_spi_832_compressed(void);


#endif /* HM01B0_CAPTURE_H_ */