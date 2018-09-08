//AndrewLEE_HyeunCHUNG

/* 
 * File:   header_motor.h
 * Author: hwc115
 *
 * Created on 13 March 2018, 10:34
 */

#ifndef HEADER_MOTOR_H
#define	HEADER_MOTOR_H

#define _XTAL_FREQ 8000000 //8Mhz frequency for motors.
#define PWMcycle 199 //standard, same motor PWM as no servo used.

//FUNCTIONS
void setMotorPWM(struct DC_motor *m); //Set PWM of Motors.
void setMotorFullSpeed(struct DC_motor *m); //Set Motors to full speed//
void initPWM(void); //Initialise PWM of Motors
void delay_s(char seconds); //Delays in seconds.
void initMotor(void); //Initialise Motors
void stop(struct DC_motor *m_L, struct DC_motor *m_R); //Stop Motor
void turnLeft(struct DC_motor *m_L, struct DC_motor *m_R); //Turn Left
void turnRight(struct DC_motor *m_L, struct DC_motor *m_R); //Turn Right
void fullSpeedAhead(struct DC_motor *m_L, struct DC_motor *m_R); //Move Forwards
void fullSpeedBack(struct DC_motor *m_L, struct DC_motor *m_R); //Move Backwards

//Structure for Motor components

struct DC_motor {
    char power; //motor power, out of 100
    char direction; //motor direction, forward(1), reverse(0)
    unsigned char *dutyLowByte; //PWM duty low byte address
    unsigned char *dutyHighByte; //PWM duty high byte address
    char dir_pin; // pin that controls direction on PORTB
    int PWMperiod; //base period of PWM cycle
};

struct DC_motor motorL, motorR; //declare two DC_motor structures

#endif	/* HEADER_MOTOR_H */

