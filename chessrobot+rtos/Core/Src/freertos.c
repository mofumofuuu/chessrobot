/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "H_Oled.h"
#include "tim.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t rx_buffer=1;
uint8_t byteNumber=5;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId OLED_TASKHandle;
osThreadId MOTOR_TASKHandle;
osThreadId SERIAL_TASKHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Oled_task(void const * argument);
void Motor_task(void const * argument);
void Serial_task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of OLED_TASK */
  osThreadDef(OLED_TASK, Oled_task, osPriorityLow, 0, 128);
  OLED_TASKHandle = osThreadCreate(osThread(OLED_TASK), NULL);

  /* definition and creation of MOTOR_TASK */
  osThreadDef(MOTOR_TASK, Motor_task, osPriorityAboveNormal, 0, 128);
  MOTOR_TASKHandle = osThreadCreate(osThread(MOTOR_TASK), NULL);

  /* definition and creation of SERIAL_TASK */
  osThreadDef(SERIAL_TASK, Serial_task, osPriorityHigh, 0, 128);
  SERIAL_TASKHandle = osThreadCreate(osThread(SERIAL_TASK), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Oled_task */
/**
* @brief Function implementing the OLED_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Oled_task */
void Oled_task(void const * argument)
{
  /* USER CODE BEGIN Oled_task */
  OLED_Init();
  OLED_Clear();
  int temp=0;
  /* Infinite loop */
  for(;;)
  {
    temp++;
    OLED_ShowNum(1,1,temp,3);
    osDelay(50);
  }
  /* USER CODE END Oled_task */
}

/* USER CODE BEGIN Header_Motor_task */
/**
* @brief Function implementing the MOTOR_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Motor_task */
void Motor_task(void const * argument)
{
  /* USER CODE BEGIN Motor_task */
   //电机一启用方向正
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
  //电机二启用方向正
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
  //电机三启用方向正
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);
  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,5000);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,5000);
  __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,5000);

  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
  /* Infinite loop */
  for(;;)
  {
    osDelay(5);
  }
  /* USER CODE END Motor_task */
}

/* USER CODE BEGIN Header_Serial_task */
/**
* @brief Function implementing the SERIAL_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Serial_task */
void Serial_task(void const * argument)
{
  /* USER CODE BEGIN Serial_task */
  
  //HAL_UART_Receive_IT(&huart2,&rx_buffer,1);
  /* Infinite loop */
  for(;;)
  {
    vTaskSuspendAll();
    //HAL_UART_Transmit(&huart2,&rx_buffer,1,HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2,&byteNumber,1,HAL_MAX_DELAY);
    xTaskResumeAll();
    osDelay(1000);
  }
  /* USER CODE END Serial_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart==&huart2){
		//HAL_UART_Transmit(&huart2,&rx_buffer,1,HAL_MAX_DELAY);
		HAL_UART_Receive_IT(&huart2,&rx_buffer,1);
	}
}
/* USER CODE END Application */
