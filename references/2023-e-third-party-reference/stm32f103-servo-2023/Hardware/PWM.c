// PWM.c
#include "stm32f10x.h"
#include "PWM.h"

/*
 * ====================================================================
 * --- 新增：舵机校准参数 ---
 * ====================================================================
 * 请根据您270度舵机的实际情况来微调这里的参数
 *
 * SERVO_MIN_COMPARE: 舵机在 0 度位置时，需要的PWM比较值。(标准值500)
 * SERVO_MAX_COMPARE: 舵机在 270 度位置时，需要的PWM比较值。(标准值2500)
 * SERVO_ANGLE_RANGE: 舵机的最大角度范围。
 */
#define SERVO_MIN_COMPARE   500
#define SERVO_MAX_COMPARE   2500
#define SERVO_ANGLE_RANGE   270.0f
// ====================================================================


/**
 * @brief  PWM初始化 (此函数保持不变)
 * @param  无
 * @retval 无
 */
void PWM_Init(void)
{
	/* 开启时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* GPIO初始化 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 配置时钟源 */
	TIM_InternalClockConfig(TIM2);

	/* 时基单元初始化 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	/* 输出比较初始化 - 通道2 (PA1) */
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	/* 输出比较初始化 - 通道3 (PA2) */
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	
	/* TIM使能 */
	TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  PWM设置通道2的CCR值 (此函数保持不变)
 * @param  Compare 要写入的CCR的值，范围：0~20000
 * @retval 无
 */
void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(TIM2, Compare);
}

/**
 * @brief  PWM设置通道3的CCR值 (此函数保持不变)
 * @param  Compare 要写入的CCR的值，范围：0~20000
 * @retval 无
 */
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2, Compare);
}


/**
 * @brief  --- 新增：舵机角度设置函数 ---
 * @brief  将角度转换为PWM比较值，并设置舵机
 * @param  Channel: 通道选择 (1=X轴, 2=Y轴)
 * @param  Angle: 您想设置的目标角度 (范围: 0.0 ~ 270.0)
 * @retval 无
 */
void PWM_SetAngle(uint8_t Channel, float Angle)
{
    uint16_t compare;

    // 1. 限制输入角度在有效范围内
    if (Angle < 0)
    {
        Angle = 0;
    }
    if (Angle > SERVO_ANGLE_RANGE)
    {
        Angle = SERVO_ANGLE_RANGE;
    }

    // 2. 进行精确的线性映射，将角度转换为PWM比较值
    compare = (uint16_t)( (Angle / SERVO_ANGLE_RANGE) * (SERVO_MAX_COMPARE - SERVO_MIN_COMPARE) + SERVO_MIN_COMPARE );

    // 3. 调用底层函数，设置对应通道的比较值
    if (Channel == 1) // 控制X轴舵机
    {
        PWM_SetCompare2(compare);
    }
    else if (Channel == 2) // 控制Y轴舵机
    {
        PWM_SetCompare3(compare);
    }
}