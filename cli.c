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

static void cmd_cam_capture(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  cameraInit();
  cameraCaptureFrame();
}

static void cmd_cam_print(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  uint8_t* camData;
  uint16_t camDataLength = 0;

  camDataLength = spiSlaveGetRxBuffer(&camData);
  NRF_LOG_RAW_INFO("camDataLength:%d\n", camDataLength);

  bool dataExists = false;
  uint16_t dataExistsAtIndex = 0;
  for (uint16_t i = 0; i < camDataLength; i++) {
    if (camData[i] != 0) {
      dataExists = true;
      dataExistsAtIndex = i;
    }
  }

  if (dataExists) {
    for (int i = 0; i < 100; i++) {
      NRF_LOG_RAW_INFO("%d:%d ", dataExistsAtIndex+i, camData[dataExistsAtIndex+i]);
    }
  } else {
    NRF_LOG_RAW_INFO("No data");
  }

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


/******************************************************************************/
/*!                SPI Commands                                               */
static void cmd_spi_readChipID(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  // read chip id = 0x24 at address 0x00
  int8_t id = 0x00;
  //uint8_t id = imuRead(0x00);

  // log the data returned
  NRF_LOG_RAW_INFO("%08d [spi IMU] chip_id = %x\n", systemTimeGetMs(), id);
}

// Read specific register
static void cmd_spi_readReg(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  uint8_t regAddr = (uint8_t)strtol(argv[1], NULL, 16);

  //uint8_t regVal = imuRead(regAddr);
  //NRF_LOG_RAW_INFO("reg %x: %x\n", regAddr, regVal);

}

// Set specific register
static void cmd_spi_setReg(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
  uint8_t regAddr = (uint8_t)strtol(argv[1], NULL, 16);
  uint8_t regVal = (uint8_t)strtol(argv[2], NULL, 16);

  NRF_LOG_RAW_INFO("reg %x: %x\n", regAddr, regVal);

  //imuWrite(regAddr, regVal);

}

static void cmd_spi(nrf_cli_t const *p_cli, size_t argc, char **argv)
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

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_spi)
{
    NRF_CLI_CMD(readReg, NULL, "spiBus, data, length", cmd_spi_readReg),
    NRF_CLI_CMD(setReg, NULL, "reg, val", cmd_spi_setReg),
    NRF_CLI_CMD(readChipID, NULL, "read IMU chip ID", cmd_spi_readChipID),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CMD_REGISTER(spi, &m_sub_spi, "spi", cmd_spi);


/******************************************************************************/
/*!                IMU Commands                                               */
static void cmd_imu_readAccel(nrf_cli_t const *p_cli, size_t argc, char **argv)
{

  imuReadAccel();

}

static void cmd_imu_readGyro(nrf_cli_t const *p_cli, size_t argc, char **argv)
{

  imuReadGyro();

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

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_imu)
{
    NRF_CLI_CMD(readAccel, NULL, "readAccel", cmd_imu_readAccel),
    NRF_CLI_CMD(readGyro, NULL, "readGyro", cmd_imu_readGyro),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CMD_REGISTER(imu, &m_sub_imu, "imu", cmd_imu);


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