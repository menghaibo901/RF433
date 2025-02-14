/**
  ******************************************************************************
  * @�ļ���		Rec433.h
  * @���	 	��������ʵ��
  ******************************************************************************
  * @����		HiBo
  * @ʱ��		2025-02-13
  * @ע��		
  *
  *
  ******************************************************************************
*/

#include "Rec433.h"
#include "tim.h"

// ����433�źſ�ʼ���ձ�־λ
volatile uint8_t receive_start_flag = 0;
// ����������ݵ�״̬�����磺0-δ��ʼ��1-�����У�2-�������
volatile uint8_t receive_state = 0;
// ���ڴ洢���յ������ݣ�������Ҫ������С��
uint32_t received_data; // ����������32bit����
uint32_t received_data_buffer; // ����������32bit����
// ��ǰ�������ݵ�����
volatile uint8_t data_index = 0;
uint16_t counterValue;
// �ⲿ�жϻص�����
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{
	
	if (GPIO_Pin == GPIO_PIN_4) 
	{
		// ������ر仯
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_RESET)
		{
			if(receive_start_flag == 1)
			{
				// �����أ�ֹͣ��ʱ������ȡ����ֵ
				HAL_TIM_Base_Stop_IT(&htim3);
	 
				// ��ȡTIM3�ĵ�ǰ����ֵ
				counterValue = __HAL_TIM_GET_COUNTER(&htim3);
				__HAL_TIM_SET_COUNTER(&htim3,0);
					// �ж��Ǹ�λ���ǵ�λ
					if(counterValue < 400)
					{
						// ��λ�ź�
						received_data = received_data << 1;
					}
					else
					{
						// �ж��Ѿ�����һ���������ڣ��ͽ�������
						if(counterValue > 900)
						{
							// ���ݳ�ʱ
							receive_start_flag = 0;
							data_index = 0;
							received_data = 0;
							
							
						}
						else
						{
							// ��λ�ź�
							received_data++;
							received_data = received_data << 1;
						}
					}
					data_index++;
					// �������
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
				// �½��أ����ܱ�ʾ�źſ�ʼ��������ʱ��
				HAL_TIM_Base_Start_IT(&htim3);
				data_index = 0; // ������������
			}
		}
		else
		{
			if(receive_start_flag == 0)
			{
				// �����أ�ֹͣ��ʱ������ȡ����ֵ
				//HAL_TIM_Base_Stop_IT(&htim3);
	 
				// ��ȡTIM3�ĵ�ǰ����ֵ
				counterValue = __HAL_TIM_GET_COUNTER(&htim3);
				__HAL_TIM_SET_COUNTER(&htim3,0);
				// �жϼ���������ֵ
				if(counterValue > 8200)
				{
					// ����ֵ������ֵ����Ϊ��һ����Ч�Ŀ�ʼ�ź�
					receive_start_flag = 1;
					
				}
					data_index = 0;
					received_data_buffer = 0;
					received_data = 0;
			}
			else
			{
				__HAL_TIM_SET_COUNTER(&htim3,0);
				// �����أ����ܱ�ʾ���ݿ�ʼ��������ʱ��
				HAL_TIM_Base_Start_IT(&htim3);
			}
		}
	}
}

// ��ȡ����
uint32_t Get_Rec_Data(void)
{
	uint32_t buffer = received_data_buffer;
	return buffer;
}


// �����main.c������Դ�ļ��У��������ص�����������ʱ�¼�
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
	  HAL_TIM_Base_Stop_IT(&htim3);
	  // ���ý��ձ�־
	  receive_start_flag = 0;
		data_index = 0;
	  
	  
    // �����ﴦ��TIM3������¼��������л�LED״̬
    //HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // ��������һ��LED������GPIOB_PIN0
  }
}


