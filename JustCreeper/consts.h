///////////////////////////////////////////////////////////////////////////////
//
// JustCreeper project
// Version 1.0
// October 2018
// Create by Vladimir Rybakov aka JustMoose
// Original: https://github.com/TheJustMoose/JustCreeper/
// License: MIT
// 
///////////////////////////////////////////////////////////////////////////////
#ifndef JUST_CREEPER_CONSTS_H
#define JUST_CREEPER_CONSTS_H

// motor speed sensor pins
const byte LEFT_SENSOR_PIN = PINH3;  // Arduino pin 16
const byte RIGHT_SENSOR_PIN = PINH4; // Arduino pin 17
#define SENSOR_PORT PINH // Do not use "const byte" here, it's not work

// motor driver pins
const byte LEFT_MOTOR_PLUS = 47;
const byte LEFT_MOTOR_MINUS = 49;
const byte RIGHT_MOTOR_PLUS = 53;
const byte RIGHT_MOTOR_MINUS = 51;

// motor masks
const byte RIGHT_BACK_MASK = B00000001;
const byte LEFT_BACK_MASK = B00001000;
const byte RIGHT_FW_MASK = B00000010;
const byte LEFT_FW_MASK = B00000100;

const int LEFT_PWM_PIN = 44;
const int RIGHT_PWM_PIN = 46;

const byte MAX_MOTOR_SPEED = 255;
const byte MAX_TURN_SPEED = 255;

const byte TRIG_1_PIN = 3;
const byte ECHO_1_PIN = 4;

const byte TRIG_2_PIN = A3;
const byte ECHO_2_PIN = A2;

const byte TRIG_3_PIN = A5;
const byte ECHO_3_PIN = A4;

const byte TRIG_4_PIN = 13;
const byte ECHO_4_PIN = 12;

const byte BATTERY_METER_PIN = A0;

// "audio" pins
const int REG_DATA = 7;    // PK5 == A15
const int REG_CLK = 6;     // PK6 == A14
const int REG_LATCH = 5;   // PK7 == A13
#define REG_PORT PORTK
#define REG_DIRECTION DDRK

// kbd pins
const byte KBD_LINE_1 = 8;
const byte KBD_LINE_2 = 9;
const byte KBD_LINE_3 = 11;
const byte KBD_LINE_4 = 10;
#define KBD_PORT PORTA
#define KBD_DIRECTION DDRA

#endif

