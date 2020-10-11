#ifndef C0C_GENERATOR_H
#define C0C_GENERATOR_H

#include "ast.h"
#include "mips_isa.h"
#include "visitor.h"

#include <map>
#include <vector>

struct DataSegEntry;
class Parser;
class QuadGenerator;
class QuadAddr;
struct Frame;
class FuncInfo;
class Quadruple;

using TypeList = std::vector<Type *>;
using DataSegEntryList = std::vector<DataSegEntry>;
using DataSegTab = std::vector<DataSegEntry *>;
using IdentTab = std::map<Identifier *, QuadAddr *>;

class Generator {
public:
  Generator(Parser *parser = nullptr, FILE *out = nullptr)
    : m_parser(parser), m_outstream(out)
  {
  }
  void SetInOut(Parser *parser, FILE *outFile)
  {
    m_parser = parser;
    m_outstream = outFile;
  }

protected:
  void Emit(const std::string &str, unsigned indent = 4)
  {
    for (; indent > 0; indent = indent - 4) {
      fprintf(m_outstream, "    ");
      // debug("    ");
    }
    fprintf(m_outstream, "%s\n", str.c_str());
    // debug("%s\n", str.c_str());
  }
  void EmitComment(const char *format, ...);

  void EmitBlankLine()
  {
    fputc('\n', m_outstream);
    // debug("\n");
  }

  void Emit(const std::string &inst, const std::string &dst)
  {
    fprintf(m_outstream, "    %-12s%s\n", inst.c_str(), dst.c_str());
    // debug("    %-12s%s\n", inst.c_str(), dst.c_str());
  }
  void Emit(const std::string &inst, Gpr dst)
  {
    Emit(inst, regs[dst]);
  }

  void
  Emit(const std::string &inst, const std::string &dst, const std::string &src)
  {
    fprintf(m_outstream, "    %-12s%s, %s\n", inst.c_str(), dst.c_str(),
            src.c_str());
    // debug("    %-12s%s, %s\n", inst.c_str(), dst.c_str(), src.c_str());
  }
  void Emit(const std::string &inst, Gpr dst, Gpr src)
  {
    Emit(inst, regs[dst], regs[src]);
  }
  void Emit(const std::string &inst, const std::string &dst, Gpr src)
  {
    Emit(inst, dst, regs[src]);
  }
  void Emit(const std::string &inst, Gpr dst, const std::string &src)
  {
    Emit(inst, regs[dst], src);
  }
  void Emit(const std::string &inst, Gpr dst, long imm)
  {
    Emit(inst, regs[dst], std::to_string(imm));
  }
  void Emit(const std::string &inst, const std::string &dst, long imm)
  {
    Emit(inst, dst, std::to_string(imm));
  }

  void Emit(const std::string &inst,
            const std::string &dst,
            const std::string &src1,
            const std::string &src2)
  {
    fprintf(m_outstream, "    %-12s%s, %s, %s\n", inst.c_str(), dst.c_str(),
            src1.c_str(), src2.c_str());
    // debug("    %-12s%s, %s, %s\n", inst.c_str(), dst.c_str(),
    // src1.c_str(),
    //       src2.c_str());
  }
  void Emit(const std::string &inst, Gpr src1, LabelStmt *label)
  {
    Emit(inst, regs[src1], label->Repr());
  }
  void Emit(const std::string &inst, Gpr src1, Gpr src2, LabelStmt *label)
  {
    Emit(inst, regs[src1], regs[src2], label->Repr());
  }
  void Emit(const std::string &inst, Gpr dst, Gpr src1, Gpr src2)
  {
    Emit(inst, regs[dst], regs[src1], regs[src2]);
  }
  void Emit(const std::string &inst, Gpr dst, Gpr src, long imm)
  {
    Emit(inst, regs[dst], regs[src], std::to_string(imm));
  }

protected:
  Parser *m_parser;
  FILE *m_outstream;
};

class CodeGenerator : public Generator {
public:
  CodeGenerator(Parser *parser = nullptr,
                QuadGenerator *qg = nullptr,
                FILE *out = nullptr)
    : Generator(parser, out), m_qg(qg)
  {
  }
  void SetQuadGen(QuadGenerator *qg)
  {
    m_qg = qg;
  }

