//AndrewLEE_HyeunCHUNG

#include <xc.h> //Microcontroller library. 
#include <string.h> //Strings library. 
#include "header_LCD.h" //Link to header file.

void E_TOG(void) { //Delay between on and off
    LCD_E = 1;
    __delay_us(5); // 5us delay 
    LCD_E = 0;
}

//function to send four bits to the LCD
void LCDout(unsigned char number) {
    //set data pins using the four bits from number. Want in binary for each bit
    LCD_DB4 = (number & 0b0001); //first bit of number
    LCD_DB5 = (number & 0b0010) >> 1; //second bit of number
    LCD_DB6 = (number & 0b0100) >> 2; //etc.
    LCD_DB7 = (number & 0b1000) >> 3;
    //toggle the enable bit to send data
    E_TOG();
    __delay_us(5); // 5us delay
}

//function to send data/commands over a 4bit interface
void SendLCD(unsigned char Byte, char type) {
    LCD_RS = type; // set RS pin whether it is a Command (low,0) or Data/Char (high,1)
    // using type as the argument
    LCDout((Byte & 0b11110000) >> 4); // send high bits of Byte using LCDout function
    __delay_us(10); // 10us delay
    LCDout((Byte & 0b00001111)); // send low bits of Byte using LCDout function
}

void LCD_Init(void) {
    // set initial LAT output values (they start up in a random state)
    LATA = 0;
    LATD = 0;
    LATC = 0;
    // set LCD pins as output (TRIS registers)
    TRISA = 0;
    TRISC = 0;
    TRISD = 0;

    // Initialisation sequence code - see the data sheet
    //delay 15mS
    __delay_ms(15);
    //send 0b0011 using LCDout
    LCDout(0b0011);
    //delay 5ms
    __delay_ms(5);
    //send 0b0011 using LCDout
    LCDout(0b0011);
    //delay 200us
    __delay_us(200);
    //send 0b0011 using LCDout
    LCDout(0b0011);
    //delay 50us
    __delay_us(50);
    //send 0b0010 using LCDout set to four bit mode
    LCDout(0b0010);
    // now use SendLCD to send whole bytes ? send function set, clear
    // screen, set entry mode, display on etc to finish initialisation
    SendLCD(0b00101100, 0); //Set function
    __delay_ms(10);
    SendLCD(0b00001000, 0); //Display off
    __delay_ms(10);
    SendLCD(0b00000001, 0); //Clear screen
    __delay_ms(10);
    SendLCD(0b00000000, 0); //Entry mode set
    __delay_ms(10);
    SendLCD(0b00001101, 0); //Display on
}

//function to put cursor to start of line
void SetLine(char line) {
    if (line == 1) {
        SendLCD(0x80, 0);
    } //Send 0x80 to set line to 1 (0x00 ddram address)
    else if (line == 2) {
        SendLCD(0xC0, 0);
    } //Send 0xC0 to set line to 2 (0x40 ddram address)
    __delay_us(50); // 50us delay
}

void LCD_String(char *string) {
    //While the data pointed to is not a 0x00 do below
    while (*string != 0) {
        //Send out the current byte pointed to
        // and increment the pointer
        SendLCD(*string++, 1);
        __delay_us(50); //so we can see each character
        //being printed in turn (remove delay if you want
        //your message to appear almost instantly)
    }
}

void LCD_Clear(void) {
    SendLCD(0b00000001, 0); //Clear screen
    __delay_ms(10);
}

char getCharSerial(void) {
    while (!PIR1bits.RCIF); //wait for the data to arrive
    return RCREG; //return byte in RCREG
}

void initBITS(void) {
    TRISC = 0b11000000; //set data direction registers
    //both need to be 1 even though RC6
    //is an output, check the datasheet!
    OSCCON = 0b11110010; // internal oscillator, 8MHz
    while (!OSCCONbits.IOFS); //Wait for OSC to become stable

    SPBRG = 205; //set baud rate to 9600
    SPBRGH = 0;
    BAUDCONbits.BRG16 = 1; //set baud rate scaling to 16 bit mode
    TXSTAbits.BRGH = 1; //high baud rate select bit
    RCSTAbits.CREN = 1; //continous receive mode
    RCSTAbits.SPEN = 1; //enable serial port, other settings default
    TXSTAbits.TXEN = 1; //enable transmitter, other settings default
}