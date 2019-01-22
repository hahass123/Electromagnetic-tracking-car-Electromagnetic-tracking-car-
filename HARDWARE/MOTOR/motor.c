#include "motor.h"

/*************************************************************************
  * @brief  ������Ƶ�GPIO���ú���
  * @param  ��
  * @retval None
	* @direction  ��ʼ��PB�ڵ�0��1��10��11��12��13��14��15�����Ƶ������ת
 *************************************************************************/
void Motor_Control_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;                                          //�������� ����
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ�Ӻ���   PBʱ�� PEʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //LED0-->PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ģʽ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5����
	GPIO_SetBits(GPIOB,GPIO_Pin_13);						 //PB.5 �����                            ��ʼ��  Ĭ���Ǹߵ�ƽ
}





