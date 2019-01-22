#include "motor.h"

/*************************************************************************
  * @brief  电机控制的GPIO配置函数
  * @param  无
  * @retval None
	* @direction  初始化PB口的0、1、10、11、12、13、14、15，控制电机正反转
 *************************************************************************/
void Motor_Control_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;                                          //变量类型 变量
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟函数   PB时钟 PE时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //模式推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5函数
	GPIO_SetBits(GPIOB,GPIO_Pin_13);						 //PB.5 输出高                            初始化  默认是高电平
}






