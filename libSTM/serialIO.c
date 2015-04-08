// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#include "../libUDB/libUDB.h"
//#include "../libUDB/oscillator.h"
//#include "../libUDB/interrupt.h"
#include "../libUDB/serialIO.h"
#include "stm32f4xx_hal.h"


////////////////////////////////////////////////////////////////////////////////
//
// GPS

extern UART_HandleTypeDef huart6;

int32_t gps_baud_rate = 0;
void udb_gps_set_rate(int32_t rate) { gps_baud_rate = rate; }
boolean udb_gps_check_rate(int32_t rate) { return (gps_baud_rate == rate); }

void udb_init_GPS(void)
{
//	HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
    /* Process Locked */
    __HAL_LOCK(&huart6);

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_ERR);

    /* Process Unlocked */
    __HAL_UNLOCK(&huart6);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
}

//#include "../MatrixPilot/defines.h" // TODO: remove, temporarily here for options to work correctly
#include "../libDCM/libDCM.h"
//#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"

void MP_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	gps_out_buffer_reset();
}

void udb_gps_start_sending_data(void)
{
	uint8_t *pData = gps_out_buffer_get();
	uint16_t Size = gps_out_buffer_length_get();
//HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
	if (pData != 0 && Size != 0)
	{
		HAL_UART_Transmit_IT(&huart6, pData, Size);
	}
#if 0
    /* Process Locked */
    __HAL_LOCK(&huart6);

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_ERR);

    /* Process Unlocked */
    __HAL_UNLOCK(&huart6);

    /* Enable the UART Transmit data register empty Interrupt */
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_TXE);
#endif
}

static char Test_Data[] = "this is test data\r\n";
#include <string.h>

