#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

#include "main.h"
#include "event.h"
#include "ble_manager.h"
#include "timers.h"

#include "nrf_nvic.h"
#include "timers.h"
#include "HM01B0_BLE_DEFINES.h"
#include "gpio.h"
#include "imu.h"

// Custom services
#include "ble_cus.h"
BLE_CUS_DEF(m_cus);

NRF_BLE_GATT_DEF(m_gatt);           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising); /**< Advertising module instance. */

#define OPCODE_LENGTH 1
#define HANDLE_LENGTH 2

static uint32_t pixelsSent = 0; // equivalent to ble_bytes_sent_counter in prev codebase
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */
static uint8_t* transmitData;
static uint16_t transmitOffset;
static uint32_t transmitLength;
static uint16_t maxAttMtuBytes;
static volatile bool transmitDone;
static uint8_t bleCusPacket[256] = {0};
static ble_uuid_t m_adv_uuids[] =                                               /**< Universally unique service identifiers. */
{
  {CUSTOM_SERVICE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN}
};

#define RING_BUFFER_SIZE TOTAL_IMAGE_SIZE
static uint8_t ringBuffer[RING_BUFFER_SIZE] = {0};
static uint16_t ringBufferHead = 0;
static uint16_t ringBufferTail = 0;
static int ringBufferBytesUsed = 0;
static uint8_t sequenceNumber = 0;

// max buffer size is 10 seconds * sampling frequency * (accel_count+gyro_count) * bytes_per_sample
#define IMU_BUFFER_SIZE (10 * IMU_FREQUENCY_HZ * 6 * 2)
static uint8_t imuBuffer[IMU_BUFFER_SIZE] = {0};
static uint16_t imuBufferHead = 0;
static uint16_t imuBufferTail = 0;
static int imuBufferBytesUsed = 0;
static bool buttonWasPressed  = false;

char const * phy_str(ble_gap_phys_t phys)
{
  static char const * str[] =
  {
    "1 Mbps",
    "2 Mbps",
    "Coded",
    "Unknown"
  };

  switch (phys.tx_phys)
  {
    case BLE_GAP_PHY_1MBPS:
      return str[0];

    case BLE_GAP_PHY_2MBPS:
    case BLE_GAP_PHY_2MBPS | BLE_GAP_PHY_1MBPS:
    case BLE_GAP_PHY_2MBPS | BLE_GAP_PHY_1MBPS | BLE_GAP_PHY_CODED:
      return str[1];

    case BLE_GAP_PHY_CODED:
      return str[2];

    default:
      return str[3];
  }
}

static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
  switch (p_evt->evt_id)
  {
    case NRF_BLE_GATT_EVT_ATT_MTU_UPDATED:
    {
      maxAttMtuBytes = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
      NRF_LOG_INFO("ATT MTU exchange completed. MTU set to %u bytes.",
                    p_evt->params.att_mtu_effective);
      break;
    }

    case NRF_BLE_GATT_EVT_DATA_LENGTH_UPDATED:
    {
      NRF_LOG_INFO("Data length updated to %u bytes.", p_evt->params.data_length);

      uint8_t dataLength;
      nrf_ble_gatt_data_length_get(&m_gatt, m_conn_handle, &dataLength);
      NRF_LOG_INFO("nrf_ble_gatt_data_length_get: %u", dataLength);

      uint16_t effMtu = nrf_ble_gatt_eff_mtu_get(&m_gatt, m_conn_handle);
      NRF_LOG_INFO("nrf_ble_gatt_eff_mtu_get: %u", effMtu);
      break;
    }
  }

  ble_cus_on_gatt_evt(&m_cus, p_evt);
}

static void gatt_init(void)
{
  ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_ble_gatt_data_length_set(&m_gatt, BLE_CONN_HANDLE_INVALID, NRF_SDH_BLE_GAP_DATA_LENGTH);
  APP_ERROR_CHECK(err_code);
}

