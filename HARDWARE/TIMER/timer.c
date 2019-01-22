#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTE精英STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,增加TIM3_PWM_Init函数。
//2,增加LED0_PWM_VAL宏定义，控制TIM3_CH2脉宽									  
//////////////////////////////////////////////////////////////////////////////////  

u8 count = 0;
s16 front_lift = 0, frout_right = 0, behind_lift = 0, behind_right = 0;//计数变量；四个点击占空比变量
float sub_val = 0;//差比和变量
float sub_val_last = 0;//上一次感应值
float sub_D;//两次感应值差值，感应值变化量，用来表示感应值变化率
int i;

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数0x00~
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
        count++;
        //AD采集单次
        adcx1 = Get_Adc1(ADC_Channel_1);
        adcx2 = Get_Adc1(ADC_Channel_2);
        temp1 = (float)adcx1 * (3.3 / 4096);
        temp2 = (float)adcx2 * (3.3 / 4096);
        
        sub_val_last = sub_val;
        //左右电感差比和
        sub_val = (float)(adcx1 - adcx2) / (float)(adcx1 + adcx2);
        //感应值变化率
        sub_D = sub_val - sub_val_last;
        
        //速度快点减占空比，因为占空比是反着的，0%为满占空比
        front_lift = SETTING_SPEED - sub_val * P_COEFFICIENT - sub_D * D_COEFFICIENT;
        frout_right = SETTING_SPEED + sub_val * P_COEFFICIENT + sub_D * D_COEFFICIENT;
        behind_lift = SETTING_SPEED - sub_val * P_COEFFICIENT - sub_D * D_COEFFICIENT;
        behind_right = SETTING_SPEED + sub_val * P_COEFFICIENT + sub_D * D_COEFFICIENT;

        if(front_lift > 99)
        {
            frout_right = 100 + (sub_val * P_COEFFICIENT + sub_D * D_COEFFICIENT) * 2;
        }
        if(frout_right > 99)
        {
            front_lift = 100 + (-sub_val * P_COEFFICIENT - sub_D * D_COEFFICIENT) * 2;
        }
        if(behind_lift > 99)
        {
            behind_right = 100 + (sub_val * P_COEFFICIENT + sub_D * D_COEFFICIENT) * 2;
        }
        if(behind_right > 99)
        {
            behind_lift = 100 + (-sub_val * P_COEFFICIENT - sub_D * D_COEFFICIENT) * 2;
        }        
        
        //中间停车，电动势制动
        if(mark == 1)
        {
            PBout(0) = 0;
            PBout(1) = 0;  
            PBout(10) = 0;
            PBout(11) = 0;
            PBout(12) = 0;
            PBout(13) = 0;
            PBout(14) = 0;
            PBout(15) = 0;
        }
        else
        {
            //电机反正转，慢速电机占空比距离满占空比的电机快速一边是两倍赋值
            if(front_lift < 0)
            {
                front_lift = -(100 + (-sub_val * P_COEFFICIENT - sub_D * D_COEFFICIENT) * 2);
                PBout(0) = 0;
                PBout(1) = 1;    
            }
            else
            {
                PBout(0) = 1;
                PBout(1) = 0;
            }
            if(frout_right < 0)
            {
                frout_right = -(100 + (sub_val * P_COEFFICIENT + sub_D * D_COEFFICIENT) * 2);
                PBout(10) = 0;
                PBout(11) = 1;
            }
            else
            {
                PBout(10) = 1;
                PBout(11) = 0;
            }
            if(behind_lift < 0)
            {
                behind_lift = -(100 + (-sub_val * P_COEFFICIENT - sub_D * D_COEFFICIENT) * 2);
                PBout(12) = 1;
                PBout(13) = 0;
            }
            else
            {
                PBout(12) = 0;
                PBout(13) = 1;
            }
            if(behind_right < 0)
            {
                behind_right = -(100 + (sub_val * P_COEFFICIENT + sub_D * D_COEFFICIENT) * 2);
                PBout(14) = 1;
                PBout(15) = 0;
            }
            else
            {
                PBout(14) = 0;
                PBout(15) = 1;    
            }
        }
        
        //占空比限幅
        if(front_lift < 1)
        {
            front_lift = 0;
        }
        if(front_lift > 99)
        {
            front_lift = 100;
        }        
        if(frout_right < 1)
        {
            frout_right = 0;
        }
        if(frout_right > 99)
        {
            frout_right = 100;
        }
        if(behind_lift < 1)
        {
            behind_lift = 0;
        }
        if(behind_lift > 99)
        {
            behind_lift = 100;
        }
        if(behind_right < 1)
        {
            behind_right = 0;
        }
        if(behind_right > 99)
        {
            behind_right = 100;
        }        
        
        //小车出跑道停车
        if((adcx1 < 200)&&(adcx2 < 200))
        {
            front_lift = frout_right = behind_lift = behind_right = 0;
        }
        
        //更新电机占空比
        TIM_SetCompare1(TIM4, front_lift);
        TIM_SetCompare2(TIM4, frout_right);	
        TIM_SetCompare3(TIM4, behind_lift);	
        TIM_SetCompare4(TIM4, behind_right);	
    }
}



//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(u32 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器4时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB外设时钟
	    
  //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH3
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //TIM_CH4
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
  //初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM4 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIM4 OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR1上的预装载寄存器
	//初始化TIM4 Channel2 PWM模式	 
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIM4 OC2
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器
	//初始化TIM4 Channel3 PWM模式	 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIM4 OC3
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR3上的预装载寄存器
	//初始化TIM4 Channel4 PWM模式	 
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIM4 OC4
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR4上的预装载寄存器
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
}


