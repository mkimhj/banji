#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "gpio.h"
#include "i2c.h"

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

void i2c_init(void)
{
  ret_code_t err_code;

  const nrf_drv_twi_config_t twi_config = {
      .scl = I2C_SCL_PIN,
      .sda = I2C_SDA_PIN,
      .frequency = NRF_DRV_TWI_FREQ_400K,
      .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
      .clear_bus_init = false};

  err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
  APP_ERROR_CHECK(err_code);

  nrf_drv_twi_enable(&m_twi);
}

void i2c_scan(void)
{
  ret_code_t err_code;
  uint8_t sample_data;
  bool detected_device = false;

  for (uint8_t address = 1; address <= TWI_ADDRESSES; address++)
  {
    err_code = nrf_drv_twi_rx(&m_twi, address, &sample_data, sizeof(sample_data));
    if (err_code == NRF_SUCCESS)
    {
      detected_device = true;
      NRF_LOG_INFO("TWI device detected at address 0x%x.", address);
    }
  }

  if (!detected_device)
  {
    NRF_LOG_INFO("No device was found.");
  }
}