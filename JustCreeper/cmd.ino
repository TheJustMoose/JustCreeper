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

  Command list routins

*/

#include "cmd.h"

Command::Command()
  : cmd(0) {
}

Command::Command(CmdType cmdtype, int cnt) {
  UINT t = cmdtype;
  t <<= 12;
  t |= cnt;
  cmd = t;
}

void Command::Clear() {
  cmd = 0;
}

boolean Command::IsValid() {
  return cmd != 0;
}

CmdType Command::Cmd() {
  return CmdType(cmd >> 12);
}

int Command::Cnt() {
  return cmd & 0xFFF;
}

void Command::Log() {
  switch (Cmd()) {
    case CNOP: LOG("nop!"); break;
    case CLEFT: LOG("LEFT"); break;
    case CRIGHT: LOG("RIGHT"); break;
    case CFWD: LOG("FORWARD"); break;
    case CBWD: LOG("BACKWARD"); break;
    case CPAUSE: LOG("PAUSE"); break;
    case COUT: LOG("OUT"); break;
    case CFIRE: LOG("FIRE"); break;
    case CREPEAT: LOG("REPEAT"); break;
    default: LOG("unknown cmd: "); LOGi(cmd);
  }

  LOG(" ");
  LOGin(Cnt());
}

bool Command::IsCmdExecutable(CmdType type) {
  switch (type) {
    case CNOP:
    case CLEFT:
    case CRIGHT:
    case CFWD:
    case CBWD:
    case CPAUSE:
    case COUT:
    case CFIRE:
    case CREPEAT: return true;
    default: return false;
  }
}


CommandList::CommandList()
  : len(0) {
}

void CommandList::Clear() {
  for (int i = 0; i < csize; i++)
    cmds[i].Clear();
  len = 0;
}

void CommandList::Push(Command cmd) {
  if (len >= csize) {
    PrintFull();
    return;
  }

  cmds[len] = cmd;
  len++;
}

void CommandList::Push(CmdType cmdtype, int cnt) {
  if (cnt > 20)
    cnt = 20;

  Push(Command(cmdtype, cnt));

  LOG("push idx: ");
  LOGin(len);
}

void CommandList::EraseLast() {
  if (len == 0)
    return;
  len--;
  cmds[len].Clear();
}

Command CommandList::Get(int pos) {
  if (pos >= 0 && pos < len)
    return cmds[pos];
  else
    return Command(CNOP, 0);
}

int CommandList::Len() {
  return len;
}
