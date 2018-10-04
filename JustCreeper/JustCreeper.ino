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
// It's a makeshift substitute for one toy, which was popular in my childhood.
// You can programme its moving by pressing buttons on on-board keyboard.
//
// Based on Arduino Mega.
//
// Use 1:120 gearbox like this:
// https://www.ebay.com/itm/231130151482
// With motors like this:
// https://www.ebay.com/itm/192389719680
// Use "encoders" like this (please use permanent marker to draw 2 sectors):
// https://www.ebay.com/itm/322267744365
// Use Optocoupler like this:
// http://www.ebay.com/itm/291742103149
// Use driver like this:
// https://ru.aliexpress.com/item/L293D-Mini-4-Channel-CH-Motor-Drive-Shield-Expansion-Board-Module-High-Voltage-Current-Module-For/1859086104.html
//
// Ready item:
// http://u0532098.isp.regruhosting.ru/img/chassis/chip.jpg
//
// TimerOne library required.
// https://www.pjrc.com/teensy/td_libs_TimerOne.html
//

#include <TimerOne.h>

#include "consts.h"
#include "macros.h"
#include "cmd.h"

const int STEP_SIZE = 250;
const int TURN_SIZE = 75;
const int PAUSE_SIZE = 10;

ULONG Abs(long val) {
  return (val > 0) ? val : -val;
}

byte old_motor_mask = 0;

int right_dist = 0;
int left_dist = 0;
int pause_dist = 0;

CommandList editor_cmds; // cmd buffer to edit
CommandList prgm_cmds; // cmd buffer to run

int ip = 0;  // instruction pointer
boolean go = false;
boolean loop_done = false;

boolean lowbat = false;
boolean redraw = false;

int skip_adc = 0;
const int bat_adc_size = 16;
int bat_adc[bat_adc_size];
int bat_idx = 0;

// current command & repeat count
CmdType cur_cmd;
int cur_cnt;

volatile ULONG left_cnt = 0;
volatile ULONG right_cnt = 0;
boolean left_encoder_val = false;
boolean right_encoder_val = false;

void OnTimer1() {
  byte val = SENSOR_PORT;
  boolean vbl = val & _BV(LEFT_SENSOR_PIN);
  boolean vbr = val & _BV(RIGHT_SENSOR_PIN);
  if (vbl != left_encoder_val) {
    left_encoder_val = vbl;
    left_cnt++;
  }
  if (vbr != right_encoder_val) {
    right_encoder_val = vbr;
    right_cnt++;
  }
}

ULONG Left() {
  noInterrupts();
  ULONG res = left_cnt;
  interrupts();
  return res;
}

ULONG Right() {
  noInterrupts();
  ULONG res = right_cnt;
  interrupts();
  return res;
}

void ClrLeft() {
  noInterrupts();
  left_cnt = 0;
  interrupts();
}

void ClrRight() {
  noInterrupts();
  right_cnt = 0;
  interrupts();
}

byte MotorMask() {
  byte res = 0;
  if (left_dist > 0)
    res |= LEFT_FW_MASK;
  if (left_dist < 0)
    res |= LEFT_BACK_MASK;

  if (right_dist > 0)
    res |= RIGHT_FW_MASK;
  if (right_dist < 0)
    res |= RIGHT_BACK_MASK;
  return res;
}

void MotorLoop() {
  boolean done = false;
  
  ULONG l = Left();
  ULONG lmod = Abs(left_dist);
  if (lmod > 0 && l > lmod) {
    done = true;
    LOGn("left done");
  }

  ULONG r = Right();
  ULONG rmod = Abs(right_dist);
  if (rmod > 0 && r > rmod) {
    done = true;
    LOGn("right done");
  }

  if (done) {
    left_dist = 0;
    right_dist = 0;
    ClrLeft();
    ClrRight();
    l = 0;
    r = 0;
  }

  byte m = MotorMask();
  if (old_motor_mask == m && m) {
    delay(10); // small delay: motors should work :)
    LOGi(l);
    LOG(" ");
    LOGi(r);
    LOGn("  still work...");
  }

  if (old_motor_mask && old_motor_mask != MotorMask()) {
    SetMotorPins(MotorMask());
    LOG("new mask: ");
    LOGin(old_motor_mask);
  }

  if (pause_dist > 0) {
    pause_dist--;
    delay(100);
  }
}

