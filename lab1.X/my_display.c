
#include "my_display.h"

//tabla para el display 7 segmentos
uint8_t tabla[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x77,0x7C,0x39,0x5E,0x79,0x71};

uint8_t display_1 = 0;



// funcion contador
uint8_t display_hex(uint8_t var_temp){
        

    display_1 = tabla[var_temp];

    
    return display_1;

}
