//AndrewLEE_HyeunCHUNG

#include <xc.h> //Microcontroller library. 
#include <stdio.h> //Standary library for functions. 

#include "header_LCD.h" //header file for LCD.
#include "header_motor.h" //header file for Motors.

#pragma config OSC = IRCIO //Use internal timer for LCD screen. 

volatile char state; //This project uses two stages/cases to switch between SEARCHING mode and RETURNING mode. 
#define SEARCHING 0 //when state = 0;
#define RETURNING 1 //when state = 1;

#define PWMcycle 199 //For motors

void main() {

    //MOTOR Settings.
    TRISB = 0;
    LATB = 0;
    initMotor();
    initPWM();

    //RFID Interrupt and Initialisation Settings (Part 1).
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
    initBITS();

    char buf1[16]; //buffer for storing IR (CAP1) readings to LCD screen.

    //LCD Settings.
    OSCCON = 0x72; //8MHz clock.
    while (!OSCCONbits.IOFS); //Wait for OSC to become stable.
    LCD_Init(); //Initialise the LCD screen.

    //IR sensor Settings. 
    //Initialising CAP1 for IR sensor.
    CAP1CON = 0b01000110; //P115, 160  0110 = Pulse-Width Measurement mode, every falling to rising edge.
    LATA = 0; //Set Pins to zero. 
    TRISAbits.RA2 = 1; //set CAP1 pin as input from IR sensor.

    //TIMER 5 P139 for IR sensor.
    T5CONbits.TMR5ON = 1; //Enable Timer 5.
    T5CONbits.TMR5CS = 0; //Use Internal clock.
    T5CONbits.T5PS = 0b11; //1:8 pre-scaler.

    //Set from Analogue to Digital for IR readings.
    ANSEL0 = 0;
    ANSEL1 = 0;

    unsigned int CAP1BUF; //Used to combine high and low readings from IR sensor.

    //RFID Settings (Part 2) added here to avoid pin interference.
    TRISC = 0b11000000;

    //RETURN Procedure Settings.
    int LOG[100]; //For logging FORWARD(0) and TURNLEFT(1).
    for (int k = 0; k < 100; k++) { //fill LOG array with 0.
        LOG[k] = 0;
    }

    char FLIP = 1; // Flips between FORWARD(0) and TURNRIGHT(1), starts (1) to search.
    signed int i = 0; // Index number for LOG array. Must be signed as motor stops when (i=(-1)).

    while (1) {
        
        switch (state) {
            case SEARCHING:
                CAP1BUF = (CAP1BUFH << 8) + CAP1BUFL; //Combine higher and lower values of IR reading (CAP1).

                sprintf(buf1, "IR = %u", CAP1BUF); //write IR reading into buf1.
                SetLine(1); //Set to send to first line of LCD screen.
                LCD_Clear(); //Clear screen.
                LCD_String(buf1); //Display IR reading to LCD screen. 

                if (CAP1BUF > 12500 && CAP1BUF < 20000) { //The IR fluctuates above and around 12500 when beacon is detected.

                    fullSpeedAhead(&motorL, &motorR); //Move forwards.

                    //This below is to check whether to increment the current index of LOG array, or move to next slot. 
                    //Only moves to the next slot of LOG array if current move != previous move.
                    if (FLIP == 1) {
                        FLIP = 0; //0 means FORWARDS(0).
                        i++; //move next slot as the previous move was turning. 
                    }
                    //If current move == previous move, it skips above IF. 
                    //Either way, increment current or new index of LOG array.
                    LOG[i]++; //increment index value by one iteration.

                } else {
                    turnRight(&motorL, &motorR); //Any other IR readings that are not above 12500 means beacon is not detected.

                    //This below is to check whether to increment the current index of LOG array, or move to next slot. 
                    //Only moves to the next slot of LOG array if current move != previous move.           
                    if (FLIP == 0) {
                        FLIP = 1; //1 means TURNRIGHT(1).
                        i++; //move next slot as the previous move was move forwards.
                    }
                    //If current move == previous move, it skips above IF. 
                    //Either way, increment current or new index of LOG array.
                    LOG[i]++; //increment index value by one iteration.
                }
                break;

            case RETURNING:
                //Using the current index number i, jump to last instruction(index value) of LOG array.
                //This conveniently ignores any null values tailing the end of the LOG array
                //Start from current FLIP value. ie If FORWARD(0), move backwards. If TURNRIGHT(1), turn left.
                //Then switch FLIP value after all iterations are fed back from current index i and move index backwards by one. 

                for (i = i; i >= 0; i--) { //Starting backwards. i will always be at last filled slot of LOG so ignores empty slots.
                    // i is also signed so when (i=(-1)), the motors stop.
                    for (int j = 0; j < LOG[i]; j++) { //Iterate backwards while sending movement instructions to motor each time.
                        if (FLIP == 0) { //If FLIP is 0, move back.
                            fullSpeedBack(&motorL, &motorR);
                        } else { //If FLIP is 1, turn left.
                            turnLeft(&motorL, &motorR);
                        }
                        __delay_ms(17);
                        //This is a calibrated delay so the motors move as the same speed as the speed during the navigation stage. 
                        //Making this higher will make RETURN motors move faster and further, thus overshooting return position.
                        //Making this lower will make RETURN motors slower and weaker, thus falling short of starting position. 
                    }

                    //switch FLIP since the next movement is different.
                    //FORWARD(0), TURNRIGHT(1).
                    if (FLIP == 0) {
                        FLIP = 1;
                    } else {
                        FLIP = 0;
                    }
                }

                //When all instructions are fed through (i=(-1)), stop motors.
                if (i < 0) {
                    stop(&motorL, &motorR);
                }
                break;
        }
    }
}

//Interrupt for RFID Reader
void interrupt high_priority InterruptHandlerHigh() {
    if (PIR1bits.RCIF) {

        char buf[12]; //Array for RFID code.
        char counter = 0; //Counter to check only 12 values pass through.
        int num = getCharSerial(); //integer to hold each reading to check for num == 2. 

        if (num == 2) { //Only start when the first value from RFID is 2 (see datasheet).
            LCD_Clear(); //Clear LCD Screen (Previous has IR readings real-time)

            while (counter < 12) { //While loop to fill RFID (buf) array
                int RFIDa = getCharSerial();
                buf[counter] = RFIDa;
                counter++;
            }

            LCD_String(buf); //send RFID data to the LCD screen.
            state = 1; //Enter RETURN phase
        }
    }
}