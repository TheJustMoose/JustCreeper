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

Control keyboard

scan codes:
--------------------------
   8      16 20 24   28 32
3  7  11  15 19 23   27 31
   6      14 18 22   26 30
1  5   9     17      25 29
--------------------------

   Cmd       Cnt     Ctrl
--------------------------
   /\      1  2  3   BS DE
<  P   >   4  5  6   T1 T2
   \/      7  8  9   M1 M2
Ou Fi Lp      0      TE GO
--------------------------
P  - Pause
Ou - Out
Fi - Fire
Lp - Loop

BS - Backspace (remove last step)
DE - Delete (remove program at all)
T1 - Test 1 (forward, fire, backward)
T2 - Reserved
M1, M2 - Button to store current program in EEPROM!

left/right == not degree but minutes arrow :)

Internal value of buttons:
0-9 - ok
a - left
d - right
w - forward
s - backward
p - pause
_ - out

! - remove all!
f - fire
c - cancel last step
x - repeat
t - test
? - check
g - go

*/

// keyboard last scan code
int prev = 0;

void SetMask(int val) {
  KBD_DIRECTION = val;
  KBD_PORT = val ^ 0xFF; // Active-Low kbd pins (output!)
}

byte ReadKbdLines() {
  if (digitalRead(KBD_LINE_1) == LOW)
    return 1;
  else if (digitalRead(KBD_LINE_2) == LOW)
    return 2;
  else if (digitalRead(KBD_LINE_3) == LOW)
    return 3;
  else if (digitalRead(KBD_LINE_4) == LOW)
    return 4;

  return 0;
}

int GetScanCode() {
  int res = 0;
  for (int i = 0; i < 8; i++) { // pins 22..29
    SetMask(1 << i);
    delay(1);
    byte l1 = ReadKbdLines();
    delay(1);
    byte l2 = ReadKbdLines();
    if (l1 != 0 && l1 == l2) {
      res = l1  + i*4;
      break;
    }
  }

  if (res == prev)
    return 0;

  prev = res;
  return res;
}

boolean IsNumericBtn(int c) {
  return c == 5 || // zero btn!
         BtnToDigit(c) > 0;
}

int BtnToDigit(int c) {
  switch (c) {
    case  3: return 1;
    case  7: return 2;
    case 15: return 3;
    case  4: return 4;
    case  8: return 5;
    case 16: return 6;
    case  2: return 7;
    case  6: return 8;
    case 14: return 9;
    case  5: return 0;
    default: return 0;
  }
}

