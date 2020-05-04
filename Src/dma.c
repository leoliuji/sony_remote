/**
  ******************************************************************************
  * File Name          : dma.c
  * Description        : This file provides code for the configuration
  *                      of all the requested memory to memory DMA transfers.
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

/* Includes ------------------------------------------------------------------*/
#include "dma.h"

/* USER CODE BEGIN 0 */
#define UART1_RX_BUFF_SIZE  256
#define UART2_RX_BUFF_SIZE  512
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */
static u8 Uart1RxBuf[UART1_RX_BUFF_SIZE];
static u8 Uart2RxBuf[UART2_RX_BUFF_SIZE];
/* USER CODE END 1 */

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/* USER CODE BEGIN 2 */
void DmaRxStart(UART_HandleTypeDef *huart)
{
  if(huart == &huart1){
    HAL_UART_Receive_DMA(&huart1, Uart1RxBuf, UART1_RX_BUFF_SIZE);
  }else if(huart == &huart2){
    HAL_UART_Receive_DMA(&huart2, Uart2RxBuf, UART2_RX_BUFF_SIZE >> 1);
  }
}

int DmaToQueue(UART_HandleTypeDef *huart, QUE_TYPE *que)
{
  u32 size = 0;
  u32 ret = 0;

  if(!huart || !que){
    return -1;
  }

  __HAL_DMA_DISABLE(huart->hdmarx);

  if(huart == &huart1){
    size = UART1_RX_BUFF_SIZE - huart->hdmarx->Instance->NDTR;
    if(size > 0){
      ret = QuePush(que, Uart1RxBuf, size);
    }
    huart->hdmarx->Instance->NDTR = UART1_RX_BUFF_SIZE; 
    
  }else if(huart == &huart2){
    size = (UART2_RX_BUFF_SIZE >> 1) - huart->hdmarx->Instance->NDTR;
    if(size > 0){
      ret = QuePush(que, Uart2RxBuf, size << 1);
    }
    huart->hdmarx->Instance->NDTR = UART2_RX_BUFF_SIZE >> 1;
  }

  __HAL_DMA_ENABLE(huart->hdmarx);

  return (int)ret;
}
/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
