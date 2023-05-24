/*
 * HM01B0_CAPTURE.c
 *
 *  Created on: Nov 25, 2018
 *      Author: Ali Najafi
 */

#include "HM01B0_CAPTURE.h"
#include "gpio.h"
#include "i2c.h"
#include "HM01B0_LVLD_TIMER.h"
#include "nrf_delay.h"

uint32_t line_count;
uint8_t image_rd_done = 0;
uint8_t image_frame_done = 0;
bool m_stream_mode_active;
uint16_t total_image_size;

void cameraPowerEnable(void){
  nrf_gpio_cfg(
    CAM_POWER,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_pin_clear(CAM_POWER);
  nrf_delay_ms(POR_DELAY);
  nrf_gpio_pin_set(CAM_POWER);
  nrf_delay_ms(POR_DELAY);
}

void hm01b0_init(void){
  /*Test if camera is functional*/
  ble_bytes_sent_counter = 0;
  i2cWrite16(CAMERA_I2C_ADDR, REG_MODE_SELECT,0x00);
  uint8_t version = i2cRead16(CAMERA_I2C_ADDR, REG_MODEL_ID_L);
  if(version != 0xB0){
      printf("REG_MODEL_ID_L: %x \n", version);
      printf("Camera version problem \n");
  }
  i2cWrite16(CAMERA_I2C_ADDR,  REG_MODE_SELECT, 0x00);//go to stand by mode

  /*Initialize and set high the SPI chip select*/
  nrf_gpio_cfg_output(CAM_SPI_CS);//Set up the chip select for SPI
  nrf_gpio_pin_set(CAM_SPI_CS);

  /*Camera settings initialization*/
  hm01b0_init_fixed_rom_qvga_fixed();
}

/*Deactivates the peripherals that change the power consumption*/
void hm_peripheral_uninit(void)
{
  spiSlaveDeInit();
}

void hm_peripheral_init(void)
{
  hm_clk_out();

  /*Initialize the GPIO settings: Frame valid for */
  gpio_setting_init();

  /*Initialize the I2C for camera*/
  nrf_delay_ms(100);

  hm01b0_init();

  lvld_timer_init();

  #if(CAM_CLK_GATING == 1)
  nrf_drv_timer_disable(&CAM_TIMER);
  #endif
}

/*Activates the peripherals that change the power consumption, when connected and before taking picture*/
void hm_peripheral_connected_init(void){
  spiSlaveInit();
}

void hm_single_capture_spi_832_stream(void){
  spiSlaveSetRxDone(0);

  /*SPI registers initilization*/
  spiSlaveSetTransferDone(false);

  spiSlaveSetBuffers();

  /*Camera values initialized*/
  image_rd_done = 0;

  /*Enable the FRAME VALID interrupt*/
  nrf_drv_gpiote_in_event_enable(CAM_FRAME_VALID, true);

  i2cWrite16(CAMERA_I2C_ADDR,  REG_MODE_SELECT, 0x01);

  spiSlaveSetRxDone(spiSlaveGetRxLength());
}


void hm_single_capture_spi_832(void)
{
  #if(CAM_CLK_GATING == 1)
  nrf_drv_timer_enable(&CAM_TIMER);
  #endif
  //nrf_delay_ms(100);
  spiSlaveSetRxDone(0);
  ble_bytes_sent_counter = 0;
  line_count = 0;

  /*SPI registers initilization*/
  spiSlaveSetTransferDone(false);

  spiSlaveSetBuffers();

  image_rd_done = 0;
  image_frame_done = 0;
  ble_bytes_sent_counter = 0;

  /*Enable the FRAME VALID interrupt*/
  nrf_drv_gpiote_in_event_enable(CAM_FRAME_VALID, true);

  lvld_timer_enable();
  nrf_gpio_pin_clear(CAM_SPI_CS);

  i2cWrite16(CAMERA_I2C_ADDR,  REG_MODE_SELECT, capture_mode);//If we use the 0x03 mode for single capture, the power of camera stays high after capturing one frame

  while (image_rd_done != 1) {
    __WFE();
  };
  while (!spiSlaveGetTransferDone()) {
    __WFE();
  };
  spiSlaveSetTransferDone(false);

  spiSlaveSetRxDone(spiSlaveGetRxLength());
}

void hm_single_capture(void)
{
  ble_bytes_sent_counter = 0;

  hm01b0_init_fixed_rom_qvga_fixed();

  /*Capture and stream one frame out*/
  i2cWrite16(CAMERA_I2C_ADDR, REG_MODE_SELECT, capture_mode);
}

void hm_single_capture_spi(void)
{
  spiSlaveSetRxDone(0);
  ble_bytes_sent_counter = 0;

  /*SPI registers initilization*/
  spiSlaveSetTransferDone(false);
  spiSlaveSetBuffers();

  nrf_gpio_pin_clear(CAM_SPI_CS);

  ble_bytes_sent_counter = 0;

  /*Enable the FRAME VALID interrupt*/
  nrf_drv_gpiote_in_event_enable(CAM_FRAME_VALID, true);

  /*Capture and stream one frame out*/
  i2cWrite16(CAMERA_I2C_ADDR,  REG_MODE_SELECT, capture_mode);

  while (!spiSlaveGetTransferDone()) { __WFE(); };
  spiSlaveSetRxDone(spiSlaveGetRxLength());
}

