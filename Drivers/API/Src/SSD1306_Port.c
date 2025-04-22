/*
 * SSD1306_Port.c
 *
 *  Created on: Apr 19, 2025
 *      Author: Eze
 */

#include "../../Drivers/API/Inc/SSD1306_Port.h"

extern I2C_HandleTypeDef hi2c1;

#define SSD1306_I2C_ADDR         0x78 // Dirección I2C (0x3C << 1)

void SSD1306_Error_Handler(void);

//Wrraper de la transmision I2C
void SSD1306_I2C_Transmit(uint8_t *pData, uint16_t Size){

	HAL_StatusTypeDef err = HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, pData, Size,  HAL_MAX_DELAY);

	if(err != HAL_OK){
		SSD1306_Error_Handler();
	}

}


void SSD1306_Error_Handler(void){

	//Se enciende el led de la placa para alertar un error
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

	//While infinito
	while(1){

	}

}