  void Gen();
  void TestGen();

protected:
  Gpr VisitQuadAddr(QuadAddr *qa);
  void GenPrologue(Frame &frame);
  void GenEpilogue(Frame &frame);
  void GenFunc(FuncInfo *func_info);
  void Gen(IdentTab &idtab);
  // Binary
  // void GenCommaOp(BinaryOp *comma);
  // void GenMemberRefOp(BinaryOp *binaryOp);
  // void GenAndOp(BinaryOp *binaryOp);
  // void GenOrOp(BinaryOp *binaryOp);
  // void GenAddOp(BinaryOp *binaryOp);
  // void GenSubOp(BinaryOp *binaryOp);
  // void GenCastOp(UnaryOp *cast);
  // void GenDerefOp(UnaryOp *deref);
  void GenAssign(QuadAddr *dst, QuadAddr *arg1);
  void GenAdd(Quadruple *quad);
  void GenSub(Quadruple *quad);
  void GenDiv(Quadruple *quad);
  void GenMult(Quadruple *quad);

  void GenPush(Quadruple *quad);
  void GenCall(Quadruple *quad);
  void GenReturn(Quadruple *quad);

  // void GenCompOp(int width, bool flt, const char *set);
  // void GenCompZero(Type *type);

  // // Unary
  // void GenIncDec(Expr *operand, bool postfix, const std::string &inst);
  // void GenStaticDecl(Decl *decl);

  // void GenSaveArea();
  // void GenBuiltin(CallExpr *CallExpr);

  enum { D_TEXT, D_DATA };
  void EmitDirective(int dir);
  void EmitLabel(const std::string &label);
  void EmitLoad(const std::string &addr, Type *type);
  void EmitLoad(const std::string &addr, int width, bool flt);
  void EmitStore(const std::string &addr, Type *type);
  void EmitStore(Gpr src, const std::string &label)
  {
    Emit("sw", regs[src], label);
  }
  void EmitStore(Gpr src, Gpr base, int imm = 0)
  {
    Emit("sw", regs[src], std::to_string(imm) + "(" + regs[base] + ")");
  }
  void EmitStore(Gpr src, Gpr base, const std::string &label)
  {
    Emit("sw", regs[src], label + "(" + regs[base] + ")");
  }

  void EmitLoad(Gpr dst, const std::string &label)
  {
    Emit("lw", regs[dst], label);
  }
  void EmitLoad(Gpr dst, Gpr base, int imm = 0)
  {
    Emit("lw", regs[dst], std::to_string(imm) + "(" + regs[base] + ")");
  }
  void EmitLoad(Gpr dst, Gpr base, const std::string &label)
  {
    Emit("lw", regs[dst], label + "(" + regs[base] + ")");
  }
  //   void EmitLoadBitField(const std::string& addr, Object* bitField);
  // void EmitStoreBitField(const ObjectAddr &addr, Type *type);

  // sw      reg, 0($sp)
  // addi    $sp, $sp, -4
  void EmitPush(Gpr reg);

  // addi    $sp, $sp, 4
  // lw      reg, 0($sp)
  void EmitPop(Gpr reg);

  // exit(10), ...
  void EmitSyscall(unsigned service_no);

  // print_int(), print_str(), ...
  void EmitSyscall(unsigned service_no, const std::string &arg0);

  void EmitPrint(StringLiteral *sl);
  void EmitPrint(IntegerLiteral *il);
  void EmitPrint(Identifier *ident);  // TODO: opt
  void EmitPrint(Expr *ident) {}      // TODO: opt
  void EmitPrint(const std::string &arg, int sysn);
  void EmitPrint(QuadAddr *qa);
  void EmitPrint(Gpr reg, int sysn);

  void EmitScanf(Identifier *ident);  // TODO: opt
  void EmitScanf(QuadAddr *qa);

  void EmitQuad(Quadruple *quad);

  QuadGenerator *m_qg;
  FuncInfo *m_curfunc;

protected:
  static DataSegEntryList data_entries;
  static int offset_;
  static FunctionDecl *curFunc_;
};

class LValGenerator : public CodeGenerator {
public:
  LValGenerator() {}

  // Expression
  virtual void VisitBinaryOp(BinaryOp *binaryOp);
  virtual void VisitUnaryOp(UnaryOp *unaryOp);
  virtual void VisitIdentifier(Identifier *ident);

  virtual void VisitCallExpr(CallExpr *CallExpr)
  {
    assert(false);
  }
  virtual void VisitTempVar(TempVar *tempVar);
};

#endif  // C0C_GENERATOR_H
