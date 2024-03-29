// main.h
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <nrfx.h>

// BLE
#define DEVICE_NAME                     "banji"                                 /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "uw"                                    /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                300                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */

#define APP_ADV_DURATION                18000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */
#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(15, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */
#define IOS_MAX_ATT_MTU_SIZE_BYTES      (182)

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

// IMU
#define IMU_FREQUENCY_HZ 50
#define IMU_INTERVAL_MS (1000/IMU_FREQUENCY_HZ)
#define IMU_TICKS APP_TIMER_TICKS(IMU_INTERVAL_MS)

// BUTTON
#define BUTTON_DEBOUNCE_MS 500

// Flash Configuration
#define FLASH_READ_BUFFER_SIZE 512

// Audio
// Each BLE packet is 182 bytes for iOS. 2 bytes are for the sequence number.
// 180 bytes remaining for audio data. A dropped pdm buffer before getting to the BLE layer
// needs to correspond to 3 "missed" packets so the phone can correctly fill in 3 packets
// and keep the mic streams aligned.
// (sizeof(uint16_t) * PDM_DECIMATION_BUFFER_LENGTH) / 3 = 180
#define PDM_BUFFER_LENGTH               (540)
#define PDM_DECIMATION_FACTOR           (2)
#define PDM_DECIMATION_BUFFER_LENGTH    (PDM_BUFFER_LENGTH / PDM_DECIMATION_FACTOR)

// Buttons and LED
#define USER_BUTTON 0                                                           // 0 is the first button in nordic code

// Stack
#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
void sleep_mode_enter(void);