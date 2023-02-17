//
// Created by Cy on 2022/12/27.
//

#ifndef VFD_FIRMWARE_VFD_DISPLAY_H
#define VFD_FIRMWARE_VFD_DISPLAY_H

#include "main.h"
#include "tim.h"

#define RST_L HAL_GPIO_WritePin(VFD_RST_GPIO_Port,VFD_RST_Pin,GPIO_PIN_RESET)
#define RST_H HAL_GPIO_WritePin(VFD_RST_GPIO_Port,VFD_RST_Pin,GPIO_PIN_SET)
#define CS_L HAL_GPIO_WritePin(VFD_CS_GPIO_Port,VFD_CS_Pin,GPIO_PIN_RESET)
#define CS_H HAL_GPIO_WritePin(VFD_CS_GPIO_Port,VFD_CS_Pin,GPIO_PIN_SET)
#define CLK_L HAL_GPIO_WritePin(VFD_CLK_GPIO_Port,VFD_CLK_Pin,GPIO_PIN_RESET)
#define CLK_H HAL_GPIO_WritePin(VFD_CLK_GPIO_Port,VFD_CLK_Pin,GPIO_PIN_SET)
#define DIN_L HAL_GPIO_WritePin(VFD_DIN_GPIO_Port,VFD_DIN_Pin,GPIO_PIN_RESET)
#define DIN_H HAL_GPIO_WritePin(VFD_DIN_GPIO_Port,VFD_DIN_Pin,GPIO_PIN_SET)

void write_6302(unsigned char w_data);
void VFD_init();
/**
 * 设置亮度
 * @param value
 */
void VFD_Set_Light(uint8_t value);
void VFD_Show(void);
void VFD_WriteOneChar(unsigned char x, unsigned char chr);
void VFD_WriteStr(unsigned char x, char *str);
void VFD_Clean();
void VFD_Clean_Index(unsigned char x);

///亮度
#define VFD_LIGHT_LOW 0xa //熄灭
#define VFD_LIGHT_HIGH 0xff //最高亮度
#define VFD_LIGHT_MIDDLE 0x7f //中间亮度

#endif //VFD_FIRMWARE_VFD_DISPLAY_H
