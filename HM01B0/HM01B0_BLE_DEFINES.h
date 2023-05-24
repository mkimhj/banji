/*
 * HM01B0_BLE_DEFINES.h
 *
 *  Created on: Apr 05, 2019
 *      Author: Ali Najafi
 */

#include <stdint.h>

//Got from HM01B0_SPI.h +++++++++++++++++++++++++++++++++++++++++
#ifndef HM01B0_BLE_DEFINES_H_
#define HM01B0_BLE_DEFINES_H_

//Code shortcuts+++++++++++++++++++++++++++++++++++++++++++++++++
//#define spi_buffer_size 33000
#define spi_buffer_size 100
#define total_spi_buffer_size_max 32000
#define total_spi_buffer_size 10000
extern uint8_t LINE_NUM;
extern uint16_t total_image_size;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//Got from HM01B0_CAPTURE.h +++++++++++++++++++++++++++++++++++++++++
/*if capture_mode=0x01 => it streams. This is useful for testing purposes*/
/*if capture_mode=0x03 => it takes frame_count shots. */
#define capture_mode  0x01
#define TEST_PATTERN 0x00
#define MEM_INIT_VALUE 0x11
#define CAM_FRAME_VALID_INT 1 // Use the frame valid in the code
#define CAM_CLK_GATING 1 // Turns off camera clock when not taking pictures
#define CAM_SINGLE_CAPTURE_POWER_FIX 0//after signle capture the camera power stays high for this reason I power cycle the camera each time
#define POR_DELAY 200
#define CAM_TURN_OFF_DELAY 100

//Got from HM01B0_I2C.h
#define SLAVE_ADDR 0x24U //address for camera
#define I2C_DATA_LENGTH 32U
/* TWI instance ID. */
#define TWI_INSTANCE_ID     0
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define I2C_RST_TIMER_VALUE 2000*8*2
#define I2C_RST_CNT_MAX 8

//Got from HM01B0_LVLD_TIMER.h +++++++++++++++++++++++++++++++++++++++++
#define LVLD_TIMER_VALUE (spi_buffer_size + 20)*8*2 //the second number in multiplication is equal to 64/cam_mclk_freq; if cam_mcl_freq=8MHz => 8
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//Timers used +++++++++++++++++++++++++++++++++++++++++
/*Timer 0 => BLE timing  NRF_DRV_TIMER_INSTANCE(0)
Timer 1 => camera clk NRF_DRV_TIMER_INSTANCE(1)
Timer 2 => I2C stuck reset timer!!   NRF_DRV_TIMER_INSTANCE(2)
Timer 3 => pwm 3  NRF_DRV_TIMER_INSTANCE(3)
Timer 4 => LVLD timer!!   NRF_DRV_TIMER_INSTANCE(4) */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//IRQ priorities +++++++++++++++++++++++++++++++++++++++++
/*Timer 0 => BLE timing
GPIOTE:5
LVLD Timer:3
SPIS_ENABLED: 4
NRFX_SPIS:4
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif /* HM01B0_BLE_DEFINES_H_ */