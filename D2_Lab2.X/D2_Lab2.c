/*
 * File:   D2_Prelab2.c
 * Author: Francisco Javier López Turcios
 *
 * Created on 30 de enero de 2022, 10:05 PM
 */


// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include "LCD.h"
#include "ADC.h"
#include "USART.h"
#define _XTAL_FREQ 4000000

//============================================================================
//============================ VARIABLES GLOBALES ============================
//============================================================================
struct pots
{
    uint8_t adresh;
    uint16_t mapeado;
    uint8_t centena;
    uint8_t decena;
    uint8_t unidad;
}pot1, pot2, cont;

uint8_t data_rx;

//============================================================================
//========================= DECLARACIÓN DE FUNCIONES =========================
//============================================================================
void config_io(void);
void config_reloj(void);
void config_int(void);
uint16_t map(uint8_t num);
void divisor(uint16_t num, uint8_t *centena, uint8_t *decena, uint8_t *unidad);

//============================================================================
//============================== INTERRUPCIONES ==============================
//============================================================================
void __interrupt() isr (void)
{
    if(PIR1bits.ADIF)               // Interrupción del ADC
    {
        if(ADCON0bits.CHS == 10)    // Si el canal es 10
            pot2.adresh = ADRESH;
        else                        // Si el canal es 12
            pot1.adresh = ADRESH;         
        
        PIR1bits.ADIF = 0;          // Limpiar bandera
    }
    
    if(PIR1bits.RCIF)               // Interrupción de RX EUSART
    {
        data_rx = RCREG;
        switch(data_rx)
        {
            case '+':                //
                cont.adresh++;
                break; 
            case '-':                //
                cont.adresh--;
                break;
            default:
                break;
        }
        PIR1bits.RCIF = 0;          // Limpiar bandera
    }
}

//============================================================================
//=================================== MAIN ===================================
//============================================================================
void main(void) 
{
    config_io();        // Configurar entradas y salidas
    config_reloj();     // Configurar oscilador interno
    
    ini_LCD();          // Inicializar LCD
    w_Titulos();        // Escribir layout en LCD
    
    config_int();       // Encender interrupciones       
    config_adc();       // Configurar y encender ADC
    config_rx();        // Configurar recepción de datos
    
    while(1)
    {
        if(!ADCON0bits.GO)              // Si la última conversión ya terminó, entrar
        {
            if(ADCON0bits.CHS == 10)    // Si el último canal fue 10, cambiar a 12
                ADCON0bits.CHS = 12;
            else                        // Si el último canal no fue 10, cambiar a 10
                ADCON0bits.CHS = 10;
            
            __delay_us(50);             // Delay para no interrumpir conversión
            ADCON0bits.GO = 1;          // Iniciar nueva conversión
        }
        
        // Mapear valores de 255-0 a 500-0
        pot1.mapeado = map(pot1.adresh);    
        pot2.mapeado = map(pot2.adresh);    
        
        // Obtener centenas, decenas y unidades de cada número
        divisor(pot1.mapeado, &pot1.centena, &pot1.decena, &pot1.unidad);   
        divisor(pot2.mapeado, &pot2.centena, &pot2.decena, &pot2.unidad);
        divisor(cont.adresh, &cont.centena, &cont.decena, &cont.unidad);
        
        // Escribir valor de Pot1 en LCD
        set_Cursor(1,0);                
        w_Char(pot1.centena + '0');
        set_Cursor(1,2);
        w_Char(pot1.decena + '0');
        set_Cursor(1,3);
        w_Char(pot1.unidad + '0');
        
        // Escribir valor de Pot2 en LCD
        set_Cursor(1,6);                
        w_Char(pot2.centena + '0');
        set_Cursor(1,8);
        w_Char(pot2.decena + '0');   
        set_Cursor(1,9);
        w_Char(pot2.unidad + '0');
        
        // Escribir valor de Cont en LCD
        set_Cursor(1,13);
        w_Char(cont.centena + '0');
        set_Cursor(1,14);
        w_Char(cont.decena + '0');
        set_Cursor(1,15);
        w_Char(cont.unidad + '0');
    } 
}


//============================================================================
//================================ FUNCIONES =================================
//============================================================================
void config_io(void)
{
    ANSEL = 0;                  // Pines digitales
    ANSELH = 0b00010100;        // AN10 y AN12 encendido
    
    TRISA = 0;                  // Puerto A salida (LEDs)
    TRISB = 0b11;               // Puerto B como entrada (Pots)
    TRISC = 0b10000000;         // Puerto C en 7 (RX) entrada, el resto salida
    
    PORTA = 0;                  // Limpiar condiciones iniciales de los puertos
    PORTB = 0;
    PORTC = 0;
    return;
}

void config_reloj(void)         // Configuración del oscilador
{
    OSCCONbits.IRCF2 = 1;       // 4MHz
    OSCCONbits.IRCF1 = 1;       // 
    OSCCONbits.IRCF0 = 0;       // 
    OSCCONbits.SCS = 1;         // Reloj interno
    return;
}

void config_int(void)
{
    INTCONbits.GIE  = 1;        // Activar interrupciones
    INTCONbits.PEIE = 1;        // Activar interrupciones periféricas
    return;
}

uint16_t map(uint8_t num)       // Mapear valores de rango 255/0 a 500/0
{
    uint16_t res;               // Definir variable de salida
    res = num*100/51;           // 500/255 = 100/51 (Factor de mapeo)
    return res;                 // Devolver variable mapeada
}

void divisor(uint16_t num, uint8_t *centena, uint8_t *decena, uint8_t *unidad)  // Divisor que saca los dígitos de un número
{
    *centena = num / 100;       // Obtener centenas
    uint8_t aux = num % 100;    // Auxliar que almacena decenas y unidades
    *decena = aux / 10;         // Obtener decenas
    *unidad = aux % 10;         // Obtener unidades
    return;
}