/*
 * API_debounce.h
 *
 *  Created on: Mar 28, 2025
 *      Author: Eze Arce
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include <stdbool.h>
#include <stdint.h>
#include "API_delay.h"

void debounceFSM_init();		// debe cargar el estado inicial
void debounceFSM_update();	// debe leer las entradas, resolver la lógica de

bool_t readKey();
bool_t readPressing();
bool_t readPushed();


#endif /* API_INC_API_DEBOUNCE_H_ */
