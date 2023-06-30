/*
 * HM01B0_LVLD_TIMER.h
 *
 *  Created on: April 25, 2019
 *      Author: Ali Najafi
 */

#include "HM01B0_LVLD_TIMER.h"
#include "ble_manager.h"
#include "gpio.h"

uint32_t lvld_timer_val = LVLD_TIMER_VALUE;

const nrf_drv_timer_t TIMER_LVLD = NRF_DRV_TIMER_INSTANCE(4);

void lvld_timer_enable(void)
{
  nrf_drv_timer_enable(&TIMER_LVLD);
}

void lvld_timer_disable(void)
{
  nrf_drv_timer_disable(&TIMER_LVLD);
}

/**
 * @brief Handler for timer events.
 */
void timer_lvld_event_handler(nrf_timer_event_t event_type, void* p_context)
{
  if (line_count < IMAGE_HEIGHT)
  {
    lvld_timer_disable();
    NRF_GPIO->OUTSET = 1UL << CAM_SPI_CS_OUT;
    spiSlaveSetBuffersBackWithLineCount(line_count);
    nrf_drv_gpiote_in_event_enable(CAM_LINE_VALID, true);
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

      #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1))
          nrf_drv_gpiote_in_event_enable(CAM_FRAME_VALID, true);
          spiSlaveSetBuffers();
      #endif
    #endif
    hm_set_capture_done();
  }
}

void lvld_timer_init(void)
{
  uint32_t err_code = NRF_SUCCESS;
  //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
  err_code = nrf_drv_timer_init(&TIMER_LVLD, &timer_cfg, timer_lvld_event_handler);
  APP_ERROR_CHECK(err_code);

  nrf_drv_timer_extended_compare(
        &TIMER_LVLD, NRF_TIMER_CC_CHANNEL4, lvld_timer_val, NRF_TIMER_SHORT_COMPARE4_CLEAR_MASK, true);
}

/** @} */
