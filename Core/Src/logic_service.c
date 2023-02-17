//
// Created by 83503 on 2022/12/28.
//
#include "logic_service.h"

///刷新频率 单位 ms
#define FQ_TIME 500 //时间页面的刷新频率
#define FQ_TIME_DIVISION 500 //时间页面的冒号闪烁频率
#define FQ_SET_TWINKLE 400 //Set设置时间与日期光标闪烁频率
#define FQ_TEMP 1000 //温度页面刷新频率
#define FQ_AUTO_PLAY 7000 //自动切换的延时时间
#define FQ_MODE 100 //MODE页面的刷新频率


///Set设置时间的选项Item
#define FLAG_SECOND 0
#define FLAG_MINUTE 1
#define FLAG_HOUR 2
#define FLAG_DAY 3
#define FLAG_MONTH 4
#define FLAG_YEAR 5

///屏幕显示的内容 Flag
#define DISPLAY_TIME 0
#define DISPLAY_DATE 1
#define DISPLAY_TEMP 2
#define DISPLAY_MODE 3
#define DISPLAY_SET 4

///Mode多功能设置自定义内容
#define MODE_LIGHT_SETTING 1 //亮度设置
#define MODE_AUTO_SWITCH 2 //自动轮播切换开关
//亮度设置二级选项卡的内容数组
char *MODE_LIGHT_SELECT_LIST[3] = {">> HIGH", ">>CENTRE", ">> LOW"};
//轮播开关设置二级选项卡的内容数组
char *MODE_AUTO_SELECT_LIST[2] = {">> OFF", ">> ON"};

#define MODE_LIGHT_SETTING_DESC "> LIGHT" //亮度文字
#define MODE_AUTO_SETTING_DESC "> AUTO" //轮播开关文字

//记录上一次点击Key按键的时间-软件消抖
uint32_t last_click_time;

///记录执行频率
uint32_t last_loop_dis_time;//上一次时间刷新
uint32_t last_loop_dis_time_division;//上一次时间分割符
uint32_t last_loop_dis_set_twinkle;//上一次set闪烁
uint32_t last_loop_dis_temp;//上一次set闪烁
uint32_t last_loop_auto_time;//上一次轮播时间
uint32_t last_loop_mode_time;//Mode页面执行时间频率

//当前操作Set功能时设置日期的模式选择的单位Flag标记
int current_set_item = FLAG_HOUR;
//记录上一个页面Page的内容标记Flag
int last_modify_display_flag;
//当前显示的Page内容
int current_display_flag = DISPLAY_TIME;

//设置页
int current_setting_show_card = MODE_LIGHT_SETTING;
//第二选项卡显示内容的数组下标，默认-1为未进入第二选项卡
int current_setting_select_idx = -1;

//闪烁状态标记位
int dis_time_division_state = 0; //时间冒号闪烁的状态
int dis_set_twinkle_state = 0; //Set闪烁的状态

DS_TimeDef currentTime; //当前的时间

char buf[9]; //字符格式缓存

///Session参数
int auto_enable = 0;//是否开启自动轮播
uint8_t light_level = VFD_LIGHT_LOW; //VFD亮度

/**
 * 初始化
 */
void SER_Init() {
    auto_enable = 0;
    light_level = VFD_LIGHT_LOW;

    VFD_Set_Light(light_level);
}

/**
 * 检查时间是否满足要求
 * @param last
 * @param delayTime
 * @return
 */
int Time_Check(uint32_t last, int delayTime) {
    return last == 0 || (HAL_GetTick() - last) > delayTime;
}

/**
 * 时间的显示
 */
