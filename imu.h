#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "gpio.h"
//#include "BMI270-Sensor-API-master/bmi270.h"

// BMI270 header config from https://community.bosch-sensortec.com/t5/MEMS-sensors-forum/BMI270-Config-File/td-p/17157

// scale factor: +/- 8G full range = 4000mG total range / 65536 counts (16 bit)


#ifndef IMU_H
#define IMU_H 

void imuInit(void);
int8_t imuRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr); // defined by Bosch API
int8_t imuWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr); // defined by Bosch API
int16_t readAccelX(void);
int16_t readAccelY(void);
int16_t readAccelZ(void);
int16_t readGyroX(void);
int16_t readGyroY(void);
int16_t readGyroZ(void);
void imuDelay(uint32_t period, void *intf_ptr);
void imuReadAccel(void);
void imuReadGyro(void);

#endif