void Push() {
  if (cur_cmd == CNOP)
    return;
  if (cur_cnt == 0)
    return;

  if (cur_cmd == CREPEAT && cur_cnt > editor_cmds.Len()) {
    LOGn("Loop is too length");
    PrintLoopTooLength();
    delay(1500);
  }
  else {
    editor_cmds.Push(cur_cmd, cur_cnt);
    PrintCmds();
  }
  
  cur_cmd = CNOP;
  cur_cnt = 0;
}

void DeleteLastCmd() {
  LOGn("del_last_cmd!");
  editor_cmds.EraseLast();
}

void DeleteAllCmds() {
  PrintDeleteAll();
  int c = 0;
  while (!(c = GetScanCode()))
    ;
  if (c != 29)
    return;
  
  LOGn("del_all_cmds!");
  editor_cmds.Clear();
  redraw = true;
}

void RunCmdAtIP() {
  LOG("run_cmd: ");
  LOGin(ip);
  if (ip >= prgm_cmds.Len()) {
    LOGn("Cmd idx out of bounds!!!");
    return;
  }

  Command cmd = prgm_cmds.Get(ip);
  if (!cmd.IsValid()) {
    LOGn("cmd is NULL");
    return;
  }

  int dist = STEP_SIZE*cmd.Cnt();
  int turn = TURN_SIZE*cmd.Cnt();
  int pause = PAUSE_SIZE*cmd.Cnt();
  boolean in_turn = false;

  switch (cmd.Cmd()) {
    case CLEFT: {
      left_dist = -turn;
      right_dist = turn;
      in_turn = true;
    } break;
    case CRIGHT: {
      left_dist = turn;
      right_dist = -turn;
      in_turn = true;
    } break;
    case CFWD: {
      left_dist = dist;
      right_dist = dist;
    } break;
    case CBWD: {
      left_dist = -dist;
      right_dist = -dist;
    } break;
    case CPAUSE: {
      pause_dist = pause;
    } break;
    case COUT: {
      // not implemented yet
    } break;
    case CFIRE: {
      for (int i = 0; i < cmd.Cnt(); i++)
        PlayAudio();
    } break;
    case CREPEAT: {
      if (loop_done) // just 1 step available now (like in original toy)
        return;

      loop_done = true;
      ip -= cmd.Cnt();
      LOG("Go to IP: ");
      LOGin(ip);
      RunCmdAtIP(); // recursion! achtung! warning! attention!
      return;
    } break;
    default: LOGn("Unknown cmd in run_cmd");
  }

  SetMotorPins(0); // stop!
  delay(500);      // wait a little before next command
  ClrLeft();       // clear all counters
  ClrRight();      // cause couters may change while car try to stop
  SetMotorSpeed(in_turn ? MAX_TURN_SPEED : MAX_MOTOR_SPEED);
  SetMotorPins(MotorMask());

  LOG("dist: ");
  LOGi(left_dist);
  LOG(" ");
  LOGin(right_dist);
  cmd.Log();
  LOGn("");
}

void RunProgram() {
  loop_done = false;
  ip = 0;
  go = true;
  RunCmdAtIP();
}

void RunLastCmd() {
  LOGn("\nrun last");
  if (editor_cmds.Len() == 0) {
    LOGn("Command list is empty.");
    return;
  }

  prgm_cmds.Clear();

  Command cmd = editor_cmds.Get(editor_cmds.Len() - 1);
  if (cmd.Cmd() == CREPEAT) {
    if (editor_cmds.Len() == 1) {
      LOGn("Nothing to repeat");
      return;
    }
    if (cmd.Cnt() > editor_cmds.Len() - 1) {
      LOGn("Loop cmd value is too length.");
      return;
    }

    // try to copy loop body
    int first = editor_cmds.Len() - cmd.Cnt() - 1;
    int j = 0;
    for (int i = first; i < editor_cmds.Len() - 1; i++, j++) { // do not copy CREPEAT cmd
      prgm_cmds.Push(editor_cmds.Get(i));
      LOG("Copy cmd["); LOGi(i); LOGn("]");
    }
  }
  else { // ordinal cmd (not loop)
    prgm_cmds.Push(cmd);
  }

  RunProgram();
}