void DIS_Datetime(void) {
    if (Time_Check(last_loop_dis_time, FQ_TIME)) {
        currentTime = DS1302_Read_Time();

        //DEBUG 打印输出日期时间
//        printf("%d-%d-%d %d:%d:%d\n", currentTime.year, currentTime.month, currentTime.day, currentTime.hour,
//               currentTime.minute, currentTime.second);

        switch (current_display_flag) {
            case DISPLAY_TIME:
                sprintf(buf, "%.2d", currentTime.hour);
                VFD_WriteStr(0, buf);

                sprintf(buf, "%.2d", currentTime.minute);
                VFD_WriteStr(3, buf);

                sprintf(buf, "%.2d", currentTime.second);
                VFD_WriteStr(6, buf);
                break;
            case DISPLAY_DATE:
                sprintf(buf, "%.2d/%.2d/%.2d", currentTime.year, currentTime.month, currentTime.day);
                VFD_WriteStr(0, buf);
                break;
            default:
                Error_Handler();
                break;
        }
        last_loop_dis_time = HAL_GetTick();
        VFD_Show();
    }

    if (current_display_flag == DISPLAY_TIME) {
        //当时时间Time显示的时候开启冒号闪烁
        if (Time_Check(last_loop_dis_time_division, FQ_TIME_DIVISION)) {
            if (dis_time_division_state) {
                VFD_Clean_Index(2);
                VFD_Clean_Index(5);
            } else {
                VFD_WriteOneChar(2, 10);
                VFD_WriteOneChar(5, 10);
            }
            dis_time_division_state = !dis_time_division_state;
            last_loop_dis_time_division = HAL_GetTick();
            VFD_Show();
        }
    }
}

/**
 * 温度的显示
 */
void DIS_Temp() {
    if (Time_Check(last_loop_dis_temp, FQ_TEMP)) {
        float tempValue = DS18B20_Get();
        sprintf(buf, "Temp%.1f", tempValue);
        VFD_Clean();
        VFD_WriteStr(0, buf);
        last_loop_dis_temp = HAL_GetTick();
        VFD_Show();
    }
}

/**
 * Set Page Content
 */
void DIS_Set() {
    if (Time_Check(last_loop_dis_set_twinkle, FQ_SET_TWINKLE)) {
        //显示出来当前的时间
        if (last_modify_display_flag == DISPLAY_TIME) {
            //时间
            char hour[3], minute[3], second[3];
            sprintf(hour, "%.2d", currentTime.hour);
            sprintf(minute, "%.2d", currentTime.minute);
            sprintf(second, "%.2d", currentTime.second);
            if (dis_set_twinkle_state) {
                switch (current_set_item) {
                    case FLAG_HOUR:
                        snprintf(hour, 3, "  ");
                        break;
                    case FLAG_MINUTE:
                        snprintf(minute, 3, "  ");
                        break;
                    case FLAG_SECOND:
                        snprintf(second, 3, "  ");
                        break;
                    default:
                        Error_Handler();
                }
            }
            sprintf(buf, "%s:%s:%s", hour, minute, second);
            VFD_WriteStr(0, buf);
        } else {
            //日期
            char year[3], month[3], day[3];
            sprintf(year, "%.2d", currentTime.year);
            sprintf(month, "%.2d", currentTime.month);
            sprintf(day, "%.2d", currentTime.day);
            if (dis_set_twinkle_state) {
                switch (current_set_item) {
                    case FLAG_YEAR:
                        snprintf(year, 3, "  ");
                        break;
                    case FLAG_MONTH:
                        snprintf(month, 3, "  ");
                        break;
                    case FLAG_DAY:
                        snprintf(day, 3, "  ");
                        break;
                    default:
                        Error_Handler();
                }
            }
            sprintf(buf, "%s/%s/%s", year, month, day);
            VFD_WriteStr(0, buf);
        }
        dis_set_twinkle_state = !dis_set_twinkle_state;
        last_loop_dis_set_twinkle = HAL_GetTick();
        VFD_Show();
    }
}

/**
 * Mode Page Content
 */
void DIS_Mode() {
    if (Time_Check(last_loop_mode_time, FQ_MODE)) {
        VFD_Clean();
        if (current_setting_select_idx != -1) {
            if (current_setting_show_card == MODE_LIGHT_SETTING) {
                VFD_WriteStr(0, MODE_LIGHT_SELECT_LIST[current_setting_select_idx]);
            } else if (current_setting_show_card == MODE_AUTO_SWITCH) {
                VFD_WriteStr(0, MODE_AUTO_SELECT_LIST[current_setting_select_idx]);
            }
        } else {
            if (current_setting_show_card == MODE_LIGHT_SETTING) {
                VFD_WriteStr(0, MODE_LIGHT_SETTING_DESC);
            } else if (current_setting_show_card == MODE_AUTO_SWITCH) {
                VFD_WriteStr(0, MODE_AUTO_SETTING_DESC);
            }
        }
        VFD_Show();
        last_loop_mode_time = HAL_GetTick();
    }
}

