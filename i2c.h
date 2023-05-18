/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID 0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID 1
#endif

#define TWI_ADDRESSES 127

void i2c_scan(void);
void i2c_init(void);