/**
  ******************************************************************************
  * File Name          : TIM.c
  * Date               : 24/03/2015 16:08:52
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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
#include "tim.h"
//ToDo: Include interrupt.h to get INT_PRI_IC
#define INT_PRI_IC 6
//#include ".\libUDB\interrupts.h"
#include "options.h"    // To get NUM_IMPUTS

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim3;    // TIM base for PWM CH1 to CH4

/* TIM10 init function */
void MX_TIM10_Init(void)
{

  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 20000;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 0;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim10);

}

/* TIM3 init function */
void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = (uint16_t) ((SystemCoreClock / 1000000) - 1);    //1MHz;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 19999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim3);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  sConfigOC.Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

  sConfigOC.Pulse = 1000;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);

  sConfigOC.Pulse = 1000;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3);

  sConfigOC.Pulse = 10000;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4);

}

/* TIM5 init function */
// It is used as free runing timer for IC
void MX_TIM5_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;


    htim5.Instance = TIM5;
    htim5.Init.Prescaler = (uint16_t) ((SystemCoreClock / 1000000) - 1);    //1MHz
//    htim5.Init.Prescaler = 84-1;     //1MHz
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 0xFFFF;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim5);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig);

    HAL_TIM_IC_Init(&htim5);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);

}

//We are not supporting parallel Input to uC, so we don't need this Inputs
/* TIM4 init function */
// It is used as free runing timer for IC
//void MX_TIM4_Init(void)
//{
//    TIM_ClockConfigTypeDef sClockSourceConfig;
//    TIM_MasterConfigTypeDef sMasterConfig;
//    TIM_IC_InitTypeDef sConfigIC;
//
//    htim4.Instance = TIM4;
//    htim4.Init.Prescaler = (uint16_t) (((SystemCoreClock / 1000000) / 2) - 1);    //500KHz
//    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
//    htim4.Init.Period = 0xFFFF;
//    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//    HAL_TIM_Base_Init(&htim4);
//
//    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//    HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig);
//
//    HAL_TIM_IC_Init(&htim4);
//
//    //ToDo: This timer need to run synchronized with TIM5
//    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//    HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);
//
//    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
//    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
//    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
//    sConfigIC.ICFilter = 0;
//
//#if (USE_PPM_INPUT == 0)
//    // Config and start chanel acording with NUM_IMPUT
//	if (NUM_INPUTS > 2)  {HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1); HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);};
//	if (NUM_INPUTS > 3)  {HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2); HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);};
//	if (NUM_INPUTS > 4)  {HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_3); HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_3);};
//	if (NUM_INPUTS > 5)  {HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_4); HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_4);};
////	if (NUM_INPUTS > 6) IC_INIT(7, REGTOK1, REGTOK2);
////#if (USE_SONAR_INPUT != 8)
////	if (NUM_INPUTS > 7) IC_INIT(8, REGTOK1, REGTOK2);
////#endif // USE_SONAR_INPUT
//#endif // USE_PPM_INPUT
//}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct;

    if(htim_base->Instance==TIM10)
    {
        /* Peripheral clock enable */
        __TIM10_CLK_ENABLE();

        /* Peripheral interrupt init*/
        HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, INT_PRI_IC, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    }
    else if(htim_base->Instance==TIM5)
    {
        /* Peripheral clock enable */
        __TIM5_CLK_ENABLE();
//This is the portion of code tha I deleted from my eclipse
//work. I deleted becouse ther is same initialization on
//HAL_TIM_IC_MspInit. But it seems to be necessary to be here.
//I will investigate this issue
        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Peripheral interrupt init*/
        HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM5_IRQn);
    }
    else if(htim_base->Instance==TIM3)
    {
        /* USER CODE BEGIN TIM3_MspInit 0 */

        /* USER CODE END TIM3_MspInit 0 */
        /* Peripheral clock enable */
        __TIM3_CLK_ENABLE();

        /**TIM3 GPIO Configuration
        PB0     ------> TIM3_CH3
        PB1     ------> TIM3_CH4
        PB4     ------> TIM3_CH1
        PB5     ------> TIM3_CH2
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM3_MspInit 1 */

        /* USER CODE END TIM3_MspInit 1 */
    }
    //We are not supporting parallel Input to uC, so we don't need this Inputs
//    else if(htim_base->Instance==TIM4)
//    {
//        /* Peripheral clock enable */
//        __TIM4_CLK_ENABLE();
//
//        /* Peripheral interrupt init*/
//        HAL_NVIC_SetPriority(TIM4_IRQn, INT_PRI_IC, 0);
//        HAL_NVIC_EnableIRQ(TIM4_IRQn);
//    }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(htim_pwm->Instance==TIM3)
    {
        /* USER CODE BEGIN TIM3_MspInit 0 */

        /* USER CODE END TIM3_MspInit 0 */
        /* Peripheral clock enable */
        __TIM3_CLK_ENABLE();

        /**TIM3 GPIO Configuration
        PB0     ------> TIM3_CH3
        PB1     ------> TIM3_CH4
        PB4     ------> TIM3_CH1
        PB5     ------> TIM3_CH2
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM3_MspInit 1 */

        /* USER CODE END TIM3_MspInit 1 */
    }
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim_ic)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(htim_ic->Instance==TIM5)
    {
        /* Peripheral clock enable */
        __TIM5_CLK_ENABLE();

        /**TIM5 GPIO Configuration
        PA0-WKUP     ------> TIM5_CH1
        PA1     ------> TIM5_CH2
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Peripheral interrupt init*/
        HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM5_IRQn);
    }
        //We are not supporting parallel Input to uC, so we don't need this Inputs
