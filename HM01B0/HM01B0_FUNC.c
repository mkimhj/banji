/*
 * HM01B0_I2c.c
 *
 *  Created on: Sept 15, 2018
 *      Author: Ali Najafi
 */

#include "i2c.h"
#include "HM01B0_FUNC.h"
#include "HM01B0_BLE_DEFINES.h"

void arducam_default(void){
  i2cWrite16(CAMERA_I2C_ADDR, 0x0103, 0x0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0100, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1003, 0x08);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1007, 0x08);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3044, 0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3045, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3047, 0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3050, 0xC0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3051, 0x42);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3052, 0x50);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3053, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3054, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3055, 0xF7);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3056, 0xF8);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3057, 0x29);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3058, 0x1F);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3059, 0x1E);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3064, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3065, 0x04);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1000, 0x43);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1001, 0x40);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1002, 0x32);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0350, 0x7F);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1006, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1008, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1009, 0xA0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100A, 0x60);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100B, 0x90);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100C, 0x40);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3022, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1012, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2000, 0x07);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2003, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2004, 0x1C);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2007, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2008, 0x58);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200B, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200C, 0x7A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200F, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2010, 0xB8);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2013, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2014, 0x58);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2017, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2018, 0x9B);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2100, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2101, 0x5F);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2102, 0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2103, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2104, 0x05);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2105, 0x02);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2106, 0x14);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2107, 0x02);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2108, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2109, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210A, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210B, 0x80);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210C, 0x40);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210D, 0x20);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210E, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210F, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2110, 0x85);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2111, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2112, 0xA0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2150, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0340, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0341, 0x7A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0342, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0343, 0x77);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3010, 0x01); // bit[0] 1 enable QVGA
  i2cWrite16(CAMERA_I2C_ADDR, 0x0383, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0387, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0390, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3011, 0x70);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3059, 0x22);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3060, 0x30);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0101, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0104, 0x01);
  //i2cWrite16(CAMERA_I2C_ADDR, 0x0390,0x03);  //1/4 binning
  //i2cWrite16(CAMERA_I2C_ADDR, 0x0383,0x03);
  //i2cWrite16(CAMERA_I2C_ADDR, 0x0387,0x03);
  //i2cWrite16(CAMERA_I2C_ADDR, 0x1012,0x03);

  i2cWrite16(CAMERA_I2C_ADDR, 0x0100, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0xFFFF, 0xFF);
}

