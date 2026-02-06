/*
 * data.c
 *
 *  Created on: Feb 5, 2026
 *      Author: admin
 */

#include "data.h"


void set_lux(uint16_t x ) {
  data_lux=x ;
}
void set_mois(uint16_t y) {
  data_mois=y ;
}

uint16_t get_lux(void) {
 return data_lux ;
}

uint16_t get_mois(void) {
 return data_mois ;
}