static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void on_cus_evt(ble_cus_t * p_cus_service, ble_cus_evt_t * p_evt)
{
  static uint32_t lastTransferTimeMs = 0;
  switch(p_evt->evt_type) {
    case BLE_CUS_EVT_NOTIFICATION_ENABLED:
      transmitDone = true; // reset value
      eventQueuePush(EVENT_BLE_DATA_STREAM_START);
      break;

    case BLE_CUS_EVT_NOTIFICATION_DISABLED:
      // End data stream signal
      eventQueuePush(EVENT_BLE_DATA_STREAM_STOP);
      break;

    case BLE_CUS_EVT_CONNECTED:
      NRF_LOG_RAW_INFO("%08d [ble] BLE_CUS_EVT_CONNECTED\n", systemTimeGetMs());
      break;

    case BLE_CUS_EVT_DISCONNECTED:
      NRF_LOG_RAW_INFO("%08d [ble] BLE_CUS_EVT_DISCONNECTED\n", systemTimeGetMs());
      break;

    case BLE_CUS_EVT_TRANSFER_1KB:
    {
      // if ((p_evt->bytes_transfered_cnt / 1024) == 0) {
        NRF_LOG_RAW_INFO("%08d [ble] sent %ukB\n", systemTimeGetMs(), (p_evt->bytes_transfered_cnt / 1024));
      // }
      break;
    }

    default:
      // No implementation needed.
      break;
  }
}

static void services_init(void)
{
  ret_code_t         err_code;
  nrf_ble_qwr_init_t qwr_init = {0};

  // Initialize Queued Write Module.
  qwr_init.error_handler = nrf_qwr_error_handler;

  err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
  APP_ERROR_CHECK(err_code);

  // Initialize mic stream service
  ble_cus_init_t cus_init;
  memset(&cus_init, 0, sizeof(cus_init));

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cus_init.custom_value_char_attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cus_init.custom_value_char_attr_md.write_perm);

  cus_init.evt_handler = on_cus_evt;
  err_code = ble_cus_init(&m_cus, &cus_init);
  APP_ERROR_CHECK(err_code);
}

static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
  switch(p_evt->evt_type)
  {
    case BLE_CONN_PARAMS_EVT_SUCCEEDED:
      break;

    case BLE_CONN_PARAMS_EVT_FAILED:
      //err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
      //APP_ERROR_CHECK(err_code);
      NRF_LOG_ERROR("BLE_CONN_PARAMS_EVT_FAILED. Keep the connection anyway..");
      break;

    default:
      NRF_LOG_INFO("Unhandled conn params event %d", p_evt->evt_type);
      break;
  }
}

static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void conn_params_init(void)
{
  ret_code_t             err_code;
  ble_conn_params_init_t cp_init;

  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params                  = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail             = false;
  cp_init.evt_handler                    = on_conn_params_evt;
  cp_init.error_handler                  = conn_params_error_handler;

  err_code = ble_conn_params_init(&cp_init);
  APP_ERROR_CHECK(err_code);
}

static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
  ret_code_t err_code;

  switch (ble_adv_evt)
  {
    case BLE_ADV_EVT_FAST:
      NRF_LOG_RAW_INFO("%08d [ble] advertising\n", systemTimeGetMs());
      err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
      APP_ERROR_CHECK(err_code);
      break;

    case BLE_ADV_EVT_IDLE:
      NRF_LOG_RAW_INFO("%08d [ble] idle\n", systemTimeGetMs());
      eventQueuePush(EVENT_BLE_IDLE);
      break;

    default:
      break;
  }
}