void hm01b0_init_fixed_rom_qvga_fixed(void)
{
  i2cWrite16(CAMERA_I2C_ADDR, REG_MODE_SELECT, 0x00);               // go to stand by mode
  i2cWrite16(CAMERA_I2C_ADDR, REG_ANA_REGISTER_17, 0x00);           // register to change the clk source(osc:1 mclk:0), if no mclk it goes to osc by default
  i2cWrite16(CAMERA_I2C_ADDR, REG_TEST_PATTERN_MODE, TEST_PATTERN_OFF);

  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_MODE, 0x00);    // VERTICAL BIN MODE
  i2cWrite16(CAMERA_I2C_ADDR, REG_QVGA_WIN_EN, 0x01); // Set line length LSB to QQVGA => enabled: makes the image 160(row)*240(col)
  i2cWrite16(CAMERA_I2C_ADDR, REG_IO_DRIVE_STR, 0xFF);

  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_X, 0x01); // Horizontal Binning enable
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_Y, 0x01); // vertical Binning enable => this register should be always 0x03 because we never go more than 160 for the height

  // frame timing control
  // i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_LINES_H, 0x02); // changed by Ali - initial
  i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_LINES_H, 0x07); // changed by Ali
  i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_LINES_L, 0x12); // changed by Ali
  i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_PCK_H, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_PCK_L, 0x78); // changed by Ali

  i2cWrite16(CAMERA_I2C_ADDR, 0x3044, 0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3045, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3047, 0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3050, 0xC0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3051, 0x42);
  //    i2cWrite16(CAMERA_I2C_ADDR, 0x3052,0x50);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3053, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3054, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3055, 0xF7);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3056, 0xF8);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3057, 0x29);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3058, 0x1F);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3064, 0x00);

  // black level control
  i2cWrite16(CAMERA_I2C_ADDR, 0x1000, 0x43);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1001, 0x40);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1002, 0x32);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1003, 0x08); // default from lattice 0x08
  i2cWrite16(CAMERA_I2C_ADDR, 0x1006, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1007, 0x08); // default from lattice 0x08

  i2cWrite16(CAMERA_I2C_ADDR, 0x0350, 0x7F);

  // Sensor reserved
  i2cWrite16(CAMERA_I2C_ADDR, 0x1008, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1009, 0xA0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100A, 0x60);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100B, 0x90); // default from lattice 0x90
  i2cWrite16(CAMERA_I2C_ADDR, 0x100C, 0x40); // default from lattice 0x40

  // Vsync, hsync and pixel shift register
  //    i2cWrite16(CAMERA_I2C_ADDR, 0x1012,0x07);//changed by Ali
  i2cWrite16(CAMERA_I2C_ADDR, 0x1012, 0x00); // lattice value

  // Statistic control and read only
  i2cWrite16(CAMERA_I2C_ADDR, 0x2000, 0x07);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2003, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2004, 0x1C);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2007, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2008, 0x58);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200B, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200C, 0x7A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200F, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2010, 0xB8);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2013, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2014, 0x58);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2017, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2018, 0x9B);

  // Automatic exposure gain control - TODO: try changing these
  i2cWrite16(CAMERA_I2C_ADDR, 0x2100, 0x01); // Auto Exposure: 1-On 0-Off
  i2cWrite16(CAMERA_I2C_ADDR, 0x2101, 0x70); // 0x70);//lattice 0xA0
  i2cWrite16(CAMERA_I2C_ADDR, 0x2102, 0x01); // lattice 0x06
  i2cWrite16(CAMERA_I2C_ADDR, 0x2104, 0x07);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2105, 0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2106, 0xA4);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2108, 0x33);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210A, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210C,0x04);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210B, 0x80);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210F, 0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2110, 0xE9);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2111, 0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2112, 0x17);

  // Sensor exposure gain
  i2cWrite16(CAMERA_I2C_ADDR, 0x0202, 0x00); // Vikram
  i2cWrite16(CAMERA_I2C_ADDR, 0x0203, 0x01); // Vikram
  i2cWrite16(CAMERA_I2C_ADDR, 0x0205, 0x00); // Vikram - Analog Gain
  i2cWrite16(CAMERA_I2C_ADDR, 0x020E, 0x00); // Vikram - Digital Gain
  i2cWrite16(CAMERA_I2C_ADDR, 0x020F, 0x00); // Vikram - Digital Gain

  i2cWrite16(CAMERA_I2C_ADDR, REG_OSC_CLK_DIV, 0x30);               // This is effective when we use external clk, Use the camera in the gated clock mode to make the clock zero when there is no data
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIT_CONTROL, 0x22);               // Set the output to send 1 bit serial
  i2cWrite16(CAMERA_I2C_ADDR, REG_PMU_PROGRAMMABLE_FRAMECNT, 0x01); // set the number of frames to be sent out, it sends N frames
}


void hm01b0_init_fixed_rom_qvga_fixed_acc(void)
{
  i2cWrite16(CAMERA_I2C_ADDR, REG_MODE_SELECT, 0x00);//go to stand by mode
  i2cWrite16(CAMERA_I2C_ADDR, REG_ANA_REGISTER_17, 0x00);//register to change the clk source(osc:1 mclk:0), if no mclk it goes to osc by default
  i2cWrite16(CAMERA_I2C_ADDR, REG_TEST_PATTERN_MODE, TEST_PATTERN);//Enable the test pattern, set it to walking 1

  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_MODE, 0x00);//VERTICAL BIN MODE
  i2cWrite16(CAMERA_I2C_ADDR, REG_QVGA_WIN_EN, 0x01);//Set line length LSB to QQVGA => enabled: makes the image 160(row)*240(col)

  /*looking at lattice cfg setting*/
  //100*100 optimization
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_X, 0x01);//Horizontal Binning enable
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_Y, 0x01);//vertical Binning enable => this register should be always 0x03 because we never go more than 160 for the height
      //frame timing control
  i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_PCK_H,0x01);
  i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_PCK_L,0x78);//changed by Ali

  i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_LINES_H,0x02);//changed by Ali
  i2cWrite16(CAMERA_I2C_ADDR, REG_FRAME_LENGTH_LINES_L,0x12);//changed by Ali

  i2cWrite16(CAMERA_I2C_ADDR, REG_OSC_CLK_DIV, 0x30);//This is effective when we use external clk, Use the camera in the gated clock mode to make the clock zero when there is no data
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIT_CONTROL, 0x20);//Set the output to send 1 bit serial
  i2cWrite16(CAMERA_I2C_ADDR, REG_PMU_PROGRAMMABLE_FRAMECNT, 0x20);//set the number of frames to be sent out, it sends N frames
}


