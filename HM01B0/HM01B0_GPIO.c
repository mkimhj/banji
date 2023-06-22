/*
 * HM01B0_GPIO.c
 *
 *  Created on: Nov 20, 2018
 *      Author: Ali Najafi
 */
#include "HM01B0_GPIO.h"
#include "gpio.h"
#include "ble_manager.h"

void in_pin_handler_CAM_LINE_VALID(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  if (line_count < IMAGE_HEIGHT)
  {
    // here we need to activate SPI CS; enable the lvld_timer; and activate the CAM_LINE_VALID interrupt; increase the counter of lines
    lvld_timer_enable();
    NRF_GPIO->OUTCLR = 1UL << CAM_SPI_CS_OUT;
    line_count++;
  }
  else
  {
    nrf_drv_gpiote_in_event_disable(CAM_LINE_VALID);
    nrf_drv_gpiote_in_event_disable(CAM_FRAME_VALID);
    lvld_timer_disable();
    NRF_GPIO->OUTSET = 1UL << CAM_SPI_CS_OUT;

    #if (CAM_FRAME_VALID_INT == 1)
      spiSlaveSetRxDone(spiSlaveGetRxLength());
      bleSetPixelsSent(0);
    #endif
    hm_set_capture_done();
  }
}

void in_pin_handler_CAM_FRAME_VALID(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  nrf_drv_gpiote_in_event_disable(CAM_FRAME_VALID);
  line_count = 0;
  spiSlaveSetRxDone(0);
  lvld_timer_enable();
  nrf_gpio_pin_clear(CAM_SPI_CS_OUT);
}

void gpio_setting_uninit(void)
{
  nrf_drv_gpiote_in_uninit(CAM_FRAME_VALID);
  nrf_drv_gpiote_in_uninit(CAM_LINE_VALID);
}

void gpio_setting_init(void)
{
  ret_code_t err_code;

  /*Finds rising edge instead of just toggling*/
  nrf_drv_gpiote_in_config_t in_config_frmvld = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
  in_config_frmvld.pull = NRF_GPIO_PIN_NOPULL;

  err_code = nrf_drv_gpiote_in_init(CAM_FRAME_VALID, &in_config_frmvld, in_pin_handler_CAM_FRAME_VALID);
  APP_ERROR_CHECK(err_code);

  /*Finds falling for line valid edge instead of just toggling*/
  nrf_drv_gpiote_in_config_t in_config_lnvld = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
  in_config_lnvld.pull = NRF_GPIO_PIN_NOPULL;

  err_code = nrf_drv_gpiote_in_init(CAM_LINE_VALID, &in_config_lnvld, in_pin_handler_CAM_LINE_VALID);
  APP_ERROR_CHECK(err_code);
}