static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
  ret_code_t err_code = NRF_SUCCESS;

  switch (p_ble_evt->header.evt_id)
  {
    case BLE_GAP_EVT_DISCONNECTED:
      NRF_LOG_INFO("Disconnected.");
      eventQueuePush(EVENT_BLE_DISCONNECTED);
      // LED indication will be changed when advertising starts.
      break;

    case BLE_GAP_EVT_CONNECTED:
      NRF_LOG_INFO("Connected.");
      err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
      APP_ERROR_CHECK(err_code);
      m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
      err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
      APP_ERROR_CHECK(err_code);
      break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    {
      uint16_t max_con_int = p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.max_conn_interval;
      uint16_t min_con_int = p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.min_conn_interval;
      NRF_LOG_INFO("Connection interval updated: %d, %d", (5*min_con_int)/4, (5*max_con_int)/4);
      ble_cus_control_char_write(&m_cus, 0xFF);
      break;
    }

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
      ble_gap_phys_t const phys =
      {
          .rx_phys = BLE_GAP_PHY_AUTO,
          .tx_phys = BLE_GAP_PHY_AUTO,
      };
      err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
      APP_ERROR_CHECK(err_code);
      break;
    }

    case BLE_GAP_EVT_PHY_UPDATE:
    {
      ble_gap_evt_phy_update_t const * p_phy_evt = &p_ble_evt->evt.gap_evt.params.phy_update;

      if (p_phy_evt->status == BLE_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION)
      {
        // Ignore LL collisions.
        NRF_LOG_DEBUG("LL transaction collision during PHY update.");
        break;
      }

      ble_gap_phys_t phys = {0};
      phys.tx_phys = p_phy_evt->tx_phy;
      phys.rx_phys = p_phy_evt->rx_phy;
      NRF_LOG_INFO("PHY update %s. PHY set to %s.",
                    (p_phy_evt->status == BLE_HCI_STATUS_CODE_SUCCESS) ?
                    "accepted" : "rejected",
                    phy_str(phys));
      break;
    }

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
      NRF_LOG_INFO("Pairing not supported");
      err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
      APP_ERROR_CHECK(err_code);
      break;

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
      NRF_LOG_INFO("No system attributes have been stored.");
      err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
      APP_ERROR_CHECK(err_code);
      break;

    case BLE_GATTC_EVT_TIMEOUT:
      NRF_LOG_INFO("GATT Client Timeout.");
      err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
      APP_ERROR_CHECK(err_code);
      break;

    case BLE_GATTS_EVT_TIMEOUT:
      NRF_LOG_INFO("GATT Server Timeout.");
      err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
      APP_ERROR_CHECK(err_code);
      break;

    case BLE_GATTS_EVT_WRITE:
      NRF_LOG_INFO("BLE_GATTS_EVT_WRITE");
      break;

    case BLE_GATTS_EVT_HVN_TX_COMPLETE:
      transmitDone = true;
      // send(); // attempt to keep sending remaining bytes in ringBuffer
      eventQueuePush(EVENT_BLE_SEND_DATA_DONE); // attempt to requeue mic data if ringBuffer was previously full
      NRF_LOG_DEBUG("Handle value notification");
      break;

    case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
      NRF_LOG_INFO("BLE_GATTC_EVT_EXCHANGE_MTU_RSP");
      break;

    case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
      NRF_LOG_INFO("BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST");
      break;

    case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
      NRF_LOG_INFO("BLE_GAP_EVT_DATA_LENGTH_UPDATE");
      break;

    case BLE_GAP_EVT_ADV_SET_TERMINATED:
      NRF_LOG_INFO("BLE_GAP_EVT_ADV_SET_TERMINATED");
      break;

    case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
      NRF_LOG_INFO("BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST");
      break;

    default:
      NRF_LOG_INFO("BLE event not handled by app: %i", p_ble_evt->header.evt_id);
      break;
  }
}

static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