void hm01b0_init_fixed_rom_qvga_fixed_maxfps(void)
{
  i2cWrite16(CAMERA_I2C_ADDR, REG_MODE_SELECT, 0x00);//go to stand by mode
  i2cWrite16(CAMERA_I2C_ADDR, REG_ANA_REGISTER_17, 0x00);//register to change the clk source(osc:1 mclk:0), if no mclk it goes to osc by default
  i2cWrite16(CAMERA_I2C_ADDR, REG_TEST_PATTERN_MODE, 0x01);//Enable the test pattern, set it to walking 1

  i2cWrite16(CAMERA_I2C_ADDR, 0x1003,0x08);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1007,0x08);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3044,0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3045,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3047,0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3050,0xC0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3051,0x42);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3052,0x50);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3053,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3054,0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3055,0xF7);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3056,0xF8);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3057,0x29);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3058,0x1F);
//    i2cWrite16(CAMERA_I2C_ADDR, 0x3059,0x1E);//bit control
  i2cWrite16(CAMERA_I2C_ADDR, 0x3064,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3065,0x04);

  //black level control
  i2cWrite16(CAMERA_I2C_ADDR, 0x1000,0x43);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1001,0x40);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1002,0x32);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0350,0x7F);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1006,0x01);

  //Sensor reserved
  i2cWrite16(CAMERA_I2C_ADDR, 0x1008,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1009,0xA0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100A,0x60);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100B,0x90);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100C,0x40);

  //Vsync, hsync and pixel shift register
  i2cWrite16(CAMERA_I2C_ADDR, 0x1012,0x07);//changed by Ali
//    i2cWrite16(CAMERA_I2C_ADDR, 0x1012,0x00);

  //Statistic control and read only
  i2cWrite16(CAMERA_I2C_ADDR, 0x2000,0x07);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2003,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2004,0x1C);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2007,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2008,0x58);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200B,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200C,0x7A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200F,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2010,0xB8);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2013,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2014,0x58);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2017,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2018,0x9B);

  //Automatic exposure gain control
  i2cWrite16(CAMERA_I2C_ADDR, 0x2100,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2104,0x07);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2105,0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2106,0xA4);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2108,0x33);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210A,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210B,0x80);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210F,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2110,0xE9);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2111,0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2112,0x17);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2150,0x03);

  //frame timing control
  i2cWrite16(CAMERA_I2C_ADDR, 0x0340,0x02);//changed by Ali
  i2cWrite16(CAMERA_I2C_ADDR, 0x0341,0x32);//changed by Ali

  i2cWrite16(CAMERA_I2C_ADDR, 0x0342,0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0343,0x72);//changed by Ali

  i2cWrite16(CAMERA_I2C_ADDR, REG_QVGA_WIN_EN, 0x01);//Set line length LSB to QQVGA => enabled: makes the image 160(row)*240(col)

  i2cWrite16(CAMERA_I2C_ADDR, REG_OSC_CLK_DIV, 0x20);//This is effective when we use external clk, Use the camera in the gated clock mode to make the clock zero when there is no data

  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_X, 0x03);//Horizontal Binning enable
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_Y, 0x03);//vertical Binning enable
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_MODE, 0x03);//VERTICAL BIN MODE

  i2cWrite16(CAMERA_I2C_ADDR, REG_BIT_CONTROL, 0x20);//Set the output to send 1 bit serial

  i2cWrite16(CAMERA_I2C_ADDR, REG_PMU_PROGRAMMABLE_FRAMECNT, 0x01);//set the number of frames to be sent out, it sends N frames
}

void hm01b0_init_brighter(void)
{
  i2cWrite16(CAMERA_I2C_ADDR, REG_MODE_SELECT, 0x00);//go to stand by mode
  i2cWrite16(CAMERA_I2C_ADDR, REG_ANA_REGISTER_17, 0x00);//register to change the clk source(osc:1 mclk:0), if no mclk it goes to osc by default
  i2cWrite16(CAMERA_I2C_ADDR, REG_TEST_PATTERN_MODE, 0x00);//Enable the test pattern, set it to walking 1

  i2cWrite16(CAMERA_I2C_ADDR, 0x1003,0x08);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1007,0x08);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3044,0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3045,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3047,0x0A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3050,0xC0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3051,0x42);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3052,0x50);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3053,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3054,0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3055,0xF7);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3056,0xF8);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3057,0x29);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3058,0x1F);
//    i2cWrite16(CAMERA_I2C_ADDR, 0x3059,0x1E);//bit control
  i2cWrite16(CAMERA_I2C_ADDR, 0x3064,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x3065,0x04);

  //black level control
  i2cWrite16(CAMERA_I2C_ADDR, 0x1000,0x00);//changed by Ali
