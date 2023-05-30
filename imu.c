#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "gpio.h"
#include "spi.h"
#include "event.h"
#include "timers.h"
#include "imu.h"
#include <math.h>
#include "bmi270/bmi270.h"

/******************************************************************************/
/*!                Macro definition                                           */
/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH  (9.80665f)

/*! Macros to select the sensors                   */
#define ACCEL          UINT8_C(0x00)
#define GYRO           UINT8_C(0x01)

/******************************************************************************/
/* Status of api are returned to this variable. */
static int8_t rslt;

/* Variable to define limit to print accel data. */
static uint8_t limit = 10;

/* Assign accel sensor to variable. */
static uint8_t sensor_list[2] = {BMI2_ACCEL, BMI2_GYRO};

// Primary IMU device structure; interface interface is done here
// replaces bmi2_interface_init
static struct bmi2_dev bmi = {
     // Set SPI interface parameters
     .chip_id = BMI270_CHIP_ID, 
     .dummy_byte = 1,
     .read_write_len = 8, 
     .intf = BMI2_SPI_INTF,
     .read = imuRead,
     .write = imuWrite,
     .delay_us = imuDelay,
     .config_file_ptr = NULL
};

/* Structure to define type of sensor and their respective data. */
static struct bmi2_sens_data sensor_data = { { 0 } };

static uint8_t indx = 0;
static float accX = 0, accY = 0, accZ = 0;
static float gyrX = 0, gyrY = 0, gyrZ= 0;
static struct bmi2_sens_config config;

// bytes to be sent over SPI
static uint8_t imuSPIBuffer[20];

/******************************************************************************/
/*!           Static Function Declaration                                     */
/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (dps / (half_scale)) * (val);
}

/*!
 *  @brief Prints the execution status of the APIs.
 */
