/*
 * TF-LC02.c
 *
 *  Created on: Apr 2, 2025
 *      Author: Eze Arce
 */


#include "main.h"
#include "../../Drivers/API/Inc/TF-LC02.h"
#include "../../Drivers/API/Inc/TF-LC02_Port.h"
#include "../../Drivers/API/Inc/API_delay.h"



#define LIDAR_FRAME_LEN 32
#define LIDAR_FRAME_MIN 5
#define LIDAR_FRAME_HEADER1 0x55
#define LIDAR_FRAME_HEADER2 0xAA
#define LIDAR_FRAME_END 	0xFA

//Enumeracion de estados para la recepcion de datos
typedef enum {
    STATE_WAIT_HEADER_1,
    STATE_WAIT_HEADER_2,
    STATE_CMD,
    STATE_LEN,
    STATE_DATA,
    STATE_END
} ParserState;

//Enumeracion de comandos del sensor
typedef enum{
	Measure = 0x81,					//Medicion de distancia en [mm].
	Crosstalk_correction,			//Correccion de Crosstalk.
	Offset_correction,				//Correccion de Offset.
	TFLC02_Reset,					//Reset del sensor.
	Get_Factory_default_settings,	//Obtencion de datos de configuracion por defecto.
	Get_Prod_info,					//Obtencion de la informacion del producto.
}TF_CMD;

//Enumeracion de errores del sensor
typedef enum{
	VALID_DATA,						//Valor valido.
	VCSEL_SHORT,					//VCSEL esta corto circuitado.
	LOW_SIGNAL,						//La cantidad de luz reflejada obtenida del objeto detectado es pequeña.
	LOW_SN = 0x04,					//La relación entre la luz reflejada del objeto detectado y la luz perturbadora es pequeña.
	TOO_MUCH_AMB = 0x08,			//La luz perturbadora es grande.
	WAF = 0x10,						//Error wrapping.
	CAL_ERROR = 0x20,				//Error de calculo interno.
	CROSSTALK_ERROR = 0x80,			//Error de crosstalk.
}TF_ERROR;

// Estructura de datos para almacenar la información del sensor.
typedef struct {
    uint16_t distance;
    uint8_t errorCode;
    bool receiveComplete;
    uint8_t calib;
    uint8_t port;
    uint8_t firmware;
    uint8_t type;
    uint8_t offset_short1;
    uint8_t offset_short2;
    uint8_t offset_long1;
    uint8_t offset_long2;
    uint16_t crosstalk;
} TF_t;

uint8_t rx_byte;						// Byte de recepcion.
uint8_t rx_buffer[LIDAR_FRAME_LEN];		// Buffer para la recepcion de datos.
uint8_t rx_index = 0;					// Indice del buffer.

ParserState parser_state = STATE_WAIT_HEADER_1;	// Inicializacion de estado para recepcion de datos.
uint8_t expected_length = 0;					// Inicializacion de longitud esperada.

volatile TF_t lidar = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool TFLC02_Parse_Packet(uint8_t *buffer, uint8_t size);

void TFLC02_Send_Command(uint8_t cmd);

//Inicializa el puerto UART.
void TFLC02_Init(void){
	TFLC02_Port_Init();
}

//Inicia la recepcion UART.
void TFLC02_Start(void){
	TFLC02_Port_Receive_IT(&rx_byte, 1);
}

//Se mide distancia
void TFLC02_Mesure(void){
	TFLC02_Send_Command(Measure);
}

//Obtiene informacion del sensor
void TFLC02_Info(void){
	TFLC02_Send_Command(Get_Prod_info);
}

//Obtiene los valores de fabrica del sensor
void TFLC02_DefaultSettings(void){
	TFLC02_Send_Command(Get_Factory_default_settings);
}

//Se revisa si la respuesta del sensor esta completa.
//Devuelve True o False dependiendo si la respuesta del sensor esta completa.
bool TFLC02_RspComplete(void){

	if(lidar.receiveComplete){
		lidar.receiveComplete = false;
		return true;
	}
	else{
		return false;
	}

}

//Se obtiene la ultima distancia sensada.
uint16_t TFLC02_GetDistance(void) {
    return lidar.distance;
}

//Se obtiene el tipo de puerto que utiliza el sensor.
uint8_t TFLC02_GetPort(void) {
    return lidar.port;
}

