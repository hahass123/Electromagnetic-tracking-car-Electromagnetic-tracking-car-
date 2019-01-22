/*******************************************************************
电机控制头文件
2018.12.29 19：56
把用户自定义头文件集中
*******************************************************************/

/*Define to prevent recursive inclusion--------------------------------*/
#ifndef __MOTOR_H__
#define __MOTOR_H__

//两个L298N模块，每个四个逻辑输入控制电机正反转
#define MODULEA_IN1 PBout(0)
#define MODULEA_IN2 PBout(1)
#define MODULEA_IN3 PBout(10)
#define MODULEA_IN4 PBout(11)
#define MODULEB_IN1 PBout(12)
#define MODULEB_IN2 PBout(13)
#define MODULEB_IN3 PBout(14)
#define MODULEB_IN4 PBout(15)

/*Include-----------------------------------------------------------------*/
#include "sys.h"

/*Founctions-----------------------------------------------------------*/
void Motor_Control_Init(void);

#endif





