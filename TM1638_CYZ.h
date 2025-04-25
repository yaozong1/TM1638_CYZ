/*
 * TM1638_from_FAE.h
 *
 *  Created on: 2025年4月25日
 *      Author: chen
 */

#ifndef TM1638_FROM_FAE_H_
#define TM1638_FROM_FAE_H_



#endif /* TM1638_FROM_FAE_H_ */

#include "SEGGER_RTT.h"
#include <stdbool.h>
#include <stdio.h>
#include "TM1638.h"
#include "TM1638_platform.h"
#include "string.h"

/**
 ==================================================================================
                               ##### Functions #####
 ==================================================================================
 */

/**
 * @brief  Initialize platform device to communicate TM1638.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
TM1638_Platform_Init(TM1638_Handler_t *Handler);



void display_one(uint8_t dat);
void display_Rice(uint16_t dat,uint8_t DigitPos);
void display_Rice_Multi(uint16_t dat[4], uint8_t digitMask);
void display_Rice_Multi_straight(uint8_t digiDATA_s[4], uint8_t digitMask);
