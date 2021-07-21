
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_my_ADC_H
#define	XC_my_ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#define _XTAL_FREQ 4000000

void start_ADC(uint8_t channel);


#endif	/* XC_HEADER_TEMPLATE_H */

