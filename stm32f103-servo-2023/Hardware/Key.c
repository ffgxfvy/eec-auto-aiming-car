#include "stm32f10x.h"      // Device header
#include "Delay.h"

/**
  * 函    数：按键初始化 (新引脚版本)
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
    /* 开启时钟 */
    // 【修改】因为引脚分布在GPIOA和GPIOB，所以两个端口的时钟都要开启
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    /* 【新增】禁用JTAG，使能SWD，释放PA15, PB3, PB4作为普通IO */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /* GPIOA 初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    // 【修改】配置 PA9, PA11, PA15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_15; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* GPIOB 初始化 */
    // 【修改】单独配置 PB4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * 函    数：按键获取键码 (新引脚版本)
  * 参    数：无
  * 返 回 值：按下按键的键码值，范围：0~4，返回0代表没有按键按下
  */
uint8_t Key_GetNum(void)
{
    // 【修改】检测按键1 (PA9)
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0)
    {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0);
        Delay_ms(20);
        return 1;
    }
    
    // 【修改】检测按键2 (PA11)
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
    {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0);
        Delay_ms(20);
        return 2;
    }
    
    // 【修改】检测按键3 (PA15)
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
    {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0);
        Delay_ms(20);
        return 3;
    }
    
    // 【修改】检测按键4 (PB4) - 现已用于复位
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
    {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0);
        Delay_ms(20);
        return 4; // 返回键码4
    }
    
    return 0; // 无按键按下
}