#include <stdbool.h>

#include "main.h"
#include "event.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "gpio.h"

void gpioInit(void)
{
  ret_code_t err_code;

  err_code = nrf_drv_gpiote_init();
  APP_ERROR_CHECK(err_code);

  // gpioOutputEnable(GPIO_1_PIN);
  // gpioWrite(GPIO_1_PIN, 0);
  // gpioOutputEnable(GPIO_3_PIN);
  // gpioWrite(GPIO_3_PIN, 0);
  // gpioOutputEnable(BLE_LED_PIN);
  // gpioWrite(BLE_LED_PIN, 0);

  nrf_gpio_cfg(
      CAM_SPI_CS,
      NRF_GPIO_PIN_DIR_OUTPUT,
      NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_PULLUP,
      NRF_GPIO_PIN_S0S1,
      NRF_GPIO_PIN_NOSENSE);
}

void gpioOutputEnable(gpioPin_t pin)
{
  gpioOutput_t outputConfig = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(0);
  nrf_drv_gpiote_out_init(pin, &outputConfig);
}

void gpioDisable(gpioPin_t pin)
{
  nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_NOPULL);
}

void gpioWrite(gpioPin_t pin, uint8_t value)
{
  if (value) {
    nrfx_gpiote_out_set(pin);
  } else {
    nrfx_gpiote_out_clear(pin);
  }
}
