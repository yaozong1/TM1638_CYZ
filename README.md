TM1638 芯片驱动库简介
一、概述
本库是用于驱动 TM1638 芯片的 C 语言代码库，主要实现了 TM1638 芯片的显示配置与控制功能，以及按键扫描功能（代码中未展示按键扫描部分）。它提供了多种显示函数，可方便地在 TM1638 驱动的数码管上显示数字、字母等信息。
二、文件结构
TM1638_CYZ.c：包含了主要的驱动代码，如显示函数、数据发送函数、延时函数等。
TM1638_CYZ.h：头文件，声明了库中使用的函数和常量。
三、主要功能
1. 常量数组
HexTo14Seg：将 0 - 9 的数字映射到 14 段数码管的编码。
HexTo14Seg_Alpha：将字母 A - Z 映射到 14 段数码管的编码。
2. 延时函数
c
void delay_us(uint32_t us);
功能：实现微秒级延时。
参数：us 为延时的微秒数。
3. 显示函数
3.1 显示单个数字
c
void display_one(uint8_t dat);
功能：在数码管的第一位显示指定数字。
参数：dat 为要显示的数字（0 - 9）。
3.2 显示特定格式数据
c
void display_Rice(uint16_t dat, uint8_t DigitPos);
功能：在指定位置显示特定格式的数据。
参数：
dat：16 位数据。
DigitPos：显示位置（0 - 3）。
3.3 显示多个特定格式数据
c
void display_Rice_Multi(uint16_t dat[4], uint8_t digitMask);
功能：显示多个特定格式的数据，可通过位掩码控制显示哪些位置。
参数：
dat：包含 4 个 16 位数据的数组。
digitMask：位掩码，如 0x01 启用位置 0，0x0F 启用全部 4 个位置。
3.4 直接显示多个数字
c
void display_Rice_Multi_straight(uint8_t digiDATA_s[4], uint8_t digitMask);
功能：直接显示多个数字（0 - 9），会自动将数字映射到 14 段编码。
参数：
digiDATA_s：包含 4 个数字（0 - 9）的数组。
digitMask：位掩码，控制显示哪些位置。
4. 数据发送函数
4.1 发送 8 位数据
c
void send_8bit(uint8_t dat);
功能：向 TM1638 芯片发送 8 位数据。
参数：dat 为要发送的 8 位数据。
4.2 发送控制命令
c
void send_command(uint8_t cmd);
功能：向 TM1638 芯片发送控制命令。
参数：cmd 为要发送的控制命令。
四、使用示例
以下是一个简单的使用示例，显示数字 5 在第一位：
c
#include "TM1638_CYZ.h"

int main() {
    // 初始化相关硬件（代码中未展示）
    display_one(5);
    while(1) {
        // 主循环
    }
    return 0;
}


五、注意事项
本库使用了 HAL 库进行 GPIO 操作，确保在使用前正确配置 HAL 库。
延时函数 delay_us 依赖于 SystemCoreClock，确保该变量正确设置。
显示函数中的控制命令和显示亮度等参数可根据需要进行调整。
