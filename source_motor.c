//AndrewLEE_HyeunCHUNG

#include <xc.h> //Microcontroller library. 
#include <string.h> //Strings library. 
#include "header_motor.h" //Link to header file.

// function to set PWM from the values in the motor structure
void setMotorPWM(struct DC_motor *m) {
    int PWMduty; //tmp variable to store PWM duty cycle
    if (m->direction) { //if forward
        // low time increases with power
        PWMduty = m->PWMperiod - ((int) (m->power)*(m->PWMperiod)) / 100;
    } else { //if reverse
        // high time increases with power
        PWMduty = ((int) (m->power)*(m->PWMperiod)) / 100;
    }
    PWMduty = (PWMduty << 2); // two LSBs are reserved for other things
    *(m->dutyLowByte) = PWMduty & 0xFF; //set low duty cycle byte
    *(m->dutyHighByte) = (PWMduty >> 8) & 0x3F; //set high duty cycle byte

    if (m->direction) { // if direction is high,
        LATB = LATB | (1 << (m->dir_pin)); // set dir_pin bit in LATB to high
    } else { // if direction is low,
        LATB = LATB & (~(1 << (m->dir_pin))); // set dir_pin bit in LATB to low
    }
}

void setMotorFullSpeed(struct DC_motor *m) {
    for (m->power; (m->power) <= 100; (m->power)++) { //increase motor power until 100
        setMotorPWM(m); //pass pointer to setMotorSpeed function (not &m here)
        __delay_us(10); //delay of 5 ms (500 ms from 0 to 100 full power)
    }
}

void initPWM(void){ // function to setup PWM
    TRISBbits.TRISB0 = 0;

    PTCON0 = 0b00000000; // free running mode, 1:1 prescale
    PTCON1 = 0b10000000; // enable PWM timer

    PWMCON0 = 0b01101111; // PWM1/3 enabled, all independent mode
    PWMCON1 = 0x00; // special features, all 0 (default)

    PTPERL = 199; // base PWM period low byte
    PTPERH = 0x00; // base PWM period high byte
}

void delay_s(char seconds) { //convenient function to delay 1 second
    for (int i = 0; i < 20; i++) {
        __delay_ms(50);
    }
};

void initMotor(void) {
    //Configure left motor.
    motorL.power = 0; //zero power to start
    motorL.direction = 1; //set default motor direction
    motorL.dutyLowByte = (unsigned char *) (&PDC0L); //store address of PWM duty low byte
    motorL.dutyHighByte = (unsigned char *) (&PDC0H); //store address of PWM duty high byte
    motorL.dir_pin = 0; //pin RB0/PWM0 controls direction
    motorL.PWMperiod = PWMcycle; //store PWMperiod for motor
    
    //Configure left motor.
    motorR.power = 0; //zero power to start
    motorR.direction = 1; //set default motor direction
    motorR.dutyLowByte = (unsigned char *) (&PDC1L); //store address of PWM duty low byte
    motorR.dutyHighByte = (unsigned char *) (&PDC1H); //store address of PWM duty high byte
    motorR.dir_pin = 2; //pin RB3/PWM3 controls direction
    motorR.PWMperiod = PWMcycle; //store PWMperiod for motor

    //Sets percentage speed, directions 1&0, motors 1&0.
    setMotorPWM(&motorL); 
    setMotorPWM(&motorR);
}

void stop(struct DC_motor *m_L, struct DC_motor *m_R) {
    //Motors stop.
    m_L->power = 0;
    m_R->power = 0;
    m_L->direction = 1;
    m_R->direction = 1;
    setMotorPWM(m_L);
    setMotorPWM(m_R);
}

void turnLeft(struct DC_motor *m_L, struct DC_motor *m_R) {
    //Motors turn left.
    stop(&motorL, &motorR);
    m_L->direction = 0;
    __delay_us(1);
    while ((m_L->power) <= 65) { //Power down from 70 to 65 as motors turns right slightly faster.
        setMotorPWM(m_L);
        setMotorPWM(m_R);
        (m_R->power)++;
        (m_L->power)++;
        __delay_us(1);
    }
}

void turnRight(struct DC_motor *m_L, struct DC_motor *m_R) {
    //Motors turn right.
    stop(&motorL, &motorR);
    m_R->direction = 0;
    __delay_us(1);
    while ((m_R->power) <= 70) {
        setMotorPWM(m_L);
        setMotorPWM(m_R);
        (m_R->power)++;
        (m_L->power)++;
        __delay_us(1);
    }
}

void fullSpeedAhead(struct DC_motor *m_L, struct DC_motor *m_R) {
    //Motors move forwards.
    stop(&motorL, &motorR);
    m_R->direction = 1;
    m_L->direction = 1;
    __delay_us(1);
    while ((m_R->power) <= 70) {
        setMotorPWM(m_L);
        setMotorPWM(m_R);
        (m_R->power)++;
        (m_L->power)++;
        __delay_us(1);
    }
}

void fullSpeedBack(struct DC_motor *m_L, struct DC_motor *m_R) {
    //Motors move backwards.
    stop(&motorL, &motorR);
    m_R->direction = 0;
    m_L->direction = 0;
    __delay_us(1);
    while ((m_R->power) <= 70) {
        setMotorPWM(m_L);
        setMotorPWM(m_R);
        (m_R->power)++;
        (m_L->power)++;
        __delay_us(1);
    }
} 