//    i2cWrite16(CAMERA_I2C_ADDR, 0x1000,0x43);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1001,0x40);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1002,0x32);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0350,0x7F);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1006,0x01);

  //Sensor reserved
  i2cWrite16(CAMERA_I2C_ADDR, 0x1008,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x1009,0xA0);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100A,0x60);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100B,0x90);
  i2cWrite16(CAMERA_I2C_ADDR, 0x100C,0x40);

  //Vsync, hsync and pixel shift register
  i2cWrite16(CAMERA_I2C_ADDR, 0x1012,0x07);//changed by Ali
//    i2cWrite16(CAMERA_I2C_ADDR, 0x1012,0x00);

  //Statistic control and read only
  i2cWrite16(CAMERA_I2C_ADDR, 0x2000,0x07);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2003,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2004,0x1C);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2007,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2008,0x58);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200B,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200C,0x7A);
  i2cWrite16(CAMERA_I2C_ADDR, 0x200F,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2010,0xB8);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2013,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2014,0x58);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2017,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2018,0x9B);

  //Automatic exposure gain control
  i2cWrite16(CAMERA_I2C_ADDR, 0x2100,0x00);//changed by Ali
//    i2cWrite16(CAMERA_I2C_ADDR, 0x2100,0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2104,0x07);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2105,0x03);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2106,0xA4);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2108,0x33);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210A,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210B,0x80);
  i2cWrite16(CAMERA_I2C_ADDR, 0x210F,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2110,0xE9);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2111,0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2112,0x17);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2150,0x03);

  //frame timing control
  i2cWrite16(CAMERA_I2C_ADDR, 0x0340,0x02);//changed by Ali
  i2cWrite16(CAMERA_I2C_ADDR, 0x0341,0x32);//changed by Ali

  i2cWrite16(CAMERA_I2C_ADDR, 0x0342,0x01);
  i2cWrite16(CAMERA_I2C_ADDR, 0x0343,0x72);//changed by Ali

  i2cWrite16(CAMERA_I2C_ADDR, 0x2101,0x00);
  i2cWrite16(CAMERA_I2C_ADDR, 0x2102,0x00);
//    i2cWrite16(CAMERA_I2C_ADDR, 0x2101,0xA0);//changed by Ali
//    i2cWrite16(CAMERA_I2C_ADDR, 0x2102,0x60);//changed by Ali
  i2cWrite16(CAMERA_I2C_ADDR, 0x020F,0x00);

  i2cWrite16(CAMERA_I2C_ADDR, 0x0205,0x30);
  i2cWrite16(CAMERA_I2C_ADDR, REG_QVGA_WIN_EN, 0x01);//Set line length LSB to QQVGA => enabled: makes the image 160(row)*240(col)
//    disable: image 160*320 //In test pattern mode, enabling this does not have any effect

  i2cWrite16(CAMERA_I2C_ADDR, REG_OSC_CLK_DIV, 0x20);//This is effective when we use external clk, Use the camera in the gated clock mode to make the clock zero when there is no data
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_X, 0x03);//Horizontal Binning enable
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_Y, 0x03);//vertical Binning enable
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_MODE, 0x03);//VERTICAL BIN MODE
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIT_CONTROL, 0x20);//Set the output to send 1 bit serial
  i2cWrite16(CAMERA_I2C_ADDR, REG_PMU_PROGRAMMABLE_FRAMECNT, 0x01);//set the number of frames to be sent out, it sends N frames
}

void hm01b0_init_datasheet_default(void)
{
  i2cWrite16(CAMERA_I2C_ADDR, REG_MODE_SELECT, 0x00);//go to stand by mode
  i2cWrite16(CAMERA_I2C_ADDR, REG_ANA_REGISTER_17, 0x00);//register to change the clk source(osc:1 mclk:0), if no mclk it goes to osc by default
  i2cWrite16(CAMERA_I2C_ADDR, REG_TEST_PATTERN_MODE, 0x00);//Enable the test pattern, set it to walking 1
  i2cWrite16(CAMERA_I2C_ADDR, REG_QVGA_WIN_EN, 0x00);//Set line length LSB to QQVGA => enabled: makes the image 160(row)*240(col)
  i2cWrite16(CAMERA_I2C_ADDR, REG_OSC_CLK_DIV, 0x20);//This is effective when we use external clk, Use the camera in the gated clock mode to make the clock zero when there is no data
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_X, 0x01);//Horizontal Binning enable
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_RDOUT_Y, 0x03);//vertical Binning enable
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIN_MODE, 0x01);//VERTICAL BIN MODE
  i2cWrite16(CAMERA_I2C_ADDR, REG_BIT_CONTROL, 0x20);//Set the output to send 1 bit serial
  i2cWrite16(CAMERA_I2C_ADDR, REG_PMU_PROGRAMMABLE_FRAMECNT, 0x01);//set the number of frames to be sent out, it sends N frames
}
