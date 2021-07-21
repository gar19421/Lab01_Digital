

/* 
 * File:   lab1_main.c
 * Author: Branodon Garrido
 *
 * Created on 19 de julio de 2021, 03:28 PM
 */


// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT//Oscillator Selection bits(INTOSC oscillator: 
                                  //CLKOUT function on RA6/OSC2/CLKOUT pin, 
                                  //I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled and 
                          //can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR  
                                //pin function is digital input, MCLR internally 
                                //tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code 
                                //protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code 
                                //protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit 
                                //Internal/External Switchover mode is disabled
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit 
                                //(Fail-Safe Clock Monitor is disabled)
#pragma config LVP = ON         //Low Voltage Programming Enable bit(RB3/PGM pin 
                                //has PGM function, low voltage programming 
                                //enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out 
                                //Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits 
                                //(Write protection off)


// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "my_ADC.h"
#include "my_display.h"


#define _XTAL_FREQ 4000000
/*
 * Declaraciones y variables
 */

//tabla para el display 7 segmentos
char tabla[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x77,0x7C,0x39,0x5E,0x79,0x71};
//uint8_t
//variables temporales en multiplexado
uint8_t var_temp;
uint8_t decenas_temp = 0;

//variables de conversion Hex to Dec
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t centenas = 0;


uint8_t dec_display = 0;
uint8_t cen_display = 0;

uint8_t nibble_inferior;
uint8_t nibble_superior;

char bandera_tit=0;
char bandera;
uint8_t bandera_disp=0;

// Prototipos
void setup();
void config_reloj();
void config_io();
void config_int_enable();
void config_timer0();
void TMR0_interrupt();
void contador();
void IOCB_interrupt();

/*
 *  main
 */

void main(void) {
    
    setup(); // llamar función de configuraciones
    ADCON0bits.GO = 1;
    
    while(1){  //loop principal
   
        if(var_temp>PORTC){
            bandera_tit = 1;
            PORTAbits.RA6 = 1;
            
        }else{
            bandera_tit = 0;
            PORTAbits.RA6 = 0;
        }
        
        //funcion para habilitar obtencion de valores del ADC
        start_ADC(0x00);
        
        //
        contador();
        
    }
    
    return;
}

void setup(){
    
    config_reloj();// configuraciones principales del programa
    config_io();
    config_int_enable();
    config_timer0();
    
    return;
};


// funcion contador
void contador(void){
        
    nibble_superior = var_temp & 0xF0; // se divide dentro de la base 
    nibble_superior = nibble_superior >> 4;
    nibble_inferior = var_temp & 0x0F; // y luego se obtiene el modulo para ver 
    
    
    dec_display = display_hex(nibble_inferior);
    cen_display = display_hex(nibble_superior);
}


// interrupciones

void __interrupt() isr(void){

    if (INTCONbits.T0IF){//se verifica que bandera de interrupcion se levanto
        TMR0_interrupt();
    }
    if (PIR1bits.ADIF){//se verifica que bandera de interrupcion se levanto
        
    
        var_temp = ADRESH;
        
        PIR1bits.ADIF = 0;
    }
    if(INTCONbits.RBIF){
        IOCB_interrupt();
    }
         
}


void TMR0_interrupt(){
    
    PORTE = 0x00; // se limpia el puerto de multiplexado
    
    //if(bandera_tit){
    //    __delay_ms(30);
    //}
    
    if(bandera_disp == 0){
        PORTD = dec_display;
        PORTEbits.RE1 = 1;
        bandera_disp++;        
    }else {
        PORTD = cen_display;
        PORTEbits.RE2 = 1;
        bandera_disp = 0;
    }
    
    
    TMR0 = 235; // valor de n para t=5ms
    INTCONbits.T0IF = 0;
    
    return;
}

void IOCB_interrupt(){ // se verifica el push presionado e incrementa o decrem..

    if (PORTBbits.RB0 == 0){
        PORTC++;
    }else if (PORTBbits.RB1 == 0){
        PORTC--;
    }
    
    INTCONbits.RBIF = 0;
    
    return;
} 

// configuraciones

void config_reloj(){

    OSCCONbits.IRCF2 =1 ; // IRCF = 110 (4Mz) 
    OSCCONbits.IRCF1 =1 ;
    OSCCONbits.IRCF0 =0 ;
    OSCCONbits.SCS = 1; // Habilitar reloj interno
    
    return;
}

void config_io(){
   
    ANSEL = 0x03;
    ANSELH = 0x00; 
    
    TRISA = 0x03;
    TRISC = 0x00; // puertos c y d como salidas
    TRISD = 0x00;
    TRISE = 0x00;
    TRISB = 0x03;
    
    OPTION_REGbits.nRBPU =  0 ; // se habilita el pull up interno en PORTB
    WPUB = 0x03;  // se habilita los pull ups para los pines RB0 y RB1 y RB2

    PORTB = 0x03;
    PORTA = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x01;
    
   // configuracion del adc
    ADCON1bits.ADFM = 0; // justificacion a la izquierda
    ADCON1bits.VCFG0 = 0; //configuracion vdd y ground en vss
    ADCON1bits.VCFG1 = 0; 
    
    ADCON0bits.ADCS = 0b10; // conversion del clock a fosc/32
    ADCON0bits.CHS = 0;
    __delay_us(50);
    ADCON0bits.ADON = 1;
      
    
    return;
}

void config_int_enable(){
    
    INTCONbits.GIE = 1; // Se habilitan las interrupciones globales
    INTCONbits.PEIE = 1;
    
    INTCONbits.RBIE = 1; // habilitar banderas de interrupción puertos B
    INTCONbits.RBIF = 0; 	
    
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    
    INTCONbits.T0IE = 1; // Se habilitan la interrupción del TMR0
    INTCONbits.T0IF = 0; // Se limpia la bandera
    
    //configuración iocb
    IOCB = 0x07; // setear interrupciones en los pines RB0 y RB1 
    
    INTCONbits.RBIF = 0; 
    
    return;
}
    
void config_timer0(){

    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS2 = 1; // PS 111 = 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    
    TMR0 = 235; // valor de n para t=5ms
    INTCONbits.T0IF = 0;
            
    return;
}





