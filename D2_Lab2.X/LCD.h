/* 
 * File: LCD.h
 * Author: Francisco Javier López Turcios
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_H
#define	LCD_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <string.h>
#define _XTAL_FREQ 4000000

void pulse_E(void);         // Función de pulso positivo en E
void ini_LCD(void);         // Inicializar LCD
void w_Instruction(uint8_t instruction);    // Escribir instrucción en LCD
void w_Char(unsigned char data);            // Escribir un char en LCD
void w_String(unsigned char data[]);        // Escribir un string en LCD
void set_Cursor(uint8_t line, uint8_t pos); // Setear posición de cursor
void w_Titulos(void);       // Función que escribe el layout en el LCD

#endif	

