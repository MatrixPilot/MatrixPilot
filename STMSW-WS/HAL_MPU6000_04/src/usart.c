/**
  ******************************************************************************
  * File Name          : USART.c
  * Date               : 04/04/2015 18:38:13
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#include "usart.h"
#include "gpio.h"
#include "stm32f4xx_hal_uart.h"

/* USER CODE BEGIN 0 */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END 0 */

UART_HandleTypeDef huart6;

/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart6);

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* Peripheral clock enable */
    __USART6_CLK_ENABLE();
  
    /**USART6 GPIO Configuration    
    PA11     ------> USART6_TX
    PA12     ------> USART6_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __USART6_CLK_DISABLE();
  
    /**USART6 GPIO Configuration    
    PA11     ------> USART6_TX
    PA12     ------> USART6_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart6, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
/* USER CODE END 1 */
// ***********************
// Funcion para enviar un caracter por el UART 2
void UART_Sendchar(char c)
{
	  HAL_UART_Transmit(&huart6, (uint8_t *)&c, 1, 0xFFFF);
}


// ***********************
// Funcion para enviar una cadena de caracteres por el UART 2
void UART_PrintString(char *pcString)
{
	int i = 0;
	// loop through until reach string's zero terminator
	while (pcString[i] != 0) {
		UART_Sendchar(pcString[i]); // print each character
		i++;
	}
}

void UART_printBytes(uint8_t* buf, uint32_t len) {
    // transfer all bytes to HW Tx FIFO
    while ( len != 0 ) {
        // send next byte
        UART_Sendchar(*buf);

        // update the buf ptr and length
        buf++;
        len--;
    }
}
void UART_printUint32(uint32_t n, uint8_t base) {
    uint32_t i = 0;
    uint8_t buf[8 * sizeof(uint32_t)]; // binary is the largest

    // check for zero case, print and bail out if so
    if (n == 0) {
    	UART_Sendchar((uint8_t)'0');
        return;
    }

    while (n > 0) {
        buf[i] = n % base;
        i++;
        n /= base;
    }

    for (; i > 0; i--) {
        if (buf[i - 1] < 10)
        	UART_Sendchar((uint8_t)('0' + buf[i - 1]));
        else
        	UART_Sendchar((uint8_t)('A' + buf[i - 1] - 10));
    }
}
void UART_printInt32(int32_t n, uint8_t base) {
    uint32_t i = 0;

    // print '-' for negative numbers, also negate
    if (n < 0) {
    	UART_Sendchar((uint8_t)'-');
        n = ((~n) + 1);
    }

    // cast to unsigned and print using uint32_t printer
    i = n;
    UART_printUint32(i, base);
}

void UART_printDouble(double n, uint8_t frac_digits) {
    uint8_t i;
    uint32_t i32;
    double rounding, remainder;

    // test for negatives
    if (n < 0.0) {
    	UART_Sendchar((uint8_t)'-');
        n = -n;
    }

    // round correctly so that print(1.999, 2) prints as "2.00"
    rounding = 0.5;
    for (i=0; i<frac_digits; i++)
        rounding /= 10.0;
    n += rounding;

    // extract the integer part of the number and print it
    i32 = (uint32_t)n;
    remainder = n - (double)i32;
    UART_printUint32(i32, 10);

    // print the decimal point, but only if there are digits beyond
    if (frac_digits > 0)
    	UART_Sendchar((uint8_t)'.');

    // extract digits from the remainder one at a time
    while (frac_digits-- > 0) {
        remainder *= 10.0;
        i32 = (uint32_t)remainder;
        UART_printUint32(i32, 10);
        remainder -= i32;
    }
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