/**
 * 保存日期的修改
 */
void Save_Time() {
    if (last_modify_display_flag == DISPLAY_TIME) {
        DS1302_Set_Time(W_SECOND, currentTime.second);
        DS1302_Set_Time(W_MINUTE, currentTime.minute);
        DS1302_Set_Time(W_HOUR, currentTime.hour);
    } else {
        DS1302_Set_Time(W_DAY, currentTime.day);
        DS1302_Set_Time(W_MONTH, currentTime.month);
        DS1302_Set_Time(W_YEAR, currentTime.year);
    }
    //回归到原先显示的界面
    current_display_flag = last_modify_display_flag;
}

/**
 * 保持设置
 */
void Save_Mode() {
    if (current_setting_show_card == MODE_AUTO_SWITCH) {
        auto_enable = current_setting_select_idx;
    } else if (current_setting_show_card == MODE_LIGHT_SETTING) {
        if (current_setting_select_idx == 0) {
            light_level = VFD_LIGHT_HIGH;
        } else if (current_setting_select_idx == 1) {
            light_level = VFD_LIGHT_MIDDLE;
        } else if (current_setting_select_idx == 2) {
            light_level = VFD_LIGHT_LOW;
        }
        VFD_Set_Light(light_level);
    }
}

/**
 * 切换显示:    内容<->时间<->日期<->温度
 * @param action
 */
void Replay_DATE_Show_Type(int action) {
    //DISPLAY_TIME 0
    //DISPLAY_DATE 1
    //DISPLAY_TEMP 2
    if (action) {
        if (current_display_flag + 1 >= 3) {
            current_display_flag = 0;
        } else {
            current_display_flag++;
        }
    } else {
        if (current_display_flag - 1 < 0) {
            current_display_flag = 2;
        } else {
            current_display_flag--;
        }
    }
}

/**
 * Set Page 点击加减按钮的Event Handler
 * @param action
 */
void AS_Set_Click(int action) {
    int diff = action == 0 ? -1 : 1;
    switch (current_set_item) {
        case FLAG_SECOND:
            if (currentTime.second + (diff) < 0) {
                currentTime.second = 59;
            } else if (currentTime.second + (diff) >= 60) {
                currentTime.second = 0;
            } else {
                currentTime.second += (diff);
            }
            break;
        case FLAG_MINUTE:
            if (currentTime.minute + (diff) < 0) {
                currentTime.minute = 59;
            } else if (currentTime.minute + (diff) >= 60) {
                currentTime.minute = 0;
            } else {
                currentTime.minute += (diff);
            }
            break;
        case FLAG_HOUR:
            if (currentTime.hour + (diff) < 0) {
                currentTime.hour = 23;
            } else if (currentTime.hour + (diff) >= 24) {
                currentTime.hour = 0;
            } else {
                currentTime.hour += (diff);
            }
            break;
        case FLAG_DAY:
            if (currentTime.day + (diff) <= 0) {
                currentTime.day = 31;
            } else if (currentTime.day + (diff) >= 32) {
                currentTime.day = 1;
            } else {
                currentTime.day += (diff);
            }
            break;
        case FLAG_MONTH:
            if (currentTime.month + (diff) <= 0) {
                currentTime.month = 12;
            } else if (currentTime.month + (diff) >= 13) {
                currentTime.month = 1;
            } else {
                currentTime.month += (diff);
            }
            break;
        case FLAG_YEAR:
            if (currentTime.year + (diff) < 0) {
                currentTime.year = 99;
            } else if (currentTime.year + (diff) > 99) {
                currentTime.year = 0;
            } else {
                currentTime.year += (diff);
            }
            break;
        default:
            Error_Handler();
    }
}

/**
 * Mode Page 点击加减按钮的Event Handler
 * @param action
 */