void RunTest() {
  LOGn("run_test!");

  prgm_cmds.Clear();
  prgm_cmds.Push(CFWD, 2);
  prgm_cmds.Push(CFIRE, 3);
  prgm_cmds.Push(CBWD, 2);

  RunProgram();
}

void RunAllCmds() {
  PrintGo();
  LOGn("run_all!");

  prgm_cmds.Clear();
  for (int i = 0; i < editor_cmds.Len(); i++)
    prgm_cmds.Push(editor_cmds.Get(i));

  RunProgram();
}

boolean IsCmdInWork() {
  return right_dist || left_dist || pause_dist;
}

void ShowState() {
  if (go) {
    LOGn("In work");
    LOG("left_dist: ");
    LOGin(left_dist);
    LOG("right_dist: ");
    LOGin(right_dist);
    LOG("pause_dist: ");
    LOGin(pause_dist);
  }
  else {
    LOGn("In edit");
    for (int i = 0; i < editor_cmds.Len(); i++) {
      LOG("[");
      LOGi(i);
      LOG("] ");
      editor_cmds.Get(i).Log();
    }
  }
}

void StopProgram() {
  go = false;
  ip = 0;
  loop_done = false;
  SetMotorPins(0);
  ClrLeft();
  ClrRight();
  LOGn("Stop!");
  redraw = true;
}

void Interpret() {
  if (IsCmdInWork()) {
    MotorLoop();
    return;
  }

  if (go && (ip >= prgm_cmds.Len())) {
    StopProgram();
    LOGn("All done.");
    return;
  }

  ip++;
  LOG("IP: ");
  LOGin(ip);
  if (ip < prgm_cmds.Len())
    RunCmdAtIP();
}

void Edit() {
  int code = GetScanCode();
  if (code == 0)
    return;

  if (!IsNumericBtn(code))
    Push(); // save previous cmd

  switch (code) {
    case 28: cur_cmd = CLEFT; break;
    case 12: cur_cmd = CRIGHT; break;
    case 19: cur_cmd = CFWD; break;
    case 18: cur_cmd = CBWD; break;
    case 20: cur_cmd = CPAUSE; break;
    case 25: cur_cmd = COUT; break;
    case 17: cur_cmd = CFIRE; break;
    case  9: cur_cmd = CREPEAT; break;

    case 31: DeleteAllCmds(); break; // remove all!
    case 23: DeleteLastCmd(); break; // cancel last step
    case 24: RunTest(); return; // test
    case 32: RunTest(); return; // test
    case 21: RunLastCmd(); return; // check
    case 29: RunAllCmds(); return;  // go
  }

  if (IsNumericBtn(code)) {
    if (cur_cmd == CNOP) {
      LOGn("Please insert cmd before count.");
      CursorTo(0, 0);
      PrintError();
    }
    else {
      cur_cnt = cur_cnt*10 + BtnToDigit(code);
      PrintCurCmd();
      PrintCmds();
    }
  }

  if (Command::IsCmdExecutable(cur_cmd)) {
    PrintCurCmd();
    PrintCmds();
  }
}

void PrintCurCmd() {
  ClrScr();
  CursorTo(0, 0);
  PrintCmd(cur_cmd, cur_cnt);
}

void PrintCmds() {
  CursorTo(0, 2);
  for (int i = 0; i < editor_cmds.Len(); i++) {
    Command cmd = editor_cmds.Get(i);
    PrintCmd(cmd.Cmd(), cmd.Cnt());
  }
}

