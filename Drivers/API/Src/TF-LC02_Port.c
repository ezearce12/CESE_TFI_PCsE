/*
 * TF-LC02_Port.c
 *
 *  Created on: Apr 2, 2025
 *      Author: Eze Arce
 */


#include "../../Drivers/API/Inc/TF-LC02.h"

extern UART_HandleTypeDef huart4;

void TFLC02_Error_Handler(void);


void TFLC02_Port_Transmit(const uint8_t *pData, uint16_t Size, uint32_t Timeout){

	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart4, pData, Size, Timeout);

	if(status != HAL_OK){
		TFLC02_Error_Handler();
	}

}

void TFLC02_Port_Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout){

	HAL_StatusTypeDef status = HAL_UART_Receive(&huart4, pData, Size, Timeout);

	if(status != HAL_OK){
		TFLC02_Error_Handler();
	}

}

void TFLC02_Port_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size){

	HAL_StatusTypeDef status = HAL_UART_Transmit_IT(&huart4, pData, Size);

	if(status != HAL_OK){
		TFLC02_Error_Handler();
	}

}

void TFLC02_Port_Receive_IT(uint8_t *pData, uint16_t Size){

	HAL_StatusTypeDef status = HAL_UART_Receive_IT(&huart4, pData, Size);

	if(status != HAL_OK){
		TFLC02_Error_Handler();
	}

}

void TFLC02_Error_Handler(void){

	//Se apaga el led de la placa para alertar el error en el sensor
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	//While infinito
	while(1){

	}

}





