void AS_Mode_Click(int action) {
    if (current_setting_select_idx == -1) {
        current_setting_show_card = current_setting_show_card % 2 + 1;
    } else {
        if (current_setting_show_card == MODE_LIGHT_SETTING) {
            current_setting_select_idx++;
            current_setting_select_idx %= 3;
        } else if (current_setting_show_card == MODE_AUTO_SWITCH) {
            current_setting_select_idx++;
            current_setting_select_idx %= 3;
        }
    }
}

/**
 * 按键 加和减的处理
 * @param action 0 减 1加
 */
void KEY_Group1(int action) {
    if (current_display_flag != DISPLAY_SET && current_display_flag != DISPLAY_MODE) {
        Replay_DATE_Show_Type(action);
    } else if (current_display_flag == DISPLAY_SET) {
        AS_Set_Click(action);
    } else if (current_display_flag == DISPLAY_MODE) {
        AS_Mode_Click(action);
    }
}

void KEY_SET() {
    if (current_display_flag == DISPLAY_SET) {
        //已经是SET了 移动修改光标
        if (last_modify_display_flag == DISPLAY_TIME) {
            if (current_set_item - 1 < FLAG_SECOND) {
                current_set_item = FLAG_HOUR;
            } else {
                current_set_item--;
            }
        } else {
            if (current_set_item - 1 < FLAG_DAY) {
                current_set_item = FLAG_YEAR;
            } else {
                current_set_item--;
            }
        }
    } else if (current_display_flag == DISPLAY_MODE) {
        //当前是MODE 确认修改mode的修改
        if (current_setting_select_idx == -1) {
            current_setting_select_idx = 0;
        } else {
            //保存设置
            Save_Mode();
            current_display_flag = last_modify_display_flag;
        }
    } else if (current_display_flag != DISPLAY_TEMP) {
        //其他情况 进入Set修改日期模式
        last_modify_display_flag = current_display_flag;
        if (last_modify_display_flag == DISPLAY_TIME) {
            current_set_item = FLAG_HOUR;
        } else {
            current_set_item = FLAG_YEAR;
        }
        current_display_flag = DISPLAY_SET;
    }
}

void KEY_MODE() {
    if (current_display_flag == DISPLAY_SET) {
        //当前是Set模式 代表保存修改的日期
        Save_Time();
    } else if (current_display_flag == DISPLAY_MODE) {
        current_display_flag = last_modify_display_flag;
    } else {
        last_modify_display_flag = current_display_flag;
        current_setting_select_idx = -1;
        current_setting_show_card = MODE_LIGHT_SETTING;
        current_display_flag = DISPLAY_MODE;
    }
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    //按键防抖500ms不可重复点击
    if (last_click_time != 0 && ((HAL_GetTick() - last_click_time) < 500)) {
        return;
    }
    if (GPIO_Pin == KEY_PLUS_Pin) {
        printf("KEY_PLUS_Pin\n");
        KEY_Group1(1);
    } else if (GPIO_Pin == KEY_REDUCE_Pin) {
        printf("KEY_REDUCE_Pin\n");
        KEY_Group1(0);
    } else if (GPIO_Pin == KEY_SET_Pin) {
        printf("KEY_SET_Pin\n");
        KEY_SET();
    } else if (GPIO_Pin == KEY_MODE_Pin) {
        printf("KEY_MODE_Pin\n");
        KEY_MODE();
    }
    last_click_time = HAL_GetTick();
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
}


/**
 * 放到Main函数的 while循环执行
 */
void SER_LOOP() {
    if (current_display_flag == DISPLAY_SET) {
        DIS_Set();
    } else if (current_display_flag == DISPLAY_MODE) {
        DIS_Mode();
    } else if (current_display_flag == DISPLAY_TEMP) {
        DIS_Temp();
    } else {
        //正常走时
        DIS_Datetime();
    }

    if (auto_enable) {
        //开启自动轮播
        if (Time_Check(last_loop_auto_time, FQ_AUTO_PLAY)) {
            if (current_display_flag != DISPLAY_SET && current_display_flag != DISPLAY_MODE) {
                Replay_DATE_Show_Type(1);
            }
            last_loop_auto_time = HAL_GetTick();
        }
    }
}