//
// Created by Cy on 2022/12/27.
//
#include "vfd_display.h"

void write_6302(unsigned char w_data) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        CLK_L;
        if ((w_data & 0x01) == 0x01) {
            DIN_H;
        } else {
            DIN_L;
        }
        w_data >>= 1;
        delayUs(2);
        CLK_H;
        delayUs(2);
    }
}

void VFD_Set_Light(uint8_t value) {
    CS_L;
    write_6302(0xe4);
    delayUs(2);
    write_6302(value);  //0xff max//0x01 min  0~255
    CS_H;
    delayUs(2);
}

void VFD_init() {
    RST_L;
    HAL_Delay(300);
    RST_H;

    //SET HOW MANY digtal numbers
    //设置显示位数
    CS_L;
    write_6302(0xe0);
    delayUs(2);
    write_6302(0x07);//DIM 0~7
    CS_H;
    delayUs(2);

    ////set bright
    //  //设置亮度
    CS_L;
    write_6302(0xe4);
    delayUs(2);
    write_6302(VFD_LIGHT_HIGH);  //0xff max//0x01 min  0~255
    CS_H;
    delayUs(2);
}

void VFD_Show(void) {
    CS_L; //开始传输
    delayUs(2);
    write_6302(0xe8);//地址寄存器起始位置
    CS_H; //停止传输
}

void VFD_WriteOneChar(unsigned char x, unsigned char chr) {
    CS_L; //开始传输
    delayUs(2);
    write_6302(0x20 + x);//地址寄存器起始位置
    delayUs(2);
    write_6302(chr + 0x30);
    CS_H; //停止传输
}

void VFD_Clean() {
    CS_L; //开始传输
    delayUs(2);
    write_6302(0x20);//地址寄存器起始位置
    for (int i = 0; i < 8; i++) {
        write_6302(0x00); //ascii与对应字符表转换
    }
    CS_H; //停止传输
}

void VFD_Clean_Index(unsigned char x) {
    CS_L; //开始传输
    delayUs(2);
    write_6302(0x20 + x);//地址寄存器起始位置
    write_6302(0x00); //ascii与对应字符表转换
    CS_H; //停止传输
}

void VFD_WriteStr(unsigned char x, char *str) {
    CS_L; //开始传输
    delayUs(2);
    write_6302(0x20 + x);//地址寄存器起始位置
    while (*str) {
        write_6302(*str); //ascii与对应字符表转换
        str++;
    }
    CS_H; //停止传输
}