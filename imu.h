#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "gpio.h"

// BMI270 header config from https://community.bosch-sensortec.com/t5/MEMS-sensors-forum/BMI270-Config-File/td-p/17157

// scale factor: +/- 8G full range = 4000mG total range / 65536 counts (16 bit)


#ifndef IMU_H
#define IMU_H 

#define SCALE 1//0.244140625 
#define CMD 0x7E
#define accel_gyr_addr 0x0C

#define CHIPID 0x00
#define PWR_CONF 0x7C
#define PWR_CTRL 0x7D
#define ACC_CONF 0x40
#define GYR_CONF 0x42

#define INIT_CTRL 0x59
#define INIT_ADDR_0 0x5B
#define INIT_DATA 0x5E
#define INT_STATUS 0x21

#define CONFIG_SIZE 8192 // BMI270 Config File Size
    
#define IMU_READ 0x80
#define IMU_WRITE 0x00

#define IMU_INT1 0
#define IMU_INT2 1

#define IMU_INT_SOURCE_WAKE_UP            (1 << 0)
#define IMU_INT_SOURCE_GENERIC1           (1 << 2)
#define IMU_INT_SOURCE_GENERIC2           (1 << 3)

#define CHIP_ID_REG                          0x00
#define CHIP_ID                              0x24

#define ERROR_REG                            0x02
#define STATUS_REG                           0x03
  typedef union {
    struct {
      unsigned interruptActive: 1;
      unsigned powerMode: 2; // 0: sleep, 1: low power, 2: normal, 3: unused
      unsigned reserved2: 1;
      unsigned commandReady: 1;
      unsigned reserved: 2;
      unsigned dataReady: 1;
    };
    uint8_t bits;
  } imuStatus_t;

#define ACC_X_LSB_REG                         0x0C
#define ACC_X_MSB_REG                         0x0D
#define ACC_Y_LSB_REG                         0x0E
#define ACC_Y_MSB_REG                         0x0F
#define ACC_Z_LSB_REG                         0x10
#define ACC_Z_MSB_REG                         0x11
#define EVENT_REG                             0x1B
#define INT_STAT0_REG                         0x1C
#define INT_STAT1_REG                         0x1D

#define ACC_CONFIG0_REG                       0x40

#define ACC_CONFIG0_POWER_MODE_NORMAL         (2 << 0)

#define INT_CONFIG0_REG                       0x1F
#define INT_CONFIG0_GEN1_ENABLE               (1 << 2)

#define INT_CONFIG1_REG                       0x20
#define INT_CONFIG1_LATCHED                   (1 << 0)
#define INT_CONFIG1_UNLATCHED                 (0 << 0)


typedef struct {
  uint8_t pin;
  uint8_t source;
  bool xEnable;
  bool yEnable;
  bool zEnable;
  bool activity;
  bool combSelectIsAnd;
  uint8_t threshold;
  uint16_t duration;
} imuGenericInterrupt_t;

void imuInit(void);
uint8_t imuRead(uint8_t reg);
void imuWrite(uint8_t reg, uint8_t data);
static void uploadConfigFile(void);
static uint8_t checkInitStatus(void);
uint16_t imuGetX(void);
uint16_t imuGetY(void);
uint16_t imuGetZ(void);
void imuGenericInterruptEnable(imuGenericInterrupt_t*);
void imuScratchpad();
void imuDumpRegisters(uint8_t start, uint8_t end);


#endif
