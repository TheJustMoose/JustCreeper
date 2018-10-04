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
#ifndef JUST_CREEPER_CMD_H
#define JUST_CREEPER_CMD_H

#include "macros.h"

enum CmdType {CNOP = 0, CLEFT, CRIGHT, CFWD, CBWD, CPAUSE, COUT, CFIRE, CREPEAT};
const int csize = 16;

class Command {
private:
  UINT cmd;

public:
  Command();
  Command(CmdType cmdtype, int cnt);

  void Clear();
  boolean IsValid();
  CmdType Cmd();
  int Cnt();
  void Log();

  static bool IsCmdExecutable(CmdType);
};

class CommandList {
private:
  Command cmds[csize];
  int len;

public:
  CommandList();
  void Clear();
  void Push(Command cmd);
  void Push(CmdType cmd, int cnt);
  void EraseLast();

  Command Get(int pos);
  int Len();
};

#endif

