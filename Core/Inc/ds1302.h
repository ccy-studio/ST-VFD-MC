//
// Created by 83503 on 2022/12/27.
//

#ifndef VFD_FIRMWARE_DS1302_H
#define VFD_FIRMWARE_DS1302_H

#include "main.h"
#include "tim.h"

#define W_SECOND 0x80
#define W_MINUTE 0x82
#define W_HOUR   0x84
#define W_DAY    0x86
#define W_MONTH  0x88
#define W_WEEK   0x8A
#define W_YEAR   0x8C
#define W_CONTR  0x8E

#define R_SECOND 0x81
#define R_MINUTE 0x83
#define R_HOUR   0x85
#define R_DAY    0x87
#define R_MONTH  0x89
#define R_WEEK   0x8B
#define R_YEAR   0x8D
#define R_CONTR  0x8F

#define RST_L     HAL_GPIO_WritePin(GPIOA, DS_RST_Pin, GPIO_PIN_RESET)
#define RST_H     HAL_GPIO_WritePin(GPIOA, DS_RST_Pin, GPIO_PIN_SET)
#define CLK_L     HAL_GPIO_WritePin(GPIOA, DS_SCLK_Pin, GPIO_PIN_RESET)
#define CLK_H     HAL_GPIO_WritePin(GPIOA, DS_SCLK_Pin, GPIO_PIN_SET)
#define DATA_L    HAL_GPIO_WritePin(GPIOA, DS_IO_Pin, GPIO_PIN_RESET)
#define DATA_H    HAL_GPIO_WritePin(GPIOA, DS_IO_Pin, GPIO_PIN_SET)

#define DATA_READ HAL_GPIO_ReadPin(GPIOA, DS_IO_Pin)

typedef struct {
    uint8_t year; //年
    uint8_t week; //周
    uint8_t month;//月
    uint8_t day;//天
    uint8_t hour;//时
    uint8_t minute;//分
    uint8_t second;//秒
} DS_TimeDef;

/**
 * 读取时间
 * @return
 */
DS_TimeDef DS1302_Read_Time(void);

/**
 * 设置时间
 * @param addr 寄存器地址
 * @param data 写入值
 */
void DS1302_Set_Time(uint8_t addr, uint8_t data);

uint8_t decToBcd(uint8_t dec);

#endif //VFD_FIRMWARE_DS1302_H
