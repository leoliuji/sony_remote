/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "usart.h"
#include "dma.h"
#include "lpk_queue.h"
#include "lpk_type.h"
#include "lpk_util.h"
#include "slip.h"
#include "iwdg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

u8 buff_8bit[1024] = {0};//队列数组
u8 buff_9bit[1024] = {0};//队列数组
QUE_TYPE que_8bit = {buff_8bit, sizeof(buff_8bit), 0, 0, 0};//队列定义
QUE_TYPE que_9bit = {buff_9bit, sizeof(buff_9bit), 0, 0, 0};//队列定义

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId TaskUart8to9Handle;
osThreadId TaskUart9to8Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

u8 GetEven(u8 x);
u8 Sum8Check(u8 *buf, u32 len);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void FuncTaskUart8to9(void const * argument);
void FuncTaskUart9to8(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityAboveNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of TaskUart8to9 */
  osThreadDef(TaskUart8to9, FuncTaskUart8to9, osPriorityNormal, 0, 4096);
  TaskUart8to9Handle = osThreadCreate(osThread(TaskUart8to9), NULL);

  /* definition and creation of TaskUart9to8 */
  osThreadDef(TaskUart9to8, FuncTaskUart9to8, osPriorityNormal, 0, 4096);
  TaskUart9to8Handle = osThreadCreate(osThread(TaskUart9to8), NULL);

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
	 for(;;)
  {
		HAL_IWDG_Refresh(&hiwdg);

    osDelay(500);
    // HAL_UART_Transmit_DMA(&huart1, "tick\r\n", 7);//DMA数据发�??
  }
	
  /* Infinite loop */

  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_FuncTaskUart8to9 */
/**
* @brief Function implementing the TaskUart8to9 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_FuncTaskUart8to9 */
void FuncTaskUart8to9(void const * argument)
{
  /* USER CODE BEGIN FuncTaskUart8to9 */
	
	  u8 data_recv[1024] = "";//数据接收缓冲�?
  u8 *data_recv_tmp = data_recv;
  u8 data_unslip[1024] = "";
  u8 data_send[1024] = "";//数据发�?�DMA缓冲�?
  u32 len = 0;
  u32 len_out;
  u32 len_remain = 0;
  u32 i = 0;
	//osDelay(1005);
	
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);//打开uart1数据接收中断
  DmaRxStart(&huart1);//�?始DMA数据接收

	
  /* Infinite loop */
    for(;;)
  {
    // osDelay(100);
    //?????? ,?????????
    ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

    len = QuePop(&que_8bit, data_recv, sizeof(data_recv));//???????
    HAL_GPIO_TogglePin(GPIOC, Tally_led_Pin);

    data_recv_tmp = SlipUnpacket(data_recv, len, data_unslip, &len_out);

    while(len_out > 0){
      SEGGER_RTT_printf(0,"receive from wirless\r\n");
      
      // while(HAL_UART_Transmit_DMA(&huart1, "mark1\r\n", 7) != HAL_OK);//DMA数据发�??
      if(Sum8Check(data_unslip, len_out)){
        for(i = 0; i < len_out; i++){
          data_send[i << 1] = data_unslip[i];
          if(i < 3){
            data_send[(i << 1) + 1] = GetEven(data_unslip[i]);//??????
          }else{
            data_send[(i << 1) + 1] = GetEven(data_unslip[i]) ? 0 : 1;//??????
          }
        }
      }
      while(HAL_UART_Transmit_DMA(&huart2, data_send, len_out) != HAL_OK);//DMA��据发???
      SEGGER_RTT_printf(0,"send to cam\r\n");
			
      len_remain = len - (data_recv_tmp - data_recv);
      data_recv_tmp = SlipUnpacket(data_recv_tmp, len_remain, data_unslip, &len_out);
    }
    
  }
  /* USER CODE END FuncTaskUart8to9 */
}

