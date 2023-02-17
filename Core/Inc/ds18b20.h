//
// Created by 83503 on 2022/12/27.
//

#ifndef VFD_FIRMWARE_DS18B20_H
#define VFD_FIRMWARE_DS18B20_H

#include "main.h"

#define GPIO_PIN_DO DS_TEMP_Pin        //设置DO引脚
#define GPIO_DO     DS_TEMP_GPIO_Port

#define DSDO_H        HAL_GPIO_WritePin(GPIO_DO, GPIO_PIN_DO, GPIO_PIN_SET)        //DO引脚置1
#define DSDO_L    HAL_GPIO_WritePin(GPIO_DO, GPIO_PIN_DO, GPIO_PIN_RESET)        //DO引脚清零
#define DSDO_READ        HAL_GPIO_ReadPin(GPIO_DO, GPIO_PIN_DO)                        //读取DO引脚

/*
 * 	ROM命令
 */
#define ROM_Skip        0xCC    //ROM跳过

/*
 * 	DS18B20功能命令
 */
#define Temperature_Conversion    0x44    //温度转换
#define Read_Scratch_Reg        0xBE    //读取暂存寄存器

float DS18B20_Get();

#endif //VFD_FIRMWARE_DS18B20_H
