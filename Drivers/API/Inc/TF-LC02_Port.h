/*
 * TF-LC02_Port.h
 *
 *  Created on: Apr 2, 2025
 *      Author: Eze
 */

#ifndef API_INC_TF_LC02_PORT_H_
#define API_INC_TF_LC02_PORT_H_

#include "stm32f4xx_hal.h"
#include "main.h"

void TFLC02_Port_Init(void);
void TFLC02_Port_Transmit(const uint8_t *pData, uint16_t Size, uint32_t Timeout);
void TFLC02_Port_Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout);
void TFLC02_Port_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
void TFLC02_Port_Receive_IT(uint8_t *pData, uint16_t Size);

#endif /* API_INC_TF_LC02_PORT_H_ */
