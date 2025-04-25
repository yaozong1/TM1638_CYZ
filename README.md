# TM1638 14段数码管显示驱动库

## 概述
本库提供了基于STM32 HAL库的TM1638 14段数码管显示驱动实现，支持数字和字母显示。

## 功能特性
- ✅ 支持14段LED显示数字0-9和字母A-Z  
- ✅ 单数字/多数字显示控制  
- ✅ 自定义显示位置  
- ✅ 优化的通信协议  
- ✅ 兼容STM32 HAL库  

## 硬件连接
| TM1638引脚 | STM32连接 | 说明       |
|------------|-----------|------------|
| CLK        | PB1       | 时钟线     |
| DIO        | PB2       | 数据线     |
| STB        | PB0       | 片选线     |
| VCC        | 5V        | 电源       |
| GND        | GND       | 地线       |

## 数码管&TM1638连接--JMF-4473BW8-059-P6.8
| TM1638引脚 | JMF-4473BW8连接 | 说明       |
|------------ |-----------|------------|
| SEG1        | 16        | 阳极1     |
| SEG2        | 1         | 阳极2     |
| ~           | ~         | ~         |
| SEG9        | 7         | 阳极7     |
| GRID1       | 14        | 阴极 1    |
| GRID2       | 13        | 阴极 2    |
| ~           | ~         | ~         |
| GRID7       | 5         | 阴极 7    |

-具体的连接请看规格数

## 快速开始
1. 添加文件到工程：
```c
#include "TM1638_CYZ.h"
```

2. 初始化后调用显示函数：
```c
// 显示单个数字
display_one(3); 

// 多数字显示
uint8_t nums[4] = {1,2,3,4};
display_Rice_Multi_straight(nums, 0x0F);
```

## API 说明

### 基本显示函数

#### `display_one(uint8_t dat)`
**功能**：在第一位显示单个数字  

**参数**：  
- `dat`: 要显示的数字 (0-9)  

**示例**：  
```c
display_one(5);  // 第一位显示数字5
```

---

#### `display_Rice(uint16_t dat, uint8_t DigitPos)`
**功能**：在指定位置显示14段编码数据  

**参数**：  
- `dat`: 14段编码数据（使用 `HexTo14Seg` 或 `HexTo14Seg_Alpha` 中的定义）  
- `DigitPos`: 显示位置 (0-3)  

**示例**：  
```c
display_Rice(HexTo14Seg_Alpha[0], 1);  // 在第二位显示字母A
```

---

### 高级显示函数

#### `display_Rice_Multi(uint16_t dat[4], uint8_t digitMask)`
**功能**：多位置显示控制  

**参数**：  
- `dat`: 包含4个14段编码的数组  
- `digitMask`: 位掩码控制哪些位置显示 (0x01-0x0F)  0x01 = 0b0001 显示第1位,0x0F=0b1111,4位都显示

**示例**：  
```c
uint16_t data[4] = {HexTo14Seg[1], HexTo14Seg[2], HexTo14Seg[3], HexTo14Seg[4]};
display_Rice_Multi(data, 0x0F);  // 显示1 2 3 4
```

---

#### `display_Rice_Multi_straight(uint8_t digiDATA_s[4], uint8_t digitMask)`
**功能**：直接显示数字的多位置控制  

**参数**：  
- `digiDATA_s`: 包含4个数字 (0-9) 的数组  
- `digitMask`: 位掩码控制哪些位置显示  

**示例**：  
```c
uint8_t nums[4] = {5, 6, 7, 8};
display_Rice_Multi_straight(nums, 0x0A);  // 在位置2和4显示6和8，0x0F=0b0000 1010
```

---


## 编码参考
### 数字编码示例：
```c
0x3F00, // 0 
0x0600, // 1
0x5B01  // 2
```

### 字母编码示例：
```c
0x7701, // A
0x7C01, // b
0x3900  // C
```
### 编码原理（共阳极）
-  编码的0xabcd, ab作为高八位控制控制每一位分别控制SEGx的GRID1~GRID8.举例，转成2进制0b00010001，代表打开GRID1和GRID5. cd控制另外一组SEGx
-  0xabcd分开为0xab和0xcd, 比如数码管的第一位SEG1（数码管外围）和SEG2(数码管内部) 
-  GRID1-8对应的Bit0-bit7分别写到00HL~0EHL的bitx，x就代表SEG（x+1），详细参看下面的表格
-  列举一个完整的编码方式 SEG1的0b0xxxxxxx每一位分别代表段码ABCDEFG1, SEG2的0b0xxxxxxx每一位分别代表G2HJKLMN(具体还是需要看连接方式)
-  编码只针对我的硬件连接上面提到的硬件连接方式
-  [点击查看 JMF-4473BW8-059-P6.8 规格书](https://cdn.sparkfun.com/assets/f/7/d/d/c/JMF-4473BW8-059-P6.8_-_.pdf)



### LED 显示数据操作顺序

### 位操作顺序（具体参考TM1638规格书）
| SEG1-SEG4      | SEG5-SEG8       | SEG9-SEG10      | 未使用          | 对应阴极  |
|----------------|-----------------|-----------------|-----------------|----------|
| xxHL（低四位）  | xxHU（高四位）   | xxHL（低四位）   | xxHU（高四位）   |          |
| B0 B1 B2 B3    | B4 B5 B6 B7     | B0 B1 B2 B3     | B4 B5 B6 B7     |         |
| **地址低字节**  | **地址高字节**   | **地址低字节**   | **地址高字节**   |          |
| 00HL           | 00HU            | 01HL            | 01HU            | GRID1    |
| 02HL           | 02HU            | 03HL            | 03HU            | GRID2    |
| 04HL           | 04HU            | 05HL            | 05HU            | GRID3    |
| 06HL           | 06HU            | 07HL            | 07HU            | GRID4    |
| 08HL           | 08HU            | 09HL            | 09HU            | GRID5    |
| 0AHL           | 0AHU            | 0BHL            | 0BHU            | GRID6    |
| 0CHL           | 0CHU            | 0DHL            | 0DHU            | GRID7    |
| 0EHL           | 0EHU            | 0FHL            | 0FHU            | GRID8    |

## 示例代码
### 显示字母组合
```c
uint16_t custom[4] = {
  HexTo14Seg_Alpha[0], // A
  HexTo14Seg[8],       // 8 
  HexTo14Seg_Alpha[2], // C
  HexTo14Seg[5]        // 5
};
display_Rice_Multi(custom, 0x0F);
```

## 注意事项
1. 使用前需初始化硬件
2. 亮度调节修改 `0x8A` 参数
3. 字母区分大小写（b≠B）

## 许可证
MIT License

## 贡献
欢迎提交Issue或PR
