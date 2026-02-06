/*
 * data.h
 *
 *  Created on: Feb 5, 2026
 *      Author: admin
 */
#include "main.h"
#ifndef INC_DATA_H_
#define INC_DATA_H_

static uint16_t data_lux=0 ;

static uint16_t data_mois=0 ;


void set_lux(uint16_t x ) ;
void set_mois(uint16_t y) ;

uint16_t get_lux(void) ;
uint16_t get_mois(void) ;

#endif /* INC_DATA_H_ */
