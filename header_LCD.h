//AndrewLEE_HyeunCHUNG

/* 
 * File:   header_LCD.h
 * Author: hwc115
 *
 * Created on 12 March 2018, 10:15
 */

#ifndef HEADER_LCD_H
#define	HEADER_LCD_H

#pragma config OSC = IRCIO //compiler directive to set clock to internal oscillator.
#define _XTAL_FREQ 8000000 //define _XTAL_FREQ for delay routines.

//Setup macros for LCD, allocating PINS.
#define LCD_RS LATAbits.LATA6
#define LCD_E LATCbits.LATC0
#define LCD_DB4 LATCbits.LATC1
#define LCD_DB5 LATCbits.LATC2
#define LCD_DB6 LATDbits.LATD0
#define LCD_DB7 LATDbits.LATD1

//FUNCTIONS for LCD
void E_TOG(void); //Toggle.
void LCDout(unsigned char number); //Allocating data pins connection to LCD.
void SendLCD(unsigned char Byte, char type); //Send to LCD.
void LCD_Init(void); //Initialising LCD.
void SetLine(char line); //Set 1 for first row, 2 for second row.
void LCD_String(char *string); //String output to LCD.
void LCD_Clear(void); //Clear LCD.
char getCharSerial(void); //get data from RFID.
void initBITS(void); //Serial Initialisation.

#endif	/* HEADER_LCD_H */

