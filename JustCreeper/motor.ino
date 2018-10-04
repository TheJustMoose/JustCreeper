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
/*

  Motor routins.

*/

void SetMotorPins(byte val) {
  LOG("motor mask: ");
  LOGin(val);

  digitalWrite(RIGHT_MOTOR_PLUS, val & RIGHT_FW_MASK);
  digitalWrite(RIGHT_MOTOR_MINUS, val & RIGHT_BACK_MASK);
  digitalWrite(LEFT_MOTOR_MINUS, val & LEFT_BACK_MASK);
  digitalWrite(LEFT_MOTOR_PLUS, val & LEFT_FW_MASK);

  old_motor_mask = val;
}


void SetMotorSpeed(byte val) {
  analogWrite(LEFT_PWM_PIN, val);
  analogWrite(RIGHT_PWM_PIN, val);
}

void InitMotors() {
  pinMode(RIGHT_MOTOR_PLUS, OUTPUT);
  pinMode(RIGHT_MOTOR_MINUS, OUTPUT);
  pinMode(LEFT_MOTOR_MINUS, OUTPUT);
  pinMode(LEFT_MOTOR_PLUS, OUTPUT);

  SetMotorPins(0);
  SetMotorSpeed(MAX_MOTOR_SPEED);
}

