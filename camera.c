#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <nrfx.h>

#include "HM01B0_CAPTURE.h"
#include "HM01B0_SPI.h"
#include "camera.h"

uint32_t image_size;

void cameraInit(void)
{
  uint32_t img_data_length = 0;
  uint8_t img_data_buffer[255];
  hm_reset_capture_done();

  // ENABLE CAMERA POWER HERE
  // something like // cameraEnablePower();

  hm_peripheral_init();
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