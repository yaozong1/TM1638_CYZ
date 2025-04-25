/*
 **********************************************************************************
 * @file   TM1638_from_FAE.c
 * @author chen
 * @brief  TM1638 chip driver
 *         Functionalities of the this file:
 *          + Display config and control functions
 *          + Keypad scan functions
 **********************************************************************************
 ***********************************************************************************/
/* Includes ---------------------------------------------------------------------*/

#include <TM1638_CYZ.h>
#include "main.h"



const uint16_t HexTo14Seg[] = {
	  0x3F00, // 0 - abcdefg1全部点亮
	  0x0600, // 1 - bc点亮
	  0x5B01, // 2 - abg1g2de点亮
	  0x4F01, // 3 - abg1g2cd点亮
	  0x6601, // 4 - fg1g2bc点亮
	  0x6D01, // 5 - afg1g2cd点亮
	  0x7D01, // 6 - afg1g2cde点亮
	  0x0700, // 7 - abc点亮
	  0x7F01, // 8 - abcdefg1g2全部点亮
	  0x6F01  // 9 - abcfg1g2d点亮
  };

const uint16_t HexTo14Seg_Alpha[] = {
	    // 字母部分（严格修正）
	    0x7701, // A - abcefg1 + g2
	    0x7C01, // b - cdefg1g2 (小写b)
	    0x3900, // C - adeg1
	    0x5E01, // D - abdeg1 + g2
	    0x7901, // E - adeg1 + g2
	    0x7101, // F - aeg1 + g2
	    0x3D01, // G - adeg1 + g2
	    0x7601, // H - cdefg1g2
	    0x0600, // I - bc
	    0x1E00, // J - bcd
	    0x7018, // K - efg1 + kl (您的正确编码)
	    0x3800, // L - deg1
	    0x360A, // M - bced + hk
	    0x3612, // N - bcef + hl
	    0x3F00, // O - abcdefg1
		//-----------------
		0x7301 , // P:  （a+b+e+g1 + g2）(g1=D6,f=D5,e=D4,b=D1,a=D0)
        // 高字节: 01110011 (g1=D6,f=D5,e=D4,b=D1,a=D0)
        // 低字节: 00000001 (g2=D0)
		0x3F10,  // Q:

		0x7311, // R: a+b+e+f+g1 + g2+l (01110011 00010001)
		0x0D03, // S: a+c+d + h+g2 (00001101 00000011)
		0x0124, // T: a + j+m (00000001 00100100)
		0x3E00, // U: b+c+d+e+f (00111110) ← 关键修正！
		0x0612, // V: b+c + h+l (00100100 00010010)
		0x3650, // W: b+c+e+f + n+l (00110110 01010000)
		0x005A, // X: h+k+n+l (00000000 01011010)
		0x002A, // Y: h+k+m (00000000 00101010)
		0x0948  // Z: a+d + k+n (00001001 01001000)

};

// 微秒级延时
void delay_us(uint32_t us) {
    uint32_t ticks = us * (SystemCoreClock / 1000000) / 5;
    while(ticks--);
}


#define TM1638_CLK_GPIO     GPIOB
#define TM1638_CLK_PIN      GPIO_PIN_1
#define TM1638_DIO_GPIO     GPIOB
#define TM1638_DIO_PIN      GPIO_PIN_2
#define TM1638_STB_GPIO     GPIOB
#define TM1638_STB_PIN      GPIO_PIN_0
// 设置IO口状态