/* USER CODE BEGIN Header_FuncTaskUart9to8 */
/**
* @brief Function implementing the TaskUart9to8 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_FuncTaskUart9to8 */
void FuncTaskUart9to8(void const * argument)
{
  /* USER CODE BEGIN FuncTaskUart9to8 */
	
	
	u8 data_recv[1024] = "";//数据接收缓冲�?
  u8 data_send[1024] = "";//数据发�?�DMA缓冲�?  
  u8 data_send_slip[1024] = "";
  u32 index_data_send = 0;
  u32 len = 0;
  u32 i = 0;
  u32 len_frame = 0;
	//osDelay(1000);
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);//打开uart2数据接收中断
  DmaRxStart(&huart2);//�?始DMA数据接收
	
  /* Infinite loop */
  for(;;)
  {
    // osDelay(100);
    //?????? ,?????????
    ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

    len = QuePop(&que_9bit, data_recv, sizeof(data_recv) >> 1);

     HAL_GPIO_TogglePin(GPIOC, Data_led_Pin);
    
    if(len > 0){
      SEGGER_RTT_printf(0,"receive from cam\r\n");
      for(i = 0; i < len; i += 2){
        switch(index_data_send){
            case 0:
            if(data_recv[i + 1] == GetEven(data_recv[i])){//????????
              data_send[index_data_send++] = data_recv[i];
              // while(HAL_UART_Transmit_DMA(&huart1, "mark1\r\n", 7) != HAL_OK);//DMA数据发�??
            }
            break;
          case 1:
            if(data_recv[i + 1] == GetEven(data_recv[i])){//????????
              len_frame = data_recv[i] + 3;
              if(IN_RANGE(len_frame, 5, 64)){
                data_send[index_data_send++] = data_recv[i];
                // while(HAL_UART_Transmit_DMA(&huart1, "mark2\r\n", 7) != HAL_OK);//DMA数据发�??
              }else{
                data_send[0] = data_recv[i];
                index_data_send = 1;
              }
            }else{
              index_data_send = 0;//??????
            }
            break;
          case 2:
            if(data_recv[i + 1] == GetEven(data_recv[i])){//????????
                data_send[index_data_send++] = data_recv[i];
                // while(HAL_UART_Transmit_DMA(&huart1, "mark3\r\n", 7) != HAL_OK);//DMA数据发�??
            }else{
              index_data_send = 0;//??????
            }
           break;
          default:
            if(data_recv[i + 1] == (GetEven(data_recv[i]) ? 0 : 1)){//???
              data_send[index_data_send++] = data_recv[i];
              if(index_data_send >= len_frame){
                if(Sum8Check(data_send, len_frame)){
                  // while(HAL_UART_Transmit_DMA(&huart1, "mark4\r\n", 7) != HAL_OK);//DMA数据发�??
                  len_frame = SlipPacket(data_send, len_frame, data_send_slip);
                  while(HAL_UART_Transmit_DMA(&huart1, data_send_slip, len_frame) != HAL_OK);//DMA数据发�??
                  SEGGER_RTT_printf(0,"send to wirless\r\n");
									//  HAL_GPIO_TogglePin(GPIOC, Data_led_Pin);
									//SEGGER_RTT_printf(0,"%x ",buf[i]);
                }
                index_data_send = 0;//????????
              }
            }else{//????????
              data_send[0] = data_recv[i];
              index_data_send = 1;
            }
        }//end switch
      }//end for
    }
  }
  /* USER CODE END FuncTaskUart9to8 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
 * @brief  ??????
 * @note   
 * @param  x: ??????
 * @retval ???
 */
u8 GetEven(u8 x)
{
  u8 i = 0, ret = 0;
  for(i = 0; i < 8; i++){
    if(x & 0x01){
      ret++;
    }
    x >>= 1;
  }
  return (ret & 0x01);
}

/**
 * @brief  
 * @note   
 * @param  buf: 
 * @param  len: 
 * @retval 
 */
u8 Sum8Check(u8 *buf, u32 len)
{
	u8 sum = 0;
  u8 i = 0;
	
	if(len < 1){
    return 0;
  }

	for(i = 0; i < len - 1; i++){
		sum += buf[i];
	}

	sum = ( u8 ) ( 0x100 - sum );
	
	if( sum == buf[len-1] )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
