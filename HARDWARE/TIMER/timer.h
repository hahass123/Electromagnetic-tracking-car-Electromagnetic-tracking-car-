#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "led.h"
#include "usart.h"
#include "include.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//AD差值乘的系数
#define P_COEFFICIENT 70
#define D_COEFFICIENT 800
//电压高了，动力大，P反而要调小
/*Founctions-----------------------------------------------------------*/
//TIM3日常中断
void TIM3_Int_Init(u16 arr,u16 psc);
//TIM4的1234通道PWM输出
void TIM4_PWM_Init(u32 arr,u16 psc);


extern u8 count;

#endif