static void advertising_init(void)
{
  ret_code_t             err_code;
  ble_advertising_init_t init;

  memset(&init, 0, sizeof(init));

  init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
  init.advdata.include_appearance      = false;
  init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

  init.config.ble_adv_fast_enabled  = true;
  init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
  init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
  init.evt_handler = on_adv_evt;

  err_code = ble_advertising_init(&m_advertising, &init);
  APP_ERROR_CHECK(err_code);

  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void SWI1_IRQHandler(bool radio_evt)
{
  // if (radio_evt) { eventQueuePush(EVENT_BLE_RADIO_START); }
}

uint32_t radio_notification_init(uint32_t irq_priority, uint8_t notification_type, uint8_t notification_distance)
{
  uint32_t err_code;

  err_code = sd_nvic_ClearPendingIRQ(SWI1_IRQn);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  err_code = sd_nvic_SetPriority(SWI1_IRQn, irq_priority);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  err_code = sd_nvic_EnableIRQ(SWI1_IRQn);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  // Configure the event
  return sd_radio_notification_cfg_set(notification_type, notification_distance);
}

// PUBLIC
#define TX_POWER 4
//(accepted values are -40, -20, -16, -12, -8, -4, 0, and 4 dBm)
void bleAdvertisingStart()
{
  ret_code_t err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, TX_POWER);
  err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);
}

void bleInit(void)
{
  ble_stack_init();
  ret_code_t err_code = radio_notification_init(3, NRF_RADIO_NOTIFICATION_TYPE_INT_ON_ACTIVE, NRF_RADIO_NOTIFICATION_DISTANCE_800US);
  APP_ERROR_CHECK(err_code);
  gap_params_init();
  gatt_init();
  services_init();
  advertising_init();
  conn_params_init();
  transmitDone = true;

  uint8_t address[6] = {0};
  address[0] = 0xC0 | ((NRF_FICR->DEVICEADDR[1] & 0xFF00) >> 8);
  address[1] = (NRF_FICR->DEVICEADDR[1] & 0xFF);
  address[2] = (NRF_FICR->DEVICEADDR[0] & 0xFF000000) >> 24;
  address[3] = (NRF_FICR->DEVICEADDR[0] & 0xFF0000) >> 16;
  address[4] = (NRF_FICR->DEVICEADDR[0] & 0xFF00) >> 8;
  address[5] = (NRF_FICR->DEVICEADDR[0] & 0xFF);

  NRF_LOG_RAW_INFO("%08d [ble] address -> ", systemTimeGetMs());
  NRF_LOG_RAW_INFO("%02X:%02X:%02X:%02X:%02X:%02X\n", address[0], address[1], address[2], address[3], address[4], address[5]);
}

// Packet format
// 0: Sequence Number
// 1: Status (1:Button state, 0:Camera New Frame)
// 2-3: Accel X
// 4-5: Accel Y
// 6-7: Accel Z
// 8-9: Gyro X
// 10-11: Gyro Y
// 12-13: Gyro Z
// 14-xxx: Camera
static uint8_t cameraDataStartIndex = 14;
static bool startOfFrame = true;

