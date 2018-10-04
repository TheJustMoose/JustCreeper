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

#include "consts.h"
// Standard Arduino LCD library
#include <LiquidCrystal.h>

/*
  LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
*/

LiquidCrystal lcd(41, 40, 39,
                  37, 36, 35, 34,
                  33, 32, 31, 30);


// Just 8 user symbols available.
//
const uint8_t left_ascii = 0;
const uint8_t right_ascii = 1;
const uint8_t up_ascii = 2;
const uint8_t down_ascii = 3;

const uint8_t bat40_ascii = 4;
const uint8_t bat60_ascii = 5;
const uint8_t bat80_ascii = 6;
const uint8_t bat100_ascii = 7;

byte up_glyph[8] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00100,
  B00000
};

byte down_glyph[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
  B00000
};

byte left_glyph[8] = {
  B00100,
  B01000,
  B11111,
  B01001,
  B00101,
  B00001,
  B00001,
  B00000
};

byte right_glyph[8] = {
  B00100,
  B00010,
  B11111,
  B10010,
  B10100,
  B10000,
  B10000,
  B00000
};

byte bat40_glyph[8] = {
  B01110,
  B01010,
  B11111,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111
};

byte bat60_glyph[8] = {
  B01110,
  B01010,
  B11111,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111
};

byte bat80_glyph[8] = {
  B01110,
  B01010,
  B11111,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111
};

byte bat100_glyph[8] = {
  B01110,
  B01010,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void CreateSyms() {
  lcd.createChar(left_ascii, left_glyph);
  lcd.createChar(right_ascii, right_glyph);
  lcd.createChar(up_ascii, up_glyph);
  lcd.createChar(down_ascii, down_glyph);

  lcd.createChar(bat40_ascii, bat40_glyph);
  lcd.createChar(bat60_ascii, bat60_glyph);
  lcd.createChar(bat80_ascii, bat80_glyph);
  lcd.createChar(bat100_ascii, bat100_glyph);
}

void PrintCmd(CmdType cmd, int cnt) {
  switch (cmd) {
    case CLEFT: lcd.write(left_ascii); break;
    case CRIGHT: lcd.write(right_ascii); break;
    case CFWD: lcd.write(up_ascii); break;
    case CBWD: lcd.write(down_ascii); break;
    case CPAUSE: lcd.write('P'); break;

    case COUT: lcd.write('_'); break;
    case CFIRE: lcd.write('F'); break;
    case CREPEAT: lcd.write('x'); break;
  }
  if (cnt > 0)
    lcd.print(cnt);
  else
    lcd.print("-");
}

void PrintBanner() {
  lcd.begin(20,4);
  lcd.setCursor(0, 0);
  lcd.print(F("Just Creeper!"));
  lcd.setCursor(0, 1);
  lcd.print(F("Edit or Test"));
}

void PrintGo() {
  ClrScr();
  lcd.setCursor(0, 0);
  lcd.print(F("Running!"));
  delay(100);
}

void PrintFull() {
  lcd.setCursor(0, 0);
  lcd.print(F("No space left!"));
}

void PrintLoopTooLength() {
  lcd.setCursor(0, 0);
  lcd.print(F("Too length!"));
}

void PrintDeleteAll() {
  ClrScr();
  lcd.setCursor(0, 0);
  lcd.print(F("DELETE ALL?!! PRESS GO."));
}

void PrintLowbatText() {
  lcd.setCursor(4, 1);
  lcd.print(F("Low Battery!"));
}

void PrintLowbat() {
  lcd.setCursor(19, 0);
  lcd.print(F("0"));
}

void PrintBat40() {
  lcd.setCursor(19, 0);
  lcd.write(bat40_ascii);
}

void PrintBat60() {
  lcd.setCursor(19, 0);
  lcd.write(bat60_ascii);
}

void PrintBat80() {
  lcd.setCursor(19, 0);
  lcd.write(bat80_ascii);
}

void PrintGoodbat() {
  lcd.setCursor(19, 0);
  lcd.write(bat100_ascii);
}

void PrintBump() {
  lcd.setCursor(4, 1);
  lcd.print(F("BUMP!"));
}

void PrintError() {
  lcd.print(F("ERROR!"));
}

void PrintDbg(int val) {
  lcd.setCursor(0, 0);
  lcd.print(F("    "));
  lcd.setCursor(0, 0);
  lcd.print(val);
}

void ClrScr() {
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    for (int j = 0; j < 20; j++)
      lcd.print(F(" "));
  }
}

void CursorTo(int x, int y) {
  lcd.setCursor(x, y);
}