//Se obtiene la version de firmware del sensor.
uint8_t TFLC02_GetFirm(void) {
    return lidar.firmware;
}

//Se obtiene la calibracion del sensor.
uint8_t TFLC02_GetCalib(void) {
    return lidar.calib;
}

// Callback de recepción
// Se implementa una máquina de estados para decodificar paquetes de datos del sensor.
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == UART4) {		//Se verifica el puerto de la interrupcion.

        switch (parser_state) {

            case STATE_WAIT_HEADER_1:
                if (rx_byte == LIDAR_FRAME_HEADER1) {
                    rx_buffer[0] = rx_byte;
                    rx_index = 1;
                    parser_state = STATE_WAIT_HEADER_2;
                }
                break;

            case STATE_WAIT_HEADER_2:
                if (rx_byte == LIDAR_FRAME_HEADER2) {
                    rx_buffer[1] = rx_byte;
                    rx_index = 2;
                    parser_state = STATE_CMD;
                } else {
                	parser_state = STATE_WAIT_HEADER_1;
                }
                break;

            case STATE_CMD:
            	rx_buffer[rx_index++] = rx_byte;
            	parser_state = STATE_LEN;
            	break;

            case STATE_LEN:
            	rx_buffer[rx_index++] = rx_byte;
            	expected_length = rx_byte;						//Se almacena la longitud esperada.
            	if((expected_length + 5) <= LIDAR_FRAME_LEN){	//Se verifica que la longitud esperada del paquete no exceda el maximo permitido.
            		parser_state = (expected_length > 0) ? STATE_DATA : STATE_END;	//Se verifica si hay que esperar datos o no.
            	}
            	else{
            		parser_state = STATE_WAIT_HEADER_1;
            	}

            	break;

            case STATE_DATA:
                rx_buffer[rx_index++] = rx_byte;
                if (rx_index == (4 + expected_length)) {		//Se verifica si se recibieron todos datos.
                    parser_state = STATE_END;
                }
                break;

            case STATE_END:
                rx_buffer[rx_index++] = rx_byte;
                if (rx_byte == LIDAR_FRAME_END) {		//Se verifica el ultimo byte.

                	if(TFLC02_Parse_Packet(rx_buffer,sizeof(rx_buffer))){	//Se procesa el paquete.
                		lidar.receiveComplete = true;
                	}
                }

                //Se reinicia la maquina de estados.
                parser_state = STATE_WAIT_HEADER_1;
                rx_index = 0;

                break;

            default:
                parser_state = STATE_WAIT_HEADER_1;
                rx_index = 0;
                break;
        }

        TFLC02_Start();		//Se reinicia la recepcion UART.
    }
}

//Se procesa el paquete recibido
//Recibe puntero al buffer que contiene los datos recibidos
//Recibe size: Tamaño del buffer
//Devuelve 1 si el paquete es válido y se procesó correctamente, 0 en caso contrario
bool TFLC02_Parse_Packet(uint8_t *buffer, uint8_t size) {

//    Se extraen comando y longitud
    uint8_t cmd = buffer[2];

    // Interpretar campos según comando
    switch(cmd) {
        case Measure:  // Medición de distancia

				lidar.distance = (rx_buffer[4] << 8) | rx_buffer[5];
				lidar.errorCode = rx_buffer[6];

            break;

        case TFLC02_Reset:  // Reset
            // No hay payload
            break;

        case Get_Factory_default_settings:

				lidar.offset_short1 = rx_buffer[4];
				lidar.offset_short2 = rx_buffer[5];
				lidar.offset_long1 = rx_buffer[6];
				lidar.offset_long2 =rx_buffer[7];
				lidar.crosstalk = (rx_buffer[8] << 8) | rx_buffer[9];
				lidar.calib = rx_buffer[10];

            break;

        case Get_Prod_info:

				lidar.type = rx_buffer[4];
				lidar.port = rx_buffer[5];
				lidar.firmware = rx_buffer[6];

            break;

        default:
        	//Comando no soportado.
        	return false;

        	break;
    }

    return true;
}

//Se envia un comando al sensor
void TFLC02_Send_Command(uint8_t cmd) {
    uint8_t frame[5] = {0x55, 0xAA, cmd, 0x00, 0xFA};
    TFLC02_Port_Transmit(frame, sizeof(frame), HAL_MAX_DELAY);
}