void setup() {
  Timer1.initialize(1000); // in us
  Timer1.attachInterrupt(OnTimer1);

  Serial.begin(9600);

  delay(1000);

  PrintBanner();
  CreateSyms();

  InitMotors();
  InitBat();
  InitAudio();

  pinMode(TRIG_1_PIN, OUTPUT); // sonar 1
  pinMode(TRIG_2_PIN, OUTPUT); // sonar 2
  pinMode(TRIG_3_PIN, OUTPUT); // sonar 3
  pinMode(TRIG_4_PIN, OUTPUT); // sonar 4
}

void Redraw() {
  if (!redraw)
    return;

  redraw = false;
  ClrScr();
  PrintBanner();
  PrintCmds();
  CheckBat();
}

void InitBat() {
  bat_idx = 0;
  for (int i = 0; i < bat_adc_size; i++)
    bat_adc[i] = 649; // Battery Ok!
}

int CalcBatAvg() {
  int res = 0;
  for (int i = 0; i < bat_adc_size; i++)
    res += bat_adc[i];
  return res / bat_adc_size;
}

void CheckBat() {
  lowbat = false;

  skip_adc++;
  if (skip_adc == 1) {
    // Lead-acid battery, voltage vs charge: 5.98V (40%) 6.37(100%)
    // (A0 via divider 10k + 10k)
    int val = analogRead(BATTERY_METER_PIN); // 1024 == 5V,
    LOG("adc val: ");
    LOGin(val);

    bat_idx++;
    bat_adc[bat_idx % 16] = val;
    if (bat_idx > 16)
      bat_idx = 0;
  }
  else if (skip_adc >= 10) {
    skip_adc = 0;
  }

  int bat_avg = CalcBatAvg();
  if (bat_avg >= 649) { // 95% == 6.34V
    PrintGoodbat();
  }
  else if (bat_avg >= 640) { // 80% == 6.25V
    PrintBat80();
  }
  else if (bat_avg >= 627) { // 60% == 6.12V
    PrintBat60();
  }
  else if (bat_avg >= 612) { // 40% == 5.98V
    PrintBat40();
  }
  else {
    lowbat = true;
    PrintLowbat();
  }

  LOG("bat_avg: ");
  LOGin(bat_avg);
  LOG("skip_adc: ");
  LOGin(skip_adc);
}

void CheckSerial() {
  if (Serial.available() > 0) { // stop!
    if (Serial.read() == ' ') {
      StopProgram();
      return;
    }
  }
}

int GetSonarDistance(byte trig_pin, byte echo_pin) {
  unsigned long duration, distance;
  // ensure LOW at Trig pin
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  // "start" sonar
  digitalWrite(trig_pin, HIGH);
  // wait 10 μs
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  // check pulse duration at Echo pin
  duration = pulseIn(echo_pin, HIGH);
  // calculate distance
  distance = duration / 58;

  LOGi(duration);
  LOG(" ");
  LOGi(distance);
  LOGn(" cm");
  delay(100);

  return distance;
}

void loop() {
  if (go) {
    CheckSerial();
    Interpret();
    if (GetSonarDistance(TRIG_1_PIN, ECHO_1_PIN) <= 5 ||
        GetSonarDistance(TRIG_2_PIN, ECHO_2_PIN) <= 5 ||
        GetSonarDistance(TRIG_3_PIN, ECHO_3_PIN) <= 5 ||
        GetSonarDistance(TRIG_4_PIN, ECHO_4_PIN) <= 5) {
      StopProgram();
      ClrScr();
      PrintBump();
      delay(5000);
      Redraw();
    }
  }
  else if (lowbat) {
    ClrScr();
    PrintLowbatText();
    PrintLowbat();
    delay(1000);
  }
  else {
    Redraw();
    Edit();
    CheckBat();
  }
  /*int d1 = GetSonarDistance(TRIG_1_PIN, ECHO_1_PIN); // ok
  int d2 = GetSonarDistance(TRIG_2_PIN, ECHO_2_PIN); // don't work
  int d3 = GetSonarDistance(TRIG_3_PIN, ECHO_3_PIN); // ok
  int d4 = GetSonarDistance(TRIG_4_PIN, ECHO_4_PIN);*/ // ok

  LOGn("");
}

