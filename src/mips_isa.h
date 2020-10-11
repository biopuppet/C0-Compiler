#ifndef C0C_MIPS_ISA_H
#define C0C_MIPS_ISA_H

#include <vector>

extern std::vector<const char *> regs;

enum Gpr {
  zero = 0,
  at = 1,
  v0 = 2,
  v1 = 3,
  a0 = 4,
  a1 = 5,
  a2,
  a3,
  t0,
  t1,
  t2,
  t3,
  t4,
  t5,
  t6,
  t7,
  s0,
  s1,
  s2,
  s3,
  s4,
  s5,
  s6,
  s7,
  t8,
  t9,
  k0,
  k1,
  gp,
  sp,
  fp,
  ra,
  pc = 32,
  hi = 33,
  lo = 34
};

enum SyscallCode {
  SC_PRINT_INT = 1,
  SC_PRINT_STR = 4,
  SC_READ_INT = 5,
  SC_READ_STR = 8,
  SC_EXIT = 10,
  SC_PRINT_CHAR = 11,
  SC_READ_CHAR = 12,
};

class QuadAddr;
struct Reg {
  Gpr m_name;
  int m_val{0};
  QuadAddr *m_bind{nullptr};
  Reg(Gpr name, int val, QuadAddr *qa) : m_name(name), m_val(val), m_bind(qa) {}
};

using RegNameList = std::vector<const char *>;

// extern struct Reg RegPool[8];

#endif  // !C0C_MIPS_ISA_H
