/**
  ******************************************************************************
  * @文件名		Rec433.h
  * @简介	 	函数功能实现
  ******************************************************************************
  * @作者		HiBo
  * @时间		2025-02-13
  * @注意		
  *
  *
  ******************************************************************************
*/

#include "Rec433.h"
#include "tim.h"

// 定义433信号开始接收标志位
volatile uint8_t receive_start_flag = 0;
// 定义接收数据的状态，例如：0-未开始，1-接收中，2-接收完成
volatile uint8_t receive_state = 0;
// 用于存储接收到的数据（根据需要调整大小）
uint32_t received_data; // 假设最多接收32bit数据
uint32_t received_data_buffer; // 假设最多接收32bit数据
// 当前接收数据的索引
volatile uint8_t data_index = 0;
uint16_t counterValue;
// 外部中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{
	
	if (GPIO_Pin == GPIO_PIN_4) 
	{
		// 处理边沿变化
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_RESET)
		{
			if(receive_start_flag == 1)
			{
				// 上升沿，停止定时器并读取计数值
				HAL_TIM_Base_Stop_IT(&htim3);
	 
				// 读取TIM3的当前计数值
				counterValue = __HAL_TIM_GET_COUNTER(&htim3);
				__HAL_TIM_SET_COUNTER(&htim3,0);
					// 判断是高位还是低位
					if(counterValue < 400)
					{
						// 低位信号
						received_data = received_data << 1;
					}
					else
					{
						// 判断已经超过一个接收周期，就结束接收
						if(counterValue > 900)
						{
							// 数据超时
							receive_start_flag = 0;
							data_index = 0;
							received_data = 0;
							
							
						}
						else
						{
							// 高位信号
							received_data++;
							received_data = received_data << 1;
						}
					}
					data_index++;
					// 接收完成
					if(data_index > 23)
					{
						receive_start_flag = 0;
						data_index = 0;
						received_data = received_data >> 1;
						received_data_buffer = received_data;
						received_data = 0;
						
					}
			}
			else
			{
				
				__HAL_TIM_SET_COUNTER(&htim3,0);
				// 下降沿，可能表示信号开始，启动定时器
				HAL_TIM_Base_Start_IT(&htim3);
				data_index = 0; // 重置数据索引
			}
		}
		else
		{
			if(receive_start_flag == 0)
			{
				// 上升沿，停止定时器并读取计数值
				//HAL_TIM_Base_Stop_IT(&htim3);
	 
				// 读取TIM3的当前计数值
				counterValue = __HAL_TIM_GET_COUNTER(&htim3);
				__HAL_TIM_SET_COUNTER(&htim3,0);
				// 判断计数器的数值
				if(counterValue > 8200)
				{
					// 计数值超过阈值，认为是一个有效的开始信号
					receive_start_flag = 1;
					
				}
					data_index = 0;
					received_data_buffer = 0;
					received_data = 0;
			}
			else
			{
				__HAL_TIM_SET_COUNTER(&htim3,0);
				// 上升沿，可能表示数据开始，启动定时器
				HAL_TIM_Base_Start_IT(&htim3);
			}
		}
	}
}

// 获取数据
uint32_t Get_Rec_Data(void)
{
	uint32_t buffer = received_data_buffer;
	return buffer;
}


// 在你的main.c或其他源文件中，添加这个回调函数来处理定时事件
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
	  HAL_TIM_Base_Stop_IT(&htim3);
	  // 重置接收标志
	  receive_start_flag = 0;
		data_index = 0;
	  
	  
    // 在这里处理TIM3的溢出事件，比如切换LED状态
    //HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // 假设你有一个LED连接在GPIOB_PIN0
  }
}


