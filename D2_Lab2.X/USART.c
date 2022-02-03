#include "USART.h"


void config_rx(void)
{
    SPBRGH = 0;
    SPBRG = 25;                 // Selector de Baud Rate
    TXSTAbits.SYNC = 0;         // Modo asíncrono
    TXSTAbits.BRGH = 1;         // High Baud Rate alta velocidad
    BAUDCTLbits.BRG16 = 0;      // Generador de Baud Rate de 8 bits
    
    RCSTAbits.RX9 = 0;          // Recepción de 8 bits
    RCSTAbits.SPEN = 1;         // Habilitar puertos seriales
    RCSTAbits.CREN = 1;         // Habilitar recepción de datos
    
    PIE1bits.RCIE = 1;          // Habilitar interrupciones de recepción
    PIR1bits.RCIF = 0;          // Apagar bandera de interrupciones de recepción
    return;
}