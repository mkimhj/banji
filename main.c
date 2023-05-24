/*
 * banji
 * maruchi kim
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <nrfx.h>

#include "app_util_platform.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "fds.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "bsp_btn_ble.h"
#include "sensorsim.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_drv_clock.h"
#include "ble_manager.h"
#include "timers.h"
#include "event.h"
#include "gpio.h"
#include "accel.h"
#include "spi.h"
#include "i2c.h"
#include "flash.h"
#include "main.h"

#include "boards.h"
#include "nrf_ppi.h"
#include "nrf_timer.h"
#include "nrf_cli.h"
#include "nrf_cli_types.h"
#include "nrf_cli_libuarte.h"

#define CLI_EXAMPLE_LOG_QUEUE_SIZE (6)

NRF_CLI_LIBUARTE_DEF(m_cli_libuarte_transport, 512, 512);
NRF_CLI_DEF(m_cli_libuarte,
            "banji] ",
            &m_cli_libuarte_transport.transport,
            '\r',
            CLI_EXAMPLE_LOG_QUEUE_SIZE);

APP_TIMER_DEF(resetTimer);

static int16_t micData[PDM_DECIMATION_BUFFER_LENGTH];
static bool bleRetry = false;
static bool bleMicStreamRequested = false;
static uint32_t expectedBufferCount = 0;

static uint8_t metadataIndex = 0;
static uint8_t metadata[180] = { 0 };

accelGenericInterrupt_t accelInterrupt1 = {
  .pin = ACCEL_INT1,
  .source = ACCEL_INT_SOURCE_GENERIC1,
  .xEnable = false,
  .yEnable = false,
  .zEnable = true,
  .activity = true,
  .combSelectIsAnd = false,
  .threshold = 0x3,
  .duration = 0x7,
};

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
  app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static void resetTimerCallback(void * p_context)
{
  NVIC_SystemReset();
}

void powerEnterSleepMode(void)
{
  ret_code_t err_code;

  NRF_LOG_RAW_INFO("%08d [power] powering off...\n", systemTimeGetMs());

  err_code = bsp_indication_set(BSP_INDICATE_IDLE);
  APP_ERROR_CHECK(err_code);

  // Drive enable signals low before shutting down
  gpioOutputEnable(ACCEL_EN_PIN);
  gpioWrite(ACCEL_EN_PIN, 0);

  spiDeInit();
  delayMs(1);

  // Prepare wakeup buttons.
  err_code = bsp_btn_ble_sleep_mode_prepare();
  APP_ERROR_CHECK(err_code);

  // Go to system-off mode (this function will not return; wakeup will cause a reset).
  err_code = sd_power_system_off();
  APP_ERROR_CHECK(err_code);
}

static void bsp_event_handler(bsp_event_t event)
{
  ret_code_t err_code;

  switch (event)
  {
    case BSP_EVENT_SLEEP:
      powerEnterSleepMode();
      break;

    case BSP_EVENT_DISCONNECT:
      err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
      if (err_code != NRF_ERROR_INVALID_STATE) { APP_ERROR_CHECK(err_code); }
      break;

    case BSP_EVENT_KEY_0:
      break;

    case BSP_EVENT_KEY_2:
      break;

    case BSP_EVENT_KEY_3:
      break;

    default:
      break;
  }
}

static void buttons_leds_init(void)
{
  ret_code_t err_code;

  err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
  APP_ERROR_CHECK(err_code);

  // Configure power off
  bsp_event_to_button_action_assign(USER_BUTTON, BSP_BUTTON_ACTION_RELEASE, BSP_EVENT_SLEEP);
}

static void logInit(void)
{
  ret_code_t err_code = NRF_LOG_INIT(app_timer_cnt_get);
  APP_ERROR_CHECK(err_code);
}

static void powerInit(void)
{
  ret_code_t err_code;
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
  sd_power_dcdc_mode_set(true);
}

static void idle(void)
{
  if (NRF_LOG_PROCESS() == false && eventQueueEmpty()) {
    nrf_pwr_mgmt_run();
  }
}

static void cli_start(void)
{
  ret_code_t ret;

  ret = nrf_cli_start(&m_cli_libuarte);
  APP_ERROR_CHECK(ret);
}

static void cli_init(void)
{
  ret_code_t ret;

  cli_libuarte_config_t libuarte_config;
  libuarte_config.tx_pin = UART_TX_PIN;
  libuarte_config.rx_pin = UART_RX_PIN;
  libuarte_config.baudrate = NRF_UARTE_BAUDRATE_115200;
  libuarte_config.parity = NRF_UARTE_PARITY_EXCLUDED;
  libuarte_config.hwfc = NRF_UARTE_HWFC_DISABLED;
  ret = nrf_cli_init(&m_cli_libuarte, &libuarte_config, true, true, NRF_LOG_SEVERITY_INFO);

  APP_ERROR_CHECK(ret);
}

static void cli_process(void)
{
  nrf_cli_process(&m_cli_libuarte);
}

static void banjiInit(void)
{
  logInit();
  NRF_LOG_RAW_INFO("%08d [banji] booting...\n", systemTimeGetMs());

  timersInit();
  ret_code_t err_code;
  err_code = app_timer_create(&resetTimer, APP_TIMER_MODE_SINGLE_SHOT, resetTimerCallback);
  APP_ERROR_CHECK(err_code);

  cli_init();
  cli_start();

  NRF_LOG_RAW_INFO("Press the Tab key to see all available commands.\n");

  gpioInit();

  eventQueueInit();
  buttons_leds_init();

  i2c_init();
  spiInit();
  // accelInit();
  // accelGenericInterruptEnable(&accelInterrupt1);

  powerInit();

  bleInit();
  bleAdvertisingStart();

  NRF_LOG_RAW_INFO("%08d [banji] booted\n", systemTimeGetMs());
}

static void processQueue(void)
{
  if (!eventQueueEmpty()) {
    switch(eventQueueFront()) {
      case EVENT_ACCEL_MOTION:
        NRF_LOG_RAW_INFO("%08d [accel] motion\n", systemTimeGetMs());
        break;

      case EVENT_ACCEL_STATIC:
        break;

      case EVENT_BLE_DATA_STREAM_START:
        NRF_LOG_RAW_INFO("%08d [ble] stream start\n", systemTimeGetMs());

        NRF_TIMER3->TASKS_CAPTURE[3] = 1;
        uint32_t timer3 = NRF_TIMER3->CC[3];
        nrf_ppi_channel_enable(NRF_PPI_CHANNEL5);
        NRF_LOG_RAW_INFO("%08d [main] PPI ENABLE %u\n", systemTimeGetMs(), timer3);

        metadata[0] = (timer3 >> 24) & 0xFF;
        metadata[1] = (timer3 >> 16) & 0xFF;
        metadata[2] = (timer3 >> 8) & 0xFF;
        metadata[3] = timer3 & 0xFF;

        bleSendData(metadata, 180);

        bleMicStreamRequested = true;
        gpioWrite(GPIO_1_PIN, 1);
        break;

      case EVENT_BLE_DATA_STREAM_STOP:
      {
        NVIC_SystemReset();
        // app_timer_start(resetTimer, APP_TIMER_TICKS(2000), NULL);
        break;
      }

      case EVENT_BLE_RADIO_START:
        // Event that fires whenever the radio starts up
        break;

      case EVENT_BLE_SEND_DATA_DONE:
        // BLE just finished, attempt to fill in more data
        send();
        break;

      case EVENT_BLE_IDLE:
        powerEnterSleepMode();
        break;

      case EVENT_BLE_DISCONNECTED:
        NVIC_SystemReset();
        break;

      case EVENT_TIMERS_ONE_SECOND_ELAPSED:
        break;

      case EVENT_METADATA_SAVE_TIMESTAMP:
      {
        uint32_t timestamp = systemTimeGetMs();
        metadata[metadataIndex++ % 180]  = (timestamp >> 24) & 0xFF;
        metadata[metadataIndex++ % 180] = (timestamp >> 16) & 0xFF;
        metadata[metadataIndex++ % 180] = (timestamp >> 8) & 0xFF;
        metadata[metadataIndex++ % 180] = timestamp & 0xFF;
        break;
      }

      default:
        NRF_LOG_RAW_INFO("%08d [main] unhandled event:%d\n", systemTimeGetMs(), eventQueueFront());
        break;
    }

    eventQueuePop();
  }
}

int main(void)
{
  banjiInit();

  for (;;)
  {
    cli_process();
    idle();
    processQueue();
  }
}

static void cmd_i2c_scan(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  i2c_scan();
}

static void cmd_i2c(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  ASSERT(p_cli);
  ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

  if ((argc == 1) || nrf_cli_help_requested(p_cli))
  {
    nrf_cli_help_print(p_cli, NULL, 0);
    return;
  }

  if (argc != 2)
  {
    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\n", argv[0]);
    return;
  }

  nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\n", argv[0], argv[1]);
}

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_i2c)
{
    NRF_CLI_CMD(scan, NULL, "Print all entered parameters.", cmd_i2c_scan),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(i2c, &m_sub_i2c, "i2c", cmd_i2c);