//    else if(htim_ic->Instance==TIM4)
//    {
//        /* Peripheral clock enable */
//        __TIM4_CLK_ENABLE();
//
//        /**TIM4 GPIO Configuration
//        PB6     ------> TIM4_CH1
//        PB7     ------> TIM4_CH2
//        PB8     ------> TIM4_CH3
//        PB9     ------> TIM4_CH4
//        */
//        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
//        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
//        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
//        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//        /* Peripheral interrupt init*/
//        HAL_NVIC_SetPriority(TIM4_IRQn, INT_PRI_IC, 0);
//        HAL_NVIC_EnableIRQ(TIM4_IRQn);
//    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

    if(htim_base->Instance==TIM10)
    {
        /* Peripheral clock disable */
        __TIM10_CLK_DISABLE();

        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
    }
    else if(htim_base->Instance==TIM5)
    {
        /* Peripheral clock disable */
        __TIM5_CLK_DISABLE();

        /**TIM5 GPIO Configuration
        PA0-WKUP------> TIM5_CH1
        PA1     ------> TIM5_CH2
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);
        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(TIM5_IRQn);
    }
    else if(htim_base->Instance==TIM3)
    {
        /* USER CODE BEGIN TIM3_MspDeInit 0 */

        /* USER CODE END TIM3_MspDeInit 0 */
        /* Peripheral clock disable */
        __TIM3_CLK_DISABLE();

        /**TIM3 GPIO Configuration
        PB0     ------> TIM3_CH3
        PB1     ------> TIM3_CH4
        PB4     ------> TIM3_CH1
        PB5     ------> TIM3_CH2
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);

        /* USER CODE BEGIN TIM3_MspDeInit 1 */

        /* USER CODE END TIM3_MspDeInit 1 */
    }
    //We are not supporting parallel Input to uC, so we don't need this Inputs
//    else if(htim_base->Instance==TIM4)
//    {
//        /* Peripheral clock disable */
//        __TIM4_CLK_DISABLE();
//
//        /**TIM4 GPIO Configuration
//        PB6     ------> TIM4_CH1
//        PB7     ------> TIM4_CH2
//        PB8     ------> TIM4_CH3
//        PB9     ------> TIM4_CH4
//        */
//        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9);
//        /* Peripheral interrupt Deinit*/
//        HAL_NVIC_DisableIRQ(TIM4_IRQn);
//    }
}

void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* htim_ic)
{

    if(htim_ic->Instance==TIM5)
    {
        /* Peripheral clock disable */
        __TIM5_CLK_DISABLE();
        /**TIM5 GPIO Configuration
        PA0-WKUP------> TIM5_CH1
        PA1     ------> TIM5_CH2
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);
        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(TIM5_IRQn);
    }
      //We are not supporting parallel Input to uC, so we don't need this Inputs
//    else if(htim_ic->Instance==TIM4)
//    {
//        /* Peripheral clock disable */
//        __TIM4_CLK_DISABLE();
//
//        /**TIM4 GPIO Configuration
//        PB6     ------> TIM4_CH1
//        PB7     ------> TIM4_CH2
//        PB8     ------> TIM4_CH3
//        PB9     ------> TIM4_CH4
//        */
//        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9);
//        /* Peripheral interrupt Deinit*/
//        HAL_NVIC_DisableIRQ(TIM4_IRQn);
//    }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
    if(htim_pwm->Instance==TIM3)
    {
        /* USER CODE BEGIN TIM3_MspDeInit 0 */

        /* USER CODE END TIM3_MspDeInit 0 */
        /* Peripheral clock disable */
        __TIM3_CLK_DISABLE();

        /**TIM3 GPIO Configuration
        PB0     ------> TIM3_CH3
        PB1     ------> TIM3_CH4
        PB4     ------> TIM3_CH1
        PB5     ------> TIM3_CH2
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);

        /* USER CODE BEGIN TIM3_MspDeInit 1 */

        /* USER CODE END TIM3_MspDeInit 1 */
    }
}


/* USER CODE BEGIN 1 */


void start_ic(void)
{
    TIM_IC_InitTypeDef sConfigIC;

    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;

#if (USE_PPM_INPUT == 0)
      //We are not supporting parallel Input to uC, so we don't need this Inputs
//NOTE: When using parallel input we need to measure Ton of each channel, so we need to look at Rising and Falling edge
//    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
    //TODO: Enable as many as NUM_INPUTS Channels
//	if (NUM_INPUTS > 0)  {HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_1); HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1);};
//	if (NUM_INPUTS > 1)  {HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2); HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2);};
#elif (USE_PPM_INPUT == 1)   //We are using PPM SIGNAL type 1
//NOTE: When I use PPM type 1 I have to get time between each rising edge
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    //TODO: Transform PPM_IC to TIM_CHANNELx: (4*PPM_IC)) -> PPM_IC=0 -> IC CH1, PPM_IC=1 -> IC CH2 -> already tested
    //if (NUM_INPUTS > 0)  {HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, 4*PPM_IC); HAL_TIM_IC_Start_IT(&htim5, 4*PPM_IC);};
//NOTE: We only support for PPM on CHANNEL 2
    if (NUM_INPUTS > 0)  {HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2); HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2);};
#else                       //We are using PPM SIGNAL typo 2
    // We need to compute time between Rising and falling edge
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
    if (NUM_INPUTS > 0)  {HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2); HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2);};
#endif // USE_PPM_INPUT
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
