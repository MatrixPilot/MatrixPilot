/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @date    25/04/2015 20:33:26
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_tx;
extern DMA_HandleTypeDef hdma_spi2_rx;
extern double tempC;
extern double X_accel, Y_accel, Z_accel;
uint8_t data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI Line0 interrupt.
*/
void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
* @brief This function handles SPI2 global interrupt.
*/
void SPI2_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi2);
}

/* USER CODE BEGIN 1 */
/**
* @brief This function handles DMA1 Stream3 global interrupt.
*/
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi2_rx);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{

	int16_t tmp=0;
    tmp=(data[6]<<8) | data[7];
    tempC = (double)(tmp)/340.0 + 36.53;
    X_accel = (int16_t)((data[0]<<8) | data[1])/16384.0;
    Y_accel = (int16_t)((data[2]<<8) | data[3])/16384.0;
    Z_accel = (int16_t)((data[4]<<8) | data[5])/16384.0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

/**
* @brief This function handles DMA1 Stream4 global interrupt.
*/
void DMA1_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream4_IRQn 0 */

  /* USER CODE END DMA1_Stream4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
  /* USER CODE BEGIN DMA1_Stream4_IRQn 1 */

  /* USER CODE END DMA1_Stream4_IRQn 1 */
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t addr;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    addr=0x3B|0x80;
//    data[0]=0x3B|0x80;
    HAL_SPI_Transmit(&hspi2, &addr, 1, 10);
    HAL_SPI_Receive_DMA(&hspi2, data, 16);
//    HAL_SPI_TransmitReceive_DMA(&hspi2, &addr, data, 17);
}
/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	uint8_t data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//
//	int16_t tmp=0;
//
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
//    data[1]=0x3B|0x80;
//    HAL_SPI_Receive(&hspi2, data, 8, 0x10);
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
//    tmp=(data[6]<<8) | data[7];
//    tempC = (double)(tmp)/340.0 + 36.53;
//    X_accel = (int16_t)((data[0]<<8) | data[1])/16384.0;
//    Y_accel = (int16_t)((data[2]<<8) | data[3])/16384.0;
//    Z_accel = (int16_t)((data[4]<<8) | data[5])/16384.0;
//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
//}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
