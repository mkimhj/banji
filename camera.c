#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <nrfx.h>

#include "HM01B0_CAPTURE.h"
#include "HM01B0_SPI.h"
#include "camera.h"
#include "gpio.h"

uint32_t image_size;
static bool cameraInitialized = false;

void cameraInit(void)
{
  uint32_t img_data_length = 0;
  uint8_t img_data_buffer[255];
  if (!cameraInitialized) {
    hm_reset_capture_done();

    // TODO: ENABLE CAMERA POWER HERE
    // something like // cameraEnablePower();

    hm_peripheral_init();

    // TODO: This initialized the SPI slave, we should move this to when the BLE connection establishes
    hm_peripheral_connected_init();

    NRF_LOG_INFO("[camera] initialized");
    cameraInitialized = true;
  } else {
    NRF_LOG_INFO("[camera] already initialized, skipping");
  }
}

void cameraDeInit(void)
{
  cameraInitialized = false;
  // TODO: Fill this out
}

void cameraCaptureFrame(void)
{
  // TODO: From Ali's code, not sure why we need this
  // if (ble_bytes_sent_counter >= spiSlaveGetRxDone())
  // {
    NRF_LOG_INFO("Starting capture...");

    hm_single_capture_spi_832();

    NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(spiSlaveGetRxLength()));

    // Queue event to send image upstream here
    // ble_its_img_info_t image_info;
    // image_info.file_size_bytes = m_length_rx;
    // ble_its_img_info_send(&m_its, &image_info);
  // }
}