/*
 * TF-LC02.h
 *
 *  Created on: Apr 2, 2025
 *      Author: Eze
 */

#ifndef API_INC_TF_LC02_H_
#define API_INC_TF_LC02_H_

#include  "TF-LC02_Port.h"
#include <stdbool.h>
#include <string.h>

void TFLC02_Init(void);
void TFLC02_Start(void);
void TFLC02_Mesure(void);
void TFLC02_Info(void);
void TFLC02_DefaultSettings(void);
bool TFLC02_RspComplete(void);
uint16_t TFLC02_GetDistance(void);
uint8_t TFLC02_GetPort(void);
uint8_t TFLC02_GetFirm(void);
uint8_t TFLC02_GetCalib(void);


#endif /* API_INC_TF_LC02_H_ */