void TM1638_DIO_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TM1638_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TM1638_DIO_GPIO, &GPIO_InitStruct);
}
// 发送8bit数据
void send_8bit(uint8_t dat) {
    TM1638_DIO_Output();

    for(uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(TM1638_CLK_GPIO, TM1638_CLK_PIN, GPIO_PIN_RESET);
        delay_us(1);

        if(dat & 0x01) {
            HAL_GPIO_WritePin(TM1638_DIO_GPIO, TM1638_DIO_PIN, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(TM1638_DIO_GPIO, TM1638_DIO_PIN, GPIO_PIN_RESET);
        }

        delay_us(1);
        HAL_GPIO_WritePin(TM1638_CLK_GPIO, TM1638_CLK_PIN, GPIO_PIN_SET);
        delay_us(1);

        dat >>= 1;
    }

    HAL_GPIO_WritePin(TM1638_CLK_GPIO, TM1638_CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TM1638_DIO_GPIO, TM1638_DIO_PIN, GPIO_PIN_RESET);
}

// 发送控制命令
void send_command(uint8_t cmd) {
    HAL_GPIO_WritePin(TM1638_STB_GPIO, TM1638_STB_PIN, GPIO_PIN_SET);
    delay_us(1);
    HAL_GPIO_WritePin(TM1638_STB_GPIO, TM1638_STB_PIN, GPIO_PIN_RESET);
    delay_us(1);

    send_8bit(cmd);
}

// 第一位显示指定数字
void display_one(uint8_t dat) {
    send_command(0x40); // 数据命令:普通模式、地址自增1
    send_command(0xC0); // 显示地址:从00H开始

    send_8bit(0x02); // 第一位显示数值


    for(uint8_t i = 0; i < 15; i++) { // 后6位不显示
        send_8bit(0x00);
    }


    send_command(0x8A); // 显示控制:开显示,11/16亮度
    HAL_GPIO_WritePin(TM1638_STB_GPIO, TM1638_STB_PIN, GPIO_PIN_SET);
}


void display_Rice(uint16_t dat, uint8_t DigitPos) {
    // 确保DigitPos在0-3范围内
    DigitPos = DigitPos % 4;

    uint8_t upper = (dat >> 8) & 0xFF;  // 高8位 aaaaaaaa
    uint8_t lower = dat & 0xFF;        // 低8位 bbbbbbbb

    send_command(0x40); // 数据命令:普通模式、地址自增1
    send_command(0xC0); // 显示地址:从00H开始

    for(uint8_t i = 0; i < 8; i++) {
        // 检查低8位和高8位的第i位
        uint8_t bit_a = (upper >> i) & 0x01; // 高8位第i位
        uint8_t bit_b = (lower >> i) & 0x01; // 低8位第i位

        // 组合成0bxxxxbaxx格式，ba位置由DigitPos决定
        uint8_t output = (bit_b << (DigitPos*2+1)) | (bit_a << (DigitPos*2));

        send_8bit(output);
        send_8bit(0x00); // 保持原始格式
    }

    send_command(0x8C); // 显示控制
    HAL_GPIO_WritePin(TM1638_STB_GPIO, TM1638_STB_PIN, GPIO_PIN_SET);
}



void display_Rice_Multi(uint16_t dat[4], uint8_t digitMask) {
    // dat[4]: 4个独立的16位数据，每个控制一个DigitPos
    // digitMask: 位掩码，如0x01启用DigitPos 0，0x0F启用全部4个位组
	//0b1111每一位代表是否显示0~3位的数码管，转成十六进制就是Ox0F

    send_command(0x40); // 设置数据模式
    send_command(0xC0); // 起始地址00H

    for (uint8_t i = 0; i < 8; i++) {
        uint8_t output = 0x00;

        // 遍历4个位组
        for (uint8_t pos = 0; pos < 4; pos++) {
            if (digitMask & (1 << pos)) { // 检查该位组是否启用
                uint8_t upper = (dat[pos] >> 8) & 0xFF; // 高8位
                uint8_t lower = dat[pos] & 0xFF;        // 低8位
                uint8_t bit_a = (upper >> i) & 0x01;   // 高8位第i位
                uint8_t bit_b = (lower >> i) & 0x01;   // 低8位第i位

                // 将ba组合到对应位置（DigitPos*2）
                output |= (bit_b << (pos * 2 + 1)) | (bit_a << (pos * 2));
            }
        }

        send_8bit(output); // 发送组合后的数据
        send_8bit(0x00);   // 填充字节（保持原有协议）
    }

    send_command(0x8A);    // 开启显示

    HAL_GPIO_WritePin(TM1638_STB_GPIO, TM1638_STB_PIN, GPIO_PIN_SET);
}



void display_Rice_Multi_straight(uint8_t digiDATA_s[4], uint8_t digitMask) {
    uint16_t digitData[4] = {0};

    // 将自然数映射到14段编码
    for(uint8_t i = 0; i < 4; i++) {
        if(digiDATA_s[i] <= 9) { // 只处理0-9
            digitData[i] = HexTo14Seg[digiDATA_s[i]];
        }
        // 超出范围的保持0x0000(不显示)
    }

    // 调用原始函数
    display_Rice_Multi(digitData, digitMask);
}
