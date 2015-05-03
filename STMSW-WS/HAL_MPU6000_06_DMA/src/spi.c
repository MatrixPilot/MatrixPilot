/**
  ******************************************************************************
  * File Name          : SPI.c
  * Date               : 25/04/2015 20:33:26
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
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
#include "spi.h"
#include "gpio.h"
#include "dma.h"
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */
//Sensor variables
//NOTE: has to be 8bit not 16, so 16 data position
uint8_t mpu_data[16], mpuCnt = 0;


SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_spi2_rx;

/* SPI2 init function */
void MX_SPI2_Init(void)
{
	//SPI2 is on APB1. APB1 max freq is 42 or 50 MHz (CCLK/2)
	//So 1MHz is max mpu6000 config freq. We need a division by 42/52.
	//We choose 64 and obtain 42MHz/64 = 656Khz.
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  //TODO: I can't figure out how to work on 16bit mode....
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
//  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  HAL_SPI_Init(&hspi2);

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspInit 0 */

  /* USER CODE END SPI2_MspInit 0 */
    /* Peripheral clock enable */
    __SPI2_CLK_ENABLE();
  
    /**SPI2 GPIO Configuration    
    PC2     ------> SPI2_MISO
    PC3     ------> SPI2_MOSI
    PB10     ------> SPI2_SCK
    PB12     ------> SPI2_NSS 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10; //|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral DMA init*/

    hdma_spi2_tx.Instance = DMA1_Stream4;
    hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_tx.Init.MemInc = DMA_MINC_DISABLE;
    hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi2_tx.Init.Mode = DMA_NORMAL;
    hdma_spi2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_spi2_tx);

    __HAL_LINKDMA(hspi,hdmatx,hdma_spi2_tx);

    hdma_spi2_rx.Instance = DMA1_Stream3;
    hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi2_rx.Init.Mode = DMA_NORMAL;
    hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_spi2_rx);

    __HAL_LINKDMA(hspi,hdmarx,hdma_spi2_rx);

    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(SPI2_IRQn, 0, 5);
    HAL_NVIC_EnableIRQ(SPI2_IRQn);
  /* USER CODE BEGIN SPI2_MspInit 1 */

  /* USER CODE END SPI2_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

  if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */

  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __SPI2_CLK_DISABLE();
  
    /**SPI2 GPIO Configuration    
    PC2     ------> SPI2_MISO
    PC3     ------> SPI2_MOSI
    PB10     ------> SPI2_SCK
    PB12     ------> SPI2_NSS 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_12);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(hspi->hdmatx);
    HAL_DMA_DeInit(hspi->hdmarx);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(SPI2_IRQn);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }
} 

/*************************************************************************************/
// This will be mpu6000.c
/* USER CODE BEGIN 1 */

void MPU6000_init16(void)
{
//	callback = fptr;

// MPU-6000 maximum SPI clock is specified as 1 MHz for all registers
//    however the datasheet states that the sensor and interrupt registers
//    may be read using an SPI clock of 20 Mhz

// As these register accesses are one time only during initial setup lets be
//    conservative and only run the SPI bus at half the maximum specified speed

	HAL_StatusTypeDef err;
	// need at least 60 msec delay here
	HAL_Delay(60);
	err = writeMPUSPIreg16(MPUREG_PWR_MGMT_1, BIT_H_RESET);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
	// 10msec delay seems to be needed for AUAV3 (MW's prototype)
	HAL_Delay(10);
	// Wake up device and select GyroZ clock (better performance)
	err = writeMPUSPIreg16(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
//	func_SPI_Write_Byte((MPUREG_PWR_MGMT_1|0x80),0x00, data);
	// Disable I2C bus (recommended on datasheet)
	err = writeMPUSPIreg16(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
	// SAMPLE RATE
	err = writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 4); // Sample rate = 200Hz  Fsample= 1Khz/(N+1) = 200Hz
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
	// scaling & DLPF
	err = writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}

//	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS);  // Gyro scale 2000º/s
	err = writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}

#if (ACCEL_RANGE == 2)
	err = writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scele +-2g, g = 8192
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
#elif (ACCEL_RANGE == 4)
	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale g = 4096
#elif (ACCEL_RANGE == 8)
	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale g = 2048
#else
#error "Invalid ACCEL_RANGE"
#endif

#if 0
	// Legacy from Mark Whitehorn's testing, we might need it some day.
	// SAMPLE RATE
	writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 7); // Sample rate = 1KHz  Fsample= 8Khz/(N+1)

	// no DLPF, gyro sample rate 8KHz
	writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_256HZ_NOLPF2);

	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s

//	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scale 2g, g = 16384
	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale g = 8192
//	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale g = 4096
#endif

	// INT CFG => Interrupt on Data Ready, totem-pole (push-pull) output INT: Clear on any read
	err = writeMPUSPIreg16(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_INT_RD_CLEAR);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
	// INT: Raw data ready
	err = writeMPUSPIreg16(MPUREG_INT_ENABLE, BIT_DATA_RDY_EN);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}

// Bump the SPI clock up towards 10.5 MHz for ongoing sensor and interrupt register reads
// 20 MHz is the maximum specified for the MPU-6000
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	HAL_SPI_Init(&hspi2);

	//Enable MPU INT
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}


// Blocking 16 bit write to SPI
HAL_StatusTypeDef writeMPUSPIreg16(uint8_t addr, uint8_t cmd)
{
	HAL_StatusTypeDef err;
	uint16_t i;
	uint8_t dato[2] = {0,0};
//	dato[1] = addr;
//	dato[0] = cmd;
	dato[0] = addr;
	dato[1] = cmd;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	err = HAL_SPI_Transmit(&hspi2, dato, 2, 10);
//	err = HAL_SPI_Transmit(&hspi2, dato, 1, 10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    for(i=0;i<0x0F;i++);
    return err;
}
// SPI module has 8 word FIFOs
// burst read 2n bytes starting at addr;
// Since first byte is address, max of 15 data bytes may be transferred with n=7
void readMPUSPI_burst16n(uint8_t data[], int16_t n, uint16_t addr, void (*call_back)(void))
{
	HAL_StatusTypeDef err;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    //TODO: The problem here is SPI_Receive need address on data[0]. It will call
    //HAL_SPI_TransmitReceive_IT->SPI_TxISR->SPI_TxColseIRQHandler->HAL_SPI_TxCpltCallback
    //wWe should implement HAL_SPI_TxCpltCallback to do what we need to do.
    data[0] = addr;
	err = HAL_SPI_Receive_IT(&hspi2, data, 2*n);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
//	uint16_t i;
//
//	MPU_SS = 0;                 // assert chip select
//	mpu_call_back = call_back;  // store the address of the call back routine
//	SPI_data = &data[0];        // store address of data buffer
//	i = SPIBUF;                 // empty read buffer
//	addr |= 0x80;               // write address-1 in high byte + n-1 dummy words to TX FIFO
//	SPIBUF = addr << 8;         // issue read command
//	for (i = 0; i < n; i++) {
//		SPIBUF = 0;             // queue 'n' null words into the SPI transmit buffer
//	}
//	_SPIIE = 1;                 // turn on SPI interrupts
}


/* USER CODE END 1 */


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
