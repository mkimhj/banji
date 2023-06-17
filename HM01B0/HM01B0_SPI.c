/*
 * HM01B0_SPI.c
 *
 *  Created on: Nov 30, 2018
 *      Author: Ali Najafi
 */


#include "HM01B0_CAPTURE.h"
#include "HM01B0_SPI.h"
#include "gpio.h"

#include <nrfx_spis.h>

nrfx_spis_t spiSlaveInstance = NRF_DRV_SPIS_INSTANCE(SPI_SLAVE_BUS); /**< SPIS instance. */
static bool transferDone = false;

static uint8_t m_tx_buf[1] = {0};                         /**< TX buffer. */
static uint8_t m_rx_buf[total_spi_buffer_size_max + 200]; /**< RX buffer. 200 added for the ACC and Mag data */
static uint16_t m_length_rx;                              /**< Transfer length. */
static uint16_t m_length_rx_done;                         /**< Transfer length. */
static uint8_t m_length_tx = 0;                           /**< Transfer length. */

nrfx_spis_config_t spiSlaveConfig = {
  .miso_pin = CAM_MISO,
  .mosi_pin = CAM_D0,
  .sck_pin = CAM_PCLK_OUT,
  .csn_pin = CAM_SPI_CS_IN,
  .mode = NRF_SPIS_MODE_0,
  .bit_order = NRF_SPIS_BIT_ORDER_MSB_FIRST,
  .csn_pullup = NRFX_SPIS_DEFAULT_CSN_PULLUP,
  .miso_drive = NRFX_SPIS_DEFAULT_MISO_DRIVE,
  .def = NRFX_SPIS_DEFAULT_DEF,
  .orc = NRFX_SPIS_DEFAULT_ORC,
  .irq_priority = NRFX_SPIS_DEFAULT_CONFIG_IRQ_PRIORITY,
};

void spiSlaveSetRxDone(uint16_t value)
{
  m_length_rx_done = value;
}

bool spiSlaveGetRxDone(void)
{
  return m_length_rx_done;
}

uint16_t spiSlaveGetRxLength(void)
{
  return m_length_rx;
}

void spiSlaveSetTransferDone(bool done)
{
  transferDone = done;
}

bool spiSlaveGetTransferDone(void)
{
  return transferDone;
}

void spiSlaveEventHandler(nrfx_spis_evt_t const* p_event, void* p_context)
{
  if (p_event->evt_type == NRFX_SPIS_XFER_DONE)
  {
    transferDone = true;
  }
}

void spiSlaveSetBuffers(void)
{
  APP_ERROR_CHECK(nrfx_spis_buffers_set(&spiSlaveInstance, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
}

void spiSlaveSetBuffersBackWithLineCount(uint32_t lineCount)
{
  nrfx_spis_buffers_set_back(&spiSlaveInstance, m_tx_buf, m_length_tx, m_rx_buf + lineCount * m_length_rx, m_length_rx);
}

void spiSlaveInit(void)
{
  APP_ERROR_CHECK(nrfx_spis_init(&spiSlaveInstance, &spiSlaveConfig, spiSlaveEventHandler, NULL));
  transferDone = false;
  spiSlaveSetRxDone(0);

  spiSlaveSetBuffers();
}

void spiSlaveDeInit(void)
{
  nrfx_spis_uninit(&spiSlaveInstance);
}

