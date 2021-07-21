
#include "my_ADC.h"

void start_ADC(uint8_t channel){
    
//habilitar la conversión
    if(ADCON0bits.GO == 0){
        ADCON0bits.CHS=channel;
        __delay_us(50);
        ADCON0bits.GO = 1;
    }

}

