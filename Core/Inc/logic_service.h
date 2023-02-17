//
// Created by 83503 on 2022/12/28.
//

#ifndef VFD_FIRMWARE_LOGIC_SERVICE_H
#define VFD_FIRMWARE_LOGIC_SERVICE_H

#include "main.h"
#include "ds1302.h"
#include "ds18b20.h"
#include "vfd_display.h"

void SER_Init();

void SER_LOOP(void);

#endif //VFD_FIRMWARE_LOGIC_SERVICE_H
