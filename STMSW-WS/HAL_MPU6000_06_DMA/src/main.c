/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
//#include "stm32f4xx_nucleo.h"
#include "stdint.h"
#include "stdio.h"
#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"
#include "dma.h"

volatile double tempC;
volatile double X_accel, Y_accel, Z_accel;

void SystemClock_Config(void);

int main(void)
{
	uint32_t i;
	char msg[50];
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  MX_SPI2_Init();
  MX_USART6_UART_Init();

  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(LED1_Port, LED1, RESET);
  HAL_GPIO_WritePin(LED2_Port, LED2, RESET);
  HAL_GPIO_WritePin(LED3_Port, LED3, RESET);
  HAL_GPIO_WritePin(LED4_Port, LED4, RESET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(LED1_Port, LED1, SET);
  HAL_GPIO_WritePin(LED2_Port, LED2, SET);
  HAL_GPIO_WritePin(LED3_Port, LED3, SET);
  HAL_GPIO_WritePin(LED4_Port, LED4, SET);

  MPU6000_init16();

  /* Infinite loop */
  while (1)
  {
	//for some reason I can't use printf with %f.... WTF!!!!
	//I have to use my old uart_printXXX routine
	sprintf(msg, "\r\nTemperature, unbias Ax, Ay, Az :");
	UART_PrintString(msg);
	UART_printDouble(tempC, 3);
	UART_Sendchar(' ');
	UART_printDouble(X_accel, 3);
	UART_Sendchar(' ');
	UART_printDouble(Y_accel, 3);
	UART_Sendchar(' ');
	UART_printDouble(Z_accel, 3);
	//some delay...
	for(i=0;i<4000000;i++);
  }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

}
