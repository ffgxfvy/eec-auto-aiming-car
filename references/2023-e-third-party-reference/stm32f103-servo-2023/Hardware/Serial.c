#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

// 定义数据包缓冲区和相关变量
uint8_t Serial_RxPacket[100];     // 全局变量，用于存储接收的数据包
uint16_t Serial_RxLen = 0;        // 记录当前接收的长度
uint8_t Serial_RxFlag = 0;        // 接收完成标志位

/**
 * @brief  串口初始化
 * @param  无
 * @retval 无
 */
void Serial_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	/*中断输出配置*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	/*USART使能*/
	USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  串口发送一个字节
 * @param  Byte 要发送的一个字节
 * @retval 无
 */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
 * @brief  串口发送一个数组
 * @param  Array 要发送数组的首地址
 * @param  Length 要发送数组的长度
 * @retval 无
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

/**
 * @brief  串口发送一个字符串
 * @param  String 要发送字符串的首地址
 * @retval 无
 */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

/**
 * @brief  次方函数（内部使用）
 * @retval 返回值等于X的Y次方
 */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

/**
 * @brief  串口发送数字
 * @param  Number 要发送的数字，范围：0~4294967295
 * @param  Length 要发送数字的长度，范围：0~10
 * @retval 无
 */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  使用printf需要重定向的底层函数
 * @param  保持原始格式即可，无需变动
 * @retval 保持原始格式即可，无需变动
 */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

/**
 * @brief  自己封装的prinf函数
 * @param  format 格式化字符串
 * @param  ... 可变的参数列表
 * @retval 无
 */
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

/**
 * @brief  获取串口接收标志位
 * @param  无
 * @retval 串口接收标志位，范围：0~1，接收到数据包后，标志位置1，读取后标志位自动清零
 */
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

/**
 * @brief  USART1中断函数 (数据包接收逻辑)
 * @param  无
 * @retval 无
 */
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		
		if (RxData == '\r' || RxData == '\n')
		{
			if (Serial_RxLen > 0)
			{
				Serial_RxPacket[Serial_RxLen] = '\0';
				Serial_RxFlag = 1;
				Serial_RxLen = 0;
			}
		}
		else
		{
			if (Serial_RxLen < 99)
			{
				Serial_RxPacket[Serial_RxLen] = RxData;
				Serial_RxLen++;
			}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}