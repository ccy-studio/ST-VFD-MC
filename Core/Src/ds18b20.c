//
// Created by 83503 on 2022/12/27.
//

#include "ds18b20.h"
#include "tim.h"

//温度

GPIO_InitTypeDef GPIO_Init;

/*
 * 	DO（PA5）引脚设置成输入模式,内部上拉
 */
void GPIO_SET_IN(void) {
    GPIO_Init.Pin = GPIO_PIN_DO;
    GPIO_Init.Mode = GPIO_MODE_INPUT;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIO_DO, &GPIO_Init);
}

/*
 * 	DO（PA5）引脚设置成推挽输出模式，初始高电平
 */
void GPIO_SET_OUT(void) {
    GPIO_Init.Pin = GPIO_PIN_DO;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_WritePin(GPIO_DO, GPIO_PIN_DO, GPIO_PIN_SET);
    HAL_GPIO_Init(GPIO_DO, &GPIO_Init);
}

void DS18B20_Rest() {
    GPIO_SET_OUT();
    DSDO_H;
    delayUs(500);
    DSDO_L; //拉低
    delayUs(750);
    DSDO_H; //拉高 等待80us再去读
    delayUs(80);
    GPIO_SET_IN();
    if (DSDO_READ == GPIO_PIN_RESET) {
        //如果是低电平 读取成功
        //再次延时等待检测结束的时间
        delayUs(430);
    } else {
        //失败
        Error_Handler();
    }
}

/**
 * 写数据
 * @param dat
 */
void DS18B20_Write(uint8_t dat) {
    GPIO_SET_OUT();
    for (int i = 0; i < 8; i++) {
        DSDO_L;
        delayUs(4); //间隙4us
        if (dat & 0x01) {
            DSDO_H;
        } else {
            DSDO_L;
        }
        delayUs(60);
        dat >>= 1;
        DSDO_H;
    }
}

uint8_t DS18B20_Read() {
    uint8_t dat = 0;
    for (int i = 0; i < 8; i++) {
        GPIO_SET_OUT();
        DSDO_L;
        delayUs(4); //间隙4us
        DSDO_H;
        delayUs(6);
        GPIO_SET_IN();
        dat >>= 1;
        if (DSDO_READ == GPIO_PIN_SET) {
            dat |= 0x80;
        }
        delayUs(60);
        GPIO_SET_OUT();
        DSDO_H;
    }
    return dat;
}

/**
 * 返回获取温度值
 * @return
 */
float DS18B20_Get() {
    uint8_t rd;
    uint16_t data;
    DS18B20_Rest();
    DS18B20_Write(ROM_Skip);
    DS18B20_Write(Temperature_Conversion);
    DS18B20_Rest();
    DS18B20_Write(ROM_Skip);
    DS18B20_Write(Read_Scratch_Reg);
    delayUs(800); //温度转换时间
    rd = DS18B20_Read(); //低8位
    data = DS18B20_Read();//高8位
    data = (data << 8) | rd; //拼成16位
    //1111100000000000 16进制为0xf800  高位中前5为S代表正负数 1是负数0为正数
    if ((data & 0xf800) == 0xf800) {
        data = ~data + 1;
    }
    //默认是12位的分辨率
    return (float) (data * 0.0625);
}