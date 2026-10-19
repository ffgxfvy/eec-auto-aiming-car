#include "stm32f10x.h"
#include <stdlib.h>
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "PWM.h"
#include "Key.h"

// --- 轨迹参数 ---
#define Y_MIN_ANGLE         140.0f
#define Y_MAX_ANGLE         150.0f
#define X_MIN_ANGLE         120.0f
#define X_MAX_ANGLE         145.0f

// --- 行为控制参数 ---
#define MOVEMENT_STEP       8.0f
#define ACTION_INTERVAL_MS  5000

// --- 全局状态变量 ---
float currentAngleX = (X_MIN_ANGLE + X_MAX_ANGLE) / 2.0f;
float currentAngleY = (Y_MIN_ANGLE + Y_MAX_ANGLE) / 2.0f;
int directionX = 1;
int directionY = 1;
uint8_t is_running = 1; // 1=运行, 0=暂停

// 定义复位函数
void ResetServoPositions(void)
{
    // 复位到初始角度
    currentAngleX = (X_MIN_ANGLE + X_MAX_ANGLE) / 2.0f;
    currentAngleY = (Y_MIN_ANGLE + Y_MAX_ANGLE) / 2.0f;
    directionX = 1; // 复位方向
    directionY = 1;

    PWM_SetAngle(1, currentAngleY);
    PWM_SetAngle(2, currentAngleX);

    OLED_ShowNum(1, 9, (uint16_t)currentAngleY, 3);
    OLED_ShowNum(2, 9, (uint16_t)currentAngleX, 3);
    Serial_Printf("System Reset: Servos to initial position.\r\n");
}


int main(void)
{
    uint32_t time_counter = 0;
    
    /* 模块初始化 */
    OLED_Init();        
    Serial_Init();      
    PWM_Init();
    Key_Init();
    
    /* 设置舵机初始位置 */
    PWM_SetAngle(1, currentAngleY);
    PWM_SetAngle(2, currentAngleX);
    
    /* OLED及串口欢迎信息 */
    OLED_ShowString(1, 1, "Y-Angle:");
    OLED_ShowString(2, 1, "X-Angle:");
    OLED_ShowString(4, 1, "State: Running "); // 初始化时显示状态
    OLED_ShowNum(1, 9, (uint16_t)currentAngleY, 3);
    OLED_ShowNum(2, 9, (uint16_t)currentAngleX, 3);
    
    Serial_Printf("\r\nServo Step-Patrol Mode\r\n");
    Serial_Printf("Press KEY2 (PA11) to Start/Stop.\r\n");
    Serial_Printf("Press KEY4 (PB4) to Reset.\r\n"); // 添加复位按键提示

    while (1)
    {
        uint8_t key_num = Key_GetNum();
        if (key_num == 2) // 如果按键2 (PA11) 被按下
        {
            is_running = !is_running;
            
            // 按下按键后，根据新状态更新OLED
            if (is_running)
            {
                OLED_ShowString(4, 1, "State: Running ");
                Serial_Printf("Movement Resumed.\r\n");
            }
            else
            {
                OLED_ShowString(4, 1, "State: Paused  ");
                Serial_Printf("Movement Paused.\r\n");
            }
        }
        else if (key_num == 4) // 如果按键4 (PB4) 被按下
        {
            OLED_ShowString(4, 1, "State: Resetting"); // 显示复位状态
            ResetServoPositions(); // 调用复位函数
            Delay_ms(500); // 稍微延时，让用户看到“Resetting”
            // 复位后，回到之前的运行状态（如果原来是运行，则继续运行；如果原来是暂停，则继续暂停）
            if (is_running) {
                OLED_ShowString(4, 1, "State: Running ");
            } else {
                OLED_ShowString(4, 1, "State: Paused  ");
            }
        }

        if (is_running)
        {
            time_counter++;
            
            if (time_counter >= (ACTION_INTERVAL_MS / 10))
            {
                time_counter = 0;

                // ... 执行舵机移动逻辑 ...
                if (currentAngleY >= Y_MAX_ANGLE) { directionY = -1; }
                else if (currentAngleY <= Y_MIN_ANGLE) { directionY = 1; }
                currentAngleY += (MOVEMENT_STEP * directionY);

                if (currentAngleX >= X_MAX_ANGLE) { directionX = -1; }
                else if (currentAngleX <= X_MIN_ANGLE) { directionX = 1; }
                currentAngleX += (MOVEMENT_STEP * directionX);
                
                PWM_SetAngle(1, currentAngleY);
                PWM_SetAngle(2, currentAngleX);

                // ... 更新角度值显示 ...
                OLED_ShowNum(1, 9, (uint16_t)currentAngleY, 3);
                OLED_ShowNum(2, 9, (uint16_t)currentAngleX, 3);
                Serial_Printf("Tick -> Y: %.0f, X: %.0f\r\n", currentAngleY, currentAngleX);
            }
        }
        
        Delay_ms(10);
    }
}