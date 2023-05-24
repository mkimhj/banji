/*
 * HM01B0_SPI.c
 *
 *  Created on: Nov 30, 2018
 *      Author: Ali Najafi
 */


#include "HM01B0_CAPTURE.h"
#include "HM01B0_SPI.h"
#include "../gpio.h"

#include <nrfx_spis.h>

nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG;

/*******************************************************************************
 * Code SPI
 ******************************************************************************/

/**
 * @brief SPIS user event handler.
 *
 * @param event
 */
void spis_event_handler(nrf_drv_spis_event_t event)
{
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE)
    {
        spis_xfer_done = true;
        //NRF_LOG_INFO(" Transfer completed. Received: %s",(uint32_t)m_rx_buf);
    }
}

void spis_pin_set(void)
{
    spis_config.csn_pin               = CAM_SPI_CS;
    spis_config.miso_pin              = CAM_MISO;
    spis_config.mosi_pin              = CAM_MOSI;
    spis_config.sck_pin               = CAM_SCLK;
}

void spi_init(void)
{
    // Enable the constant latency sub power mode to minimize the time it takes
    // for the SPIS peripheral to become active after the CSN line is asserted
    // (when the CPU is in sleep mode).
//    NRF_POWER->TASKS_CONSTLAT = 1;

//    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
//    NRF_LOG_DEFAULT_BACKENDS_INIT();

    APP_ERROR_CHECK(nrf_drv_spis_init(&spis, &spis_config, spis_event_handler));

//    printf('%d',sizeof(m_tx_buf));
//    memset(m_rx_buf, 0, m_length_rx);
    spis_xfer_done = false;
    m_length_rx_done = 0;

    APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
    //*((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)spis_task)) = 0x1UL;
}