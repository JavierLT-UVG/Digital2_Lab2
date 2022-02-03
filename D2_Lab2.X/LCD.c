#include "LCD.h"

#define lcd_0 PORTAbits.RA0
#define lcd_1 PORTAbits.RA1
#define lcd_2 PORTAbits.RA2
#define lcd_3 PORTAbits.RA3
#define lcd_4 PORTAbits.RA4
#define lcd_5 PORTAbits.RA5
#define lcd_6 PORTAbits.RA6
#define lcd_7 PORTAbits.RA7

#define lcd_PORT PORTA

#define lcd_RS PORTCbits.RC0
#define lcd_E PORTCbits.RC1


void pulse_E(void)          // Pulso de 0.5 ms en E
{
    lcd_E = 1;
    __delay_ms(0.5);
    lcd_E = 0;
    return;
}

void ini_LCD(void)                  // Inicializar el LCD
{
    lcd_RS = 0;                     // Iniciar todo en 0
    lcd_E = 0;
    lcd_PORT = 0;
    
    __delay_ms(16);                 // Encender LCD
    w_Instruction(0b00110000);
    __delay_ms(5);
    w_Instruction(0b00110000);
    __delay_us(200);
    w_Instruction(0b00110000);
    
    __delay_ms(4);
    w_Instruction(0b00111000);      // Modo dos líneas, caracteres 5x8
    
    __delay_ms(4);
    w_Instruction(0b00001100);      // Enceder y configurar cursor (invisible)
    return;
}

void w_Instruction(uint8_t instruction) // Escribir instrucciones de config
{
    lcd_RS = 0;             // Pines como instrucciones
    lcd_PORT = instruction; // Cargar instrucción al Puerto
    pulse_E();              // Enable para que pase la instrucción
    return;
}

void w_Char(unsigned char data)         // Escribir un solo char
{
    lcd_RS = 1;             // Pines como dato a mostrar en pantalla
    lcd_PORT = data;        // Cargar char al Puerto
    pulse_E();              // Enable para que pase el char a la pantalla
    return;
}

void w_String(unsigned char data[])     // Escribir arrays de char (strings)
{
    for(uint8_t i=0; i<strlen(data); i++)   // Para cada char en el array de chars "data"
    {
        w_Char(data[i]);                    // Escribir en la pantalla cada char
    }
    return;
}

void set_Cursor(uint8_t line, uint8_t pos) // Posicionar cursor donde se desee
{                                          // Parámetros: line (0-1), pos(0-39)
    uint8_t direction = 0b10000000;        // Modo configurar posición de cursor
    line = line<<6;                        // Definir línea (0 o 1)
    direction = direction | line | pos;    // Unir config, con línea, con posición en la línea en una sola instrucción
    w_Instruction(direction);              // Enviar instrucción al LCD
    return;
}

void w_Titulos(void)    // Escribir el layout del programa
{
    // Escribir string de título en primera línea
    unsigned char title[] = "S1:   S2:   S3:";
    set_Cursor(0,1);
    w_String(title);        
    
    // Escribir "." y "V" de Pot 1
    set_Cursor(1,1);        
    w_Char('.');
    set_Cursor(1,4);
    w_Char('V');
    
    // Escribir "." y "V" de Pot 2
    set_Cursor(1,7);        
    w_Char('.');
    set_Cursor(1,10);
    w_Char('V');
}
