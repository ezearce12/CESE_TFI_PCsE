/*
 * SSD1306.c
 *
 *  Created on: Apr 15, 2025
 *      Author: Eze
 */

#include "../../Drivers/API/Inc/SSD1306.h"
#include "../../Drivers/API/Inc/SSD1306_Port.h"
#include "../../Drivers/API/Inc/font.h"
#include <stdlib.h>
#include <stdio.h>

void SSD1306_SendCommand(uint8_t command);
void SSD1306_SendData(uint8_t* data, size_t size);
void SSD1306_SetCursor(uint8_t x, uint8_t page);

//Envia comando al display
//Recibe el comando a enviar
void SSD1306_SendCommand(uint8_t command) {
    uint8_t data[2];
    data[0] = SSD1306_COMMAND;
    data[1] = command;

    SSD1306_I2C_Transmit(data, 2);

}

//Envio de datos al display
//Recibe direccion de memoria de los datos a enviar y el tamaño a enviar.
void SSD1306_SendData(uint8_t* data, size_t size) {

	static uint8_t buffer[129];
	if (size + 1 > sizeof(buffer)) return;

	memset(buffer, 0, sizeof(buffer));
    buffer[0] = SSD1306_DATA;
    memcpy(&buffer[1], data, size);

    SSD1306_I2C_Transmit(buffer, size + 1);

}

//Inicializa el display
//Inicializa el puerto i2c y realiza la secuencia de inico del display
void SSD1306_Init(void) {

    HAL_Delay(100);

    SSD1306_SendCommand(0xAE); // Display OFF
    SSD1306_SendCommand(0xD5);
    SSD1306_SendCommand(0x80);

    SSD1306_SendCommand(0xA8);
    SSD1306_SendCommand(0x3F);

    SSD1306_SendCommand(0xD3);
    SSD1306_SendCommand(0x00);

    SSD1306_SendCommand(0x40);

    SSD1306_SendCommand(0x8D);
    SSD1306_SendCommand(0x14);

    SSD1306_SendCommand(0x20);
    SSD1306_SendCommand(0x00);

    SSD1306_SendCommand(0xA1);
    SSD1306_SendCommand(0xC8);

    SSD1306_SendCommand(0xDA);
    SSD1306_SendCommand(0x12);
    SSD1306_SendCommand(0x81);
    SSD1306_SendCommand(0x7F);

    SSD1306_SendCommand(0xD9);
    SSD1306_SendCommand(0xF1);

    SSD1306_SendCommand(0xDB);
    SSD1306_SendCommand(0x40);
    SSD1306_SendCommand(0xA4);
    SSD1306_SendCommand(0xA6);
    SSD1306_SendCommand(0xAF);

}

//Limpia todo el display
void SSD1306_Clear(void) {
    uint8_t zero_buffer[SSD1306_WIDTH];
    memset(zero_buffer, 0x00, sizeof(zero_buffer));

    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_SendCommand(0xB0 + page);       // Page start
        SSD1306_SendCommand(0x00);              // Lower column start
        SSD1306_SendCommand(0x10);              // Higher column start
        SSD1306_SendData(zero_buffer, SSD1306_WIDTH);
    }
}

//Enciende el display
void SSD1306_DisplayOn(void) {
    SSD1306_SendCommand(0xAF);
}

//Apaga el display
void SSD1306_DisplayOff(void) {
    SSD1306_SendCommand(0xAE);
}

//Configra el cursor en el mapa de bits del display
//Recibe el indice de caracter 0 - 20.
//Recibe la pagina del display 0 - 7.
void SSD1306_SetCursor(uint8_t x, uint8_t page) {

	static uint8_t cursor5x7;


	cursor5x7 = x * 6;										//Se redimensiona el cursor por la fuente que se usa 5x7 + espacio entre caracteres.

    SSD1306_SendCommand(0xB0 + page);               		// Configura la pagina
    SSD1306_SendCommand(0x00 + (cursor5x7 & 0x0F));         // bit inferior
    SSD1306_SendCommand(0x10 + ((cursor5x7 >> 4) & 0x0F));  // bit superor

}

//Envia un caracter mas el espacio al display
//Recibe una caracter
void SSD1306_WriteChar(char c) {
	static uint8_t space = 0x00;

	if (c < 32 || c > 126) c = '?'; // Caracteres no soportados

    SSD1306_SendData((uint8_t*)Font5x7[c - 32], 5); // Enviar 5 columnas
    SSD1306_SendData(&space, 1); 					// Espacio entre caracteres
}


//Envia un string al display
//Recibe la direccion de memopria del string a enviar
void SSD1306_WriteString(char* str) {

	while (*str) {
        SSD1306_WriteChar(*str++);
    }

}

//Imprime mediciones en el display
//Recibe la medicion actual, maxima y minima
void SSD1306_PrintMesurement(uint16_t Actual,uint16_t Maxima,uint16_t Minima){

	static char buffer[22];

	sprintf(buffer, "%2d.%d[cm]", Actual / 10, Actual % 10);
	SSD1306_SetCursor(10, 0);
	SSD1306_WriteString(buffer);

	sprintf(buffer, "%2d.%d[cm]", Maxima / 10, Maxima % 10);
	SSD1306_SetCursor(7, 1);
	SSD1306_WriteString(buffer);

	sprintf(buffer, "%2d.%d[cm]", Minima / 10, Minima % 10);
	SSD1306_SetCursor(7, 2);
	SSD1306_WriteString(buffer);

}


//Imprime el muestreo en el display
//recibe el muestreo a imprimr
void SSD1306_PrintMuestreo(uint32_t muestreo){

	static char buffer[22];

	sprintf(buffer, "%lu[ms]", muestreo);
	SSD1306_SetCursor(9, 3);
	SSD1306_WriteString(buffer);
}


//Imprime las etiquetas estaticas en el display
void SSD1306_PrintSetup(uint8_t port,uint8_t cal){

	static char buffer[22];

	sprintf(buffer, "Distancia:");
	SSD1306_SetCursor(0, 0);
	SSD1306_WriteString(buffer);

	sprintf(buffer, "Maxima:");
	SSD1306_SetCursor(0, 1);
	SSD1306_WriteString(buffer);

	sprintf(buffer, "Minima:");
	SSD1306_SetCursor(0, 2);
	SSD1306_WriteString(buffer);

	sprintf(buffer, "Muestreo:");
	SSD1306_SetCursor(0, 3);
	SSD1306_WriteString(buffer);

	sprintf(buffer, "%s",
			(cal == 0x00) ? "Sin calibrar" :
            (cal == 0x01) ? "Crosstalk calibrado" :
            (cal == 0x02) ? "Offset calibrado" :
            (cal == 0x03) ? "Calibracion completa" : "Desconocida");
	SSD1306_SetCursor(0, 5);
	SSD1306_WriteString(buffer);


	sprintf(buffer, "%s",
			(port == 0x41) ? "UART+I2C" :
            (port == 0x49) ? "Solo I2C" :
            (port == 0x55) ? "Solo UART" : "Desconocido");

	SSD1306_SetCursor(0, 6);
	SSD1306_WriteString(buffer);

}
