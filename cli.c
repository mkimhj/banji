#include "i2c.h"
#include "gpio.h"
#include "camera.h"
#include "HM01B0_SPI.h"
#include "HM01B0_CLK.h"
#include "spi.h"
#include "imu.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_cli.h"
#include "nrf_cli_types.h"
#include "nrf_cli_libuarte.h"
#include "timers.h"
#include "spi.h"

#define CLI_EXAMPLE_LOG_QUEUE_SIZE (6)

NRF_CLI_LIBUARTE_DEF(m_cli_libuarte_transport, 512, 512);
NRF_CLI_DEF(m_cli_libuarte,
            "banji] ",
            &m_cli_libuarte_transport.transport,
            '\r',
            CLI_EXAMPLE_LOG_QUEUE_SIZE);


// I2C COMMANDS
static void cmd_i2c_scan(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  i2cScan();
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

static uint16_t pixel = 0;

static void cmd_cam_capture(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  cameraInit();
  cameraStartStream();
  pixel = 0;
}

static void cmd_cam_print(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  uint8_t* camData;
  uint16_t camDataLength = 0;

  camDataLength = spiSlaveGetRxBuffer(&camData);

  bool dataExists = false;
  uint16_t dataExistsAtIndex = 0;
  for (uint16_t i = 0; i < camDataLength; i++) {
    if (camData[i] != 0) {
      dataExists = true;
    }
  }

  if (dataExists) {
    for (int i = pixel; i < pixel + 100; i++) {
      NRF_LOG_RAW_INFO("%d ", camData[i]);
    }
  } else {
    NRF_LOG_RAW_INFO("No data");
  }

  pixel += 100;

  NRF_LOG_RAW_INFO("\n");
}

static void cmd_cam_clk(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  hm_clk_out();
}

// CAMERA COMMANDS
static void cmd_cam(nrf_cli_t const *p_cli, size_t argc, char **argv)
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

static void cmd_spis_receive(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  static bool initialized = false;
  if (initialized == false) {
    spiSlaveInit();
    initialized = true;
  }

  spiSlaveSetBuffers();
}

static void cmd_spis(nrf_cli_t const *p_cli, size_t argc, char **argv)
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


static void cmd_imu_readAccel(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  imuReadAccel();
}

/******************************************************************************/
/*!                IMU Commands                                               */
static void cmd_imu_readGyro(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  imuReadGyro();
}

static void cmd_imu_readReg(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  uint8_t regAddr = (uint8_t)strtol(argv[1], NULL, 16);
  uint8_t regVal;
  imuRead(regAddr, &regVal, 1, NULL);
  NRF_LOG_RAW_INFO("reg %x: %x\n", regAddr, regVal);
}

static void cmd_imu_setReg(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  uint8_t regAddr = (uint8_t)strtol(argv[1], NULL, 16);
  const uint8_t regData = (uint8_t)strtol(argv[2], NULL, 16);
  imuWrite(regAddr, &regData, 1, NULL); 
}

static void cmd_imu_readChipID(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  uint8_t regAddr = 0x00;
  uint8_t regVal;
  imuRead(regAddr, &regVal, 1, NULL);
  NRF_LOG_RAW_INFO("reg %x: %x\n", regAddr, regVal);
}

static void cmd_imu(nrf_cli_t const *p_cli, size_t argc, char **argv)
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

////////////////////////////////////////////////////////////////////////////////
void cliInit(void)
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
  ret = nrf_cli_start(&m_cli_libuarte);
  APP_ERROR_CHECK(ret);
}

void cliProcess(void)
{
  nrf_cli_process(&m_cli_libuarte);
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_spis){
    NRF_CLI_CMD(receive, NULL, "spiBus, data, length", cmd_spis_receive),
    NRF_CLI_SUBCMD_SET_END};
NRF_CLI_CMD_REGISTER(spis, &m_sub_spis, "spis", cmd_spis);

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_i2c){
    NRF_CLI_CMD(scan, NULL, "Print all entered parameters.", cmd_i2c_scan),
    NRF_CLI_SUBCMD_SET_END};
NRF_CLI_CMD_REGISTER(i2c, &m_sub_i2c, "i2c", cmd_i2c);

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_cam){
    NRF_CLI_CMD(clk, NULL, "Print all entered parameters.", cmd_cam_clk),
    NRF_CLI_CMD(capture, NULL, "Print all entered parameters.", cmd_cam_capture),
    NRF_CLI_CMD(print, NULL, "Print all entered parameters.", cmd_cam_print),
    NRF_CLI_SUBCMD_SET_END};
NRF_CLI_CMD_REGISTER(cam, &m_sub_cam, "camera", cmd_cam);

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_imu)
{
    NRF_CLI_CMD(read accel, NULL, "readAccel", cmd_imu_readAccel),
    NRF_CLI_CMD(read gyro, NULL, "readGyro", cmd_imu_readGyro),
    NRF_CLI_CMD(read reg, NULL, "spiBus, data, length", cmd_imu_readReg),
    NRF_CLI_CMD(set reg, NULL, "reg, val", cmd_imu_setReg),
    NRF_CLI_CMD(read chip ID, NULL, "reg, val", cmd_imu_readChipID),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(imu, &m_sub_imu, "imu", cmd_imu);