static void bmi2_print_error_code(int8_t rslt)
{
    switch (rslt)
    {
        case BMI2_OK:

            /* Do nothing */
            break;

        case BMI2_W_FIFO_EMPTY:
            NRF_LOG_RAW_INFO("[imu] error_code:%d FIFO empty\r\n", rslt);
            break;
        case BMI2_W_PARTIAL_READ:
            NRF_LOG_RAW_INFO("[imu] error_code:%d FIFO partial read\r\n", rslt);
            break;
        case BMI2_E_NULL_PTR:
           NRF_LOG_RAW_INFO("[imu] error_code:%d Null pointer error. It occurs when the user tries to assign value (not address) to a pointer, which has been initialized to NULL.\r\n", rslt);
            break;

        case BMI2_E_COM_FAIL:
           NRF_LOG_RAW_INFO("[imu] error_code:%d Communication failure error. It occurs due to read/write operation failure and also due to power failure during communication\r\n",rslt);
            break;

        case BMI2_E_DEV_NOT_FOUND:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Device not found error. It occurs when the device chip id is incorrectly read\r\n",
                   rslt);
            break;

        case BMI2_E_INVALID_SENSOR:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Invalid sensor error. It occurs when there is a mismatch in the requested feature with the available one\r\n", rslt);
            break;

        case BMI2_E_SELF_TEST_FAIL:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Self-test failed error. It occurs when the validation of accel self-test data is not satisfied\r\n",rslt);
            break;

        case BMI2_E_INVALID_INT_PIN:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Invalid interrupt pin error. It occurs when the user tries to configure interrupt pins apart from INT1 and INT2\r\n",rslt);
            break;

        case BMI2_E_OUT_OF_RANGE:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Out of range error. It occurs when the data exceeds from filtered or unfiltered data from fifo and also when the range exceeds the maximum range for accel and gyro while performing FOC\r\n", rslt);
            break;

        case BMI2_E_ACC_INVALID_CFG:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Invalid Accel configuration error. It occurs when there is an error in accel configuration register which could be one among range, BW or filter performance in reg address 0x40\r\n",rslt);
            break;

        case BMI2_E_GYRO_INVALID_CFG:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Error: Invalid Gyro configuration error. It occurs when there is a error in gyro configuration register which could be one among range, BW or filter performance in reg address 0x42\r\n", rslt);
            break;

        case BMI2_E_ACC_GYR_INVALID_CFG:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Invalid Accel-Gyro configuration error. It occurs when there is a error in accel and gyro configuration registers which could be one among range, BW or filter performance in reg address 0x40 and 0x42\r\n", rslt);
            break;

        case BMI2_E_CONFIG_LOAD:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Configuration load error. It occurs when failure observed while loading the configuration into the sensor\r\n", rslt);
            break;

        case BMI2_E_INVALID_PAGE:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Invalid page error. It occurs due to failure in writing the correct feature configuration from selected page\r\n", rslt);
            break;

        case BMI2_E_SET_APS_FAIL:
            NRF_LOG_RAW_INFO("[imu] error_code:%d APS failure error. It occurs due to failure in write of advance power mode configuration register\r\n", rslt);
            break;

        case BMI2_E_AUX_INVALID_CFG:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Invalid AUX configuration error. It occurs when the auxiliary interface settings are not enabled properly\r\n",rslt);
            break;

        case BMI2_E_AUX_BUSY:
            NRF_LOG_RAW_INFO("[imu] error_code:%d AUX busy error. It occurs when the auxiliary interface buses are engaged while configuring the AUX\r\n", rslt);
            break;

        case BMI2_E_REMAP_ERROR:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Remap error. It occurs due to failure in assigning the remap axes data for all the axes after change in axis position\r\n", rslt);
            break;

        case BMI2_E_GYR_USER_GAIN_UPD_FAIL:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Gyro user gain update fail error. It occurs when the reading of user gain update status fails\r\n",rslt);
            break;

        case BMI2_E_SELF_TEST_NOT_DONE:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Self-test not done error. It occurs when the self-test process is ongoing or not completed\r\n",rslt);
            break;

        case BMI2_E_INVALID_INPUT:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Invalid input error. It occurs when the sensor input validity fails\r\n", rslt);
            break;

        case BMI2_E_INVALID_STATUS:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Invalid status error. It occurs when the feature/sensor validity fails\r\n", rslt);
            break;

        case BMI2_E_CRT_ERROR:
            NRF_LOG_RAW_INFO("[imu] error_code:%d CRT error. It occurs when the CRT test has failed\r\n", rslt);
            break;

        case BMI2_E_ST_ALREADY_RUNNING:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Self-test already running error. It occurs when the self-test is already running and another has been initiated\r\n",rslt);
            break;

        case BMI2_E_CRT_READY_FOR_DL_FAIL_ABORT:
            NRF_LOG_RAW_INFO("[imu] error_code:%d CRT ready for download fail abort error. It occurs when download in CRT fails due to wrong address location\r\n", rslt);
            break;

        case BMI2_E_DL_ERROR:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Download error. It occurs when write length exceeds that of the maximum burst length\r\n", rslt);
            break;

        case BMI2_E_PRECON_ERROR:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Pre-conditional error. It occurs when precondition to start the feature was not completed\r\n",rslt);
            break;

        case BMI2_E_ABORT_ERROR:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Abort error. It occurs when the device was shaken during CRT test\r\n", rslt);
            break;

        case BMI2_E_WRITE_CYCLE_ONGOING:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Write cycle ongoing error. It occurs when the write cycle is already running and another has been initiated\r\n", rslt);
            break;

        case BMI2_E_ST_NOT_RUNING:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Self-test is not running error. It occurs when self-test running is disabled while it's running\r\n", rslt);
            break;

        case BMI2_E_DATA_RDY_INT_FAILED:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Data ready interrupt error. It occurs when the sample count exceeds the FOC sample limit and data ready status is not updated\r\n",rslt);
            break;

        case BMI2_E_INVALID_FOC_POSITION:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Error: Invalid FOC position error. It occurs when average FOC data is obtained for the wrong axes\r\n", rslt);
            break;

        default:
            NRF_LOG_RAW_INFO("[imu] error_code:%d Unknown error code\r\n", rslt);
            break;
    }
}

/*!
 * @brief This internal API is used to set configurations for accel and gyro.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer and gyro configuration. */
    struct bmi2_sens_config config[2];

    /* Configure the type of feature. */
    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(config, 2, bmi);
    bmi2_print_error_code(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi);
    bmi2_print_error_code(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config[ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_200HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config[GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config[GYRO].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel and gyro configurations. */
        rslt = bmi2_set_sensor_config(config, 2, bmi);
        bmi2_print_error_code(rslt);
    }

    return rslt;
}

/******************************************************************************/

int8_t imuRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr){
    imuSPIBuffer[0] = reg_addr;
    memcpy(imuSPIBuffer+1, reg_data, len);
    spiTransfer(SPI_BUS, imuSPIBuffer, len + 1);
    memcpy(reg_data,imuSPIBuffer+1,len);
    return BMI2_INTF_RET_SUCCESS;
}