void send(void)
{
  bool transmitSuccess = true;
  int length = maxAttMtuBytes;

  while(transmitSuccess && (ringBufferBytesUsed > 0)) {
    // if (ringBufferBytesUsed < (length - cameraDataStartIndex)) { break; }
    if (ringBufferBytesUsed < (length - cameraDataStartIndex))
    {
      length = ringBufferBytesUsed + cameraDataStartIndex;
    }

    bleCusPacket[0] = sequenceNumber;
    bleCusPacket[1] = 0x0;
    if (startOfFrame) {
      bleCusPacket[1] |= 0b1;
      startOfFrame = false;
    }

    bool pressed = (buttonPressed() || buttonWasPressed);

    if (pressed) {
      bleCusPacket[1] |= 0b10; // button state

      if (imuBufferBytesUsed >= 12) {
        bleCusPacket[1] |= 0b100; // mark imu data as valid
        for (int i = 2; i < 14; i++) {
          bleCusPacket[i] = imuBuffer[(imuBufferHead + (i - 2)) % IMU_BUFFER_SIZE];
        }
      }

      // int16_t accelX = readAccelX();
      // int16_t accelY = readAccelY();
      // int16_t accelZ = readAccelZ();
      // int16_t gyroX = readGyroX();
      // int16_t gyroY = readGyroY();
      // int16_t gyroZ = readGyroZ();

      // bleCusPacket[2] = accelX & 0xFF;
      // bleCusPacket[3] = (accelX >> 8) & 0xFF;
      // bleCusPacket[4] = accelY & 0xFF;
      // bleCusPacket[5] = (accelY >> 8) & 0xFF;
      // bleCusPacket[6] = accelZ & 0xFF;
      // bleCusPacket[7] = (accelZ >> 8) & 0xFF;

      // bleCusPacket[8] = gyroX & 0xFF;
      // bleCusPacket[9] = (gyroX >> 8) & 0xFF;
      // bleCusPacket[10] = gyroY & 0xFF;
      // bleCusPacket[11] = (gyroY >> 8) & 0xFF;
      // bleCusPacket[12] = gyroZ & 0xFF;
      // bleCusPacket[13] = (gyroZ >> 8) & 0xFF;
    }

    for (int i = cameraDataStartIndex; i < length; i++) {
      bleCusPacket[i] = ringBuffer[(ringBufferHead + (i - cameraDataStartIndex)) % RING_BUFFER_SIZE];
    }

    transmitSuccess = ble_cus_transmit(&m_cus, bleCusPacket, length);

    if (transmitSuccess) {
      ringBufferHead = (ringBufferHead + (length - cameraDataStartIndex)) % RING_BUFFER_SIZE;
      ringBufferBytesUsed -= (length - cameraDataStartIndex);
      sequenceNumber++;
      pixelsSent += length - cameraDataStartIndex;

      if (imuBufferBytesUsed >= 12) {
        imuBufferHead = (imuBufferHead + (12)) % IMU_BUFFER_SIZE;
        imuBufferBytesUsed -= 12;
      }

      if (buttonWasPressed) {
        buttonWasPressed = false;
      }
    }
  }
}

void bleSendData(uint8_t * data, uint16_t length)
{
  if (length > RING_BUFFER_SIZE) {
    NRF_LOG_RAW_INFO("[ble] input too large\n");
  }

  // Flag to reset image buffer on phone
  startOfFrame = true;

  for (uint16_t i = 0; i < length; i++) {
    ringBuffer[(ringBufferTail + i) % RING_BUFFER_SIZE] = data[i];
  }

  ringBufferTail = (ringBufferTail + length) % RING_BUFFER_SIZE;
  ringBufferBytesUsed += length;
  send();
}

void bleImuSendData(uint8_t * data, uint16_t length)
{
  if (length > IMU_BUFFER_SIZE) {
    NRF_LOG_RAW_INFO("[ble] input too large\n");
  }

  for (uint16_t i = 0; i < length; i++)
  {
    imuBuffer[(imuBufferTail + i) % IMU_BUFFER_SIZE] = data[i];
  }

  imuBufferTail = (imuBufferTail + length) % IMU_BUFFER_SIZE;
  imuBufferBytesUsed += length;
  send();
}

void bleSetButtonPressed(bool pressed)
{
  buttonWasPressed = pressed;
}

void bleImuResetBuffer(void)
{
  imuBufferHead = 0;
  imuBufferTail = 0;
  imuBufferBytesUsed = 0;
}

bool bleBufferHasSpace(uint16_t length)
{
  return ((ringBufferBytesUsed + length) < RING_BUFFER_SIZE);
}

uint32_t bleGetRingBufferBytesAvailable(void)
{
  return (RING_BUFFER_SIZE - ringBufferBytesUsed);
}

void blePushSequenceNumber(void)
{
  sequenceNumber += 3;
}

void bleSetPixelsSent(uint32_t value)
{
  pixelsSent = value;
}

uint32_t bleGetPixelsSent(void)
{
  return pixelsSent;
}

ret_code_t bleDisconnect(void)
{
  return sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}

void bleService(void) {
  uint32_t prevPixelsSent = pixelsSent;
  send();
  if (pixelsSent >= TOTAL_IMAGE_SIZE) {
    eventQueuePush(EVENT_CAMERA_READY_NEXT_FRAME);
    pixelsSent = 0;
  }
  // else if (pixelsSent != prevPixelsSent) {
  //   NRF_LOG_RAW_INFO("[ble] pixelsSent:%d\n", pixelsSent);
  // }
}