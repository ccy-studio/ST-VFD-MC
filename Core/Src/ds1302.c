//
// Created by 83503 on 2022/12/27.
//
#include "ds1302.h"
//实时时钟

GPIO_InitTypeDef DS_IoPinDef;

void DS_Set_Data_Read() {
    DS_IoPinDef.Pin = DS_IO_Pin;
    DS_IoPinDef.Mode = GPIO_MODE_INPUT;
    DS_IoPinDef.Pull = GPIO_PULLUP;
    DS_IoPinDef.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &DS_IoPinDef);
}

void DS_Set_Data_Write() {
    DS_IoPinDef.Pin = DS_IO_Pin;
    DS_IoPinDef.Mode = GPIO_MODE_OUTPUT_PP;
    DS_IoPinDef.Speed = GPIO_SPEED_FREQ_LOW;
    DS_IoPinDef.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &DS_IoPinDef);
}

/**
 * 二进制转十进制
 * @param bcd
 * @return
 */
uint8_t bcdToDec(const uint8_t bcd) {
    return (10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}


// Returns the binary-coded decimal of 'dec'. Inverse of bcdToDec.
/**
 * 十进制转二进制
 * @param dec
 * @return
 */
uint8_t decToBcd(const uint8_t dec) {
    uint8_t tens, ones;

    tens = dec / 10;
    ones = dec % 10;
    return (tens << 4) | ones;
}

// Returns the hour in 24-hour format from the hour register value.
uint8_t hourFromRegisterValue(const uint8_t value) {
    uint8_t adj;
    if (value & 128)  // 12-hour mode
        adj = 12 * ((value & 32) >> 5);
    else           // 24-hour mode
        adj = 10 * ((value & (32 + 16)) >> 4);
    return (value & 15) + adj;
}

void DS_Single_Write(uint8_t data) {
    //设置IO引脚为推挽输出模式
    DS_Set_Data_Write();
    for (int i = 0; i < 8; i++) {
        if (data & 0x01) {
            DATA_H;
        } else {
            DATA_L;
        }
        CLK_L; //时钟信号拉低
        delayUs(2);
        CLK_H; //时钟信号拉高 写入数据
        delayUs(2);
        data >>= 1;
    }
}

void DS_Write(uint8_t addr, uint8_t data) {
    //将rest拉低
    RST_L;
    CLK_L;
    delayUs(2);
    //开始发送数据
    RST_H; //拉高RST写入数据
    DS_Single_Write(addr);
    DS_Single_Write(data);
    RST_L;
    CLK_L;
}

uint8_t DS1302_Read_Single_Time(uint8_t regAddress) {
    RST_L;
    CLK_L;
    uint8_t regData = 0x00;
    RST_H;
    DS_Single_Write(regAddress);
    DS_Set_Data_Read();
    delayUs(2);
    for (int i = 0; i < 8; i++) {
        regData >>= 0x01;
        CLK_H;
        delayUs(5);
        CLK_L;
        delayUs(10);

        if (DATA_READ == GPIO_PIN_SET)
            regData = regData | 0x80;
    }
    RST_H;
    return bcdToDec(regData);
}

DS_TimeDef DS1302_Read_Time(void) {
    DS_TimeDef dsTimeDef;
    dsTimeDef.year = DS1302_Read_Single_Time(R_YEAR);
    dsTimeDef.week = DS1302_Read_Single_Time(R_WEEK);
    dsTimeDef.month = DS1302_Read_Single_Time(R_MONTH);
    dsTimeDef.day = DS1302_Read_Single_Time(R_DAY);
    dsTimeDef.hour = DS1302_Read_Single_Time(R_HOUR);
    dsTimeDef.minute = DS1302_Read_Single_Time(R_MINUTE);
    dsTimeDef.second = DS1302_Read_Single_Time(R_SECOND);
    return dsTimeDef;
}

void DS1302_Set_Time(uint8_t addr, uint8_t data) {
    DS_Write(W_CONTR, 0x00);
    DS_Write(addr, decToBcd(data));
    DS_Write(W_CONTR, 0x80);
}