int8_t imuWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr){

    imuSPIBuffer[0] = reg_addr;
    memcpy(imuSPIBuffer+1, reg_data, len);
    spiTransfer(SPI_BUS,imuSPIBuffer,len+1);

    return BMI2_INTF_RET_SUCCESS;
}

void imuDelay(uint32_t period, void *intf_ptr){
    //delayMs(period);
    nrf_delay_us(period);
}

void imuInit(void)
{
    // Initialize SPI interface
    //BMI270_sensor_init();
    rslt = bmi270_init(&bmi);
    bmi2_print_error_code(rslt);

    if(rslt == BMI2_OK)
    {
        // Accel configuration settings. 
        rslt = set_accel_gyro_config(&bmi);
        bmi2_print_error_code(rslt);

        // NOTE: Accel enable must be done after setting configurations
        rslt = bmi2_sensor_enable(sensor_list, 2, &bmi);
        bmi2_print_error_code(rslt);

        NRF_LOG_RAW_INFO("IMU initialization successful\n")
    }
    else{
        NRF_LOG_RAW_INFO("IMU initialization failed\n")
    }
}

void imuReadAccel(void){

    config.type = BMI2_ACCEL;
    // Get the accel configurations. 
    rslt = bmi2_get_sensor_config(&config, 1, &bmi);
    bmi2_print_error_code(rslt);

    rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
    bmi2_print_error_code(rslt);

    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC))
    {
        //Converting lsb to meter per second squared for 16 bit accelerometer at 2G range.
        accX = lsb_to_mps2(sensor_data.acc.x, (float)2, bmi.resolution);
        accY = lsb_to_mps2(sensor_data.acc.y, (float)2, bmi.resolution);
        accZ = lsb_to_mps2(sensor_data.acc.z, (float)2, bmi.resolution);
        NRF_LOG_RAW_INFO("Accel: x y z \n");
        NRF_LOG_RAW_INFO(NRF_LOG_FLOAT_MARKER "  ", NRF_LOG_FLOAT(accX));
        NRF_LOG_RAW_INFO(NRF_LOG_FLOAT_MARKER "  ", NRF_LOG_FLOAT(accY));
        NRF_LOG_RAW_INFO(NRF_LOG_FLOAT_MARKER"\n",NRF_LOG_FLOAT(accZ));
    }
}

void imuReadGyro(void){
    
    config.type = BMI2_GYRO;
    /* Get the accel configurations. */
    rslt = bmi2_get_sensor_config(&config, 1, &bmi);
    bmi2_print_error_code(rslt);

    rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
    bmi2_print_error_code(rslt);

    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR)){
        /* Converting lsb to degree/second for 16 bit gyro at 2000dps range. */
        gyrX = lsb_to_dps(sensor_data.gyr.x, (float)2000, bmi.resolution);
        gyrY = lsb_to_dps(sensor_data.gyr.y, (float)2000, bmi.resolution);
        gyrZ = lsb_to_dps(sensor_data.gyr.z, (float)2000, bmi.resolution);
        NRF_LOG_RAW_INFO("Gyro: x y z \n");
        NRF_LOG_RAW_INFO(NRF_LOG_FLOAT_MARKER "  ", NRF_LOG_FLOAT(gyrX));
        NRF_LOG_RAW_INFO(NRF_LOG_FLOAT_MARKER "  ", NRF_LOG_FLOAT(gyrY));
        NRF_LOG_RAW_INFO(NRF_LOG_FLOAT_MARKER"\n",NRF_LOG_FLOAT(gyrZ));
    }
}

int16_t readAccelX(void){
    rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
    bmi2_print_error_code(rslt);
    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC)){
        return sensor_data.acc.x;
    }else{
        return -1;
    }
}

int16_t readAccelY(void){
    rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
    bmi2_print_error_code(rslt);
    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC)){
        return sensor_data.acc.x;
    }else{
        return -1;
    }
}

int16_t readAccelZ(void){
    rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
    bmi2_print_error_code(rslt);
    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC)){
        return sensor_data.acc.z;
    }else{
        return -1;
    }
}

int16_t readGyroX(void){
    rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
    bmi2_print_error_code(rslt);
    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR)){
        return sensor_data.gyr.x;
    }else{
        return -1;
    }
}

int16_t readGyroY(void){
    rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
    bmi2_print_error_code(rslt);
    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR)){
        return sensor_data.gyr.y;
    }else{
        return -1;
    }
}

int16_t readGyroZ(void){
    rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
    bmi2_print_error_code(rslt);
    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR)){
        return sensor_data.gyr.z;
    }else{
        return -1;
    }
}