void Test_HAL_UART_Transmit_IT(void)
{
	int i;

//	huart6->State = HAL_UART_STATE_READY;
	for (i = 0; i < 10; i++)
	{
//HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
		HAL_UART_Transmit_IT(&huart6, Test_Data, strlen(Test_Data));
		HAL_Delay(50);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void MP_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // we want to signal the process responsible for this uart
}

//void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
void MP_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

        // Disable the UART Parity Error Interrupt
        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

        // Disable the UART Error Interrupt: (Frame error, noise error, overrun error)
        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

        huart->State = HAL_UART_STATE_READY;

      /* Disable the UART Transmit Complete Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

      /* Enable the UART Transmit Complete Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_TC);



	printf("MP_UART_ErrorCallback code: %u\r\n", huart->ErrorCode);
	huart->ErrorCode = HAL_UART_ERROR_NONE;
}

static HAL_StatusTypeDef MP_UART_Receive_IT(UART_HandleTypeDef *huart)
{
//  uint16_t* tmp;
//  uint32_t tmp1 = 0;

//  tmp1 = huart->State;
//  if((tmp1 == HAL_UART_STATE_BUSY_RX) || (tmp1 == HAL_UART_STATE_BUSY_TX_RX))
  if (1)
  {
		uint8_t rxchar = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
		udb_gps_callback_received_byte(rxchar);
/*
    if(huart->Init.WordLength == UART_WORDLENGTH_9B)
    {
      tmp = (uint16_t*) huart->pRxBuffPtr;
      if(huart->Init.Parity == UART_PARITY_NONE)
      {
        *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
        huart->pRxBuffPtr += 2;
      }
      else
      {
        *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
        huart->pRxBuffPtr += 1;
      }
    }
    else
    {
      if(huart->Init.Parity == UART_PARITY_NONE)
      {
        *huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
      }
      else
      {
        *huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
      }
    }

    if(--huart->RxXferCount == 0)
    {
      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

      // Check if a transmit process is ongoing or not
      if(huart->State == HAL_UART_STATE_BUSY_TX_RX)
      {
        huart->State = HAL_UART_STATE_BUSY_TX;
      }
      else
      {
        // Disable the UART Parity Error Interrupt
        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

        // Disable the UART Error Interrupt: (Frame error, noise error, overrun error)
        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

        huart->State = HAL_UART_STATE_READY;
      }
      MP_UART_RxCpltCallback(huart);

      return HAL_OK;
    }
  */
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

static HAL_StatusTypeDef MP_UART_Transmit_IT(UART_HandleTypeDef *huart)
{
  uint16_t* tmp;
  uint32_t tmp1 = 0;

  tmp1 = huart->State;
  if((tmp1 == HAL_UART_STATE_BUSY_TX) || (tmp1 == HAL_UART_STATE_BUSY_TX_RX))
  {
    if(huart->Init.WordLength == UART_WORDLENGTH_9B)
    {
      tmp = (uint16_t*) huart->pTxBuffPtr;
      huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      if(huart->Init.Parity == UART_PARITY_NONE)
      {
        huart->pTxBuffPtr += 2;
      }
      else
      {
        huart->pTxBuffPtr += 1;
      }
    }
    else
    {
      huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
    }

    if(--huart->TxXferCount == 0)
    {
      /* Disable the UART Transmit Complete Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

      /* Enable the UART Transmit Complete Interrupt */
      __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

static HAL_StatusTypeDef MP_UART_EndTransmit_IT(UART_HandleTypeDef *huart)
{
  /* Disable the UART Transmit Complete Interrupt */
  __HAL_UART_DISABLE_IT(huart, UART_IT_TC);

  /* Check if a receive process is ongoing or not */
  if(huart->State == HAL_UART_STATE_BUSY_TX_RX)
  {
    huart->State = HAL_UART_STATE_BUSY_RX;
  }
  else
  {
    /* Disable the UART Parity Error Interrupt */
    __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

    /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

    huart->State = HAL_UART_STATE_READY;
  }

  MP_UART_TxCpltCallback(huart);
//  HAL_UART_TxCpltCallback(huart);

  return HAL_OK;
}

void MP_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t tmp1 = 0, tmp2 = 0;

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_UART_CLEAR_PEFLAG(huart);

    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_UART_CLEAR_FEFLAG(huart);

    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_UART_CLEAR_NEFLAG(huart);

    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_UART_CLEAR_OREFLAG(huart);

    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    MP_UART_Receive_IT(huart);
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    MP_UART_Transmit_IT(huart);
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
  /* UART in mode Transmitter end --------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    MP_UART_EndTransmit_IT(huart);
  }

  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Set the UART state ready to be able to start again the process */
    huart->State = HAL_UART_STATE_READY;

    MP_UART_ErrorCallback(huart);
  }
}


//void USART2_IRQHandler(void)

/*
void __attribute__((__interrupt__,__no_auto_psv__)) _U1ErrInterrupt(void)
{
	_U1EIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

	U1STAbits.PERR = 0;
	U1STAbits.FERR = 0;
	U1STAbits.OERR = 0;
	interrupt_restore_corcon;
}
 */
/*
void __attribute__((__interrupt__,__no_auto_psv__)) _U1TXInterrupt(void)
{
	_U1TXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

#if (HILSIM_USB != 1)
	int16_t txchar = udb_gps_callback_get_byte_to_send();
#else
	int16_t txchar = -1;
#endif // HILSIM_USB
	if (txchar != -1)
	{
		U1TXREG = (uint8_t)txchar;
	}
	interrupt_restore_corcon;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(void)
{
	_U1RXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

	while (U1STAbits.URXDA)
	{
		uint8_t rxchar = U1RXREG;
#if (HILSIM_USB != 1)
		udb_gps_callback_received_byte(rxchar);
#endif // HILSIM_USB
	}
	U1STAbits.OERR = 0;
	interrupt_restore_corcon;
}
 */

/////////////////////////////////////////////////////////////////////////////////////////
// Serial

int32_t serial_baud_rate = 0;

void udb_serial_start_sending_data(void) {}
void udb_serial_set_rate(int32_t rate) { serial_baud_rate = rate; }

void udb_init_USART(void)
{
}

boolean udb_serial_check_rate(int32_t rate)
{
//	return (U2BRG == UDB_BAUD(rate));
	return true;
}

/*
void __attribute__((__interrupt__, __no_auto_psv__)) _U2TXInterrupt(void)
{
	_U2TXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

	int16_t txchar = udb_serial_callback_get_byte_to_send();
	if (txchar != -1)
	{
		U2TXREG = (uint8_t)txchar;
		uart2_tx_count++;
	}
	interrupt_restore_corcon;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U2RXInterrupt(void)
{
	_U2RXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

	while (U2STAbits.URXDA)
	{
		uint8_t rxchar = U2RXREG;
		udb_serial_callback_received_byte(rxchar);
		uart2_rx_count++;
	}
	U2STAbits.OERR = 0;
	interrupt_restore_corcon;
}
 */



#if 0
To that I added to the file "stm32f2xx_hal.с"
/ *********************************************** /
 __weak void HAL_UART_REC_BYTE(UART_HandleTypeDef *huart, uint16_t byte)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_REC_BYTE could be implemented in the user file
   */
}
void HAL_UART_IRQHandler_ALWAYS_RECIEVE(UART_HandleTypeDef *huart){
  uint32_t tmp1 = 0, tmp2 = 0;


  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))  {
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))  {
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);

    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);

    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }

  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if(tmp2 != RESET)  {
uint16_t tmp;
//uint8_t byte;
    if(huart->Init.WordLength == UART_WORDLENGTH_9B)    {
      //tmp = (uint16_t*) huart->pRxBuffPtr;
      if(huart->Init.Parity == UART_PARITY_NONE)      {
        tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
        //huart->pRxBuffPtr += 2;
      } else {
        tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
        //huart->pRxBuffPtr += 1;
      }
    } else {
      if(huart->Init.Parity == UART_PARITY_NONE)      {
        tmp = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
      } else {
        tmp = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
      }
    }


    HAL_UART_REC_BYTE(huart, tmp);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
  }

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    UART_Transmit_IT(huart);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);
  }

  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Set the UART state ready to be able to start again the process */
    huart->State = HAL_UART_STATE_READY;

    HAL_UART_ErrorCallback(huart);
  }
}
/ ************************************************* /

The file "stm32f2xx h.с" replaced the interrupt function(sample)
/*****************************************************/
void UART5_IRQHandler(void){
  HAL_NVIC_ClearPendingIRQ(UART5_IRQn);
  //HAL_UART_IRQHandler(&huart5);
HAL_UART_IRQHandler_ALWAYS_RECIEVE(&huart5);
}
/*****************************************************/

and made the implementation of the function:
(here it is necessary to insert the algorithm receiving bytes)
/************************************/
void HAL_UART_REC_BYTE(UART_HandleTypeDef *huart, uint16_t byte)
{
switch((uint32_t)huart->Instance){
case (uint32_t)USART1:
break;
case (uint32_t)USART2:
break;
case (uint32_t)USART3:
break;
case (uint32_t)UART4:
break;
case (uint32_t)UART5:
break;
case (uint32_t)USART6:
break;
}
}
/************************************/
I am sorry for bad english
#endif // 0
