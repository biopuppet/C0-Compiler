#include "quad_generator.h"
#include "debug.h"
#include "generator.h"
#include "mips_isa.h"
#include "parser.h"

class Quadruple;
class QuadAddr;
static MemoryPoolImp<QuadAddr> quadAddrPool;
static MemoryPoolImp<Quadruple> quadPool;
static MemoryPoolImp<DataSegEntry> dataSegEntryPool;

std::string DataSegEntry::Repr()
{
  auto entry_class = m_class == EC_WORD ?
                       ".word" :
                       m_class == EC_SPACE ?
                       ".space" :
                       m_class == EC_ASCIIZ ? ".asciiz" : "error entry class!";
  return m_ident->Name() + ": " + entry_class + " " + std::to_string(m_val) +
         " : " + std::to_string(m_num);
}

DataSegEntry *DataSegEntry::New(Identifier *ident,
                                enum EntryClass ec,
                                long val,
                                unsigned num,
                                int align)
{
  auto ret =
    new (dataSegEntryPool.Alloc()) DataSegEntry(ident, ec, val, num, align);
  ret->m_pool = &dataSegEntryPool;
  return ret;
}

DataSegEntry *DataSegEntry::New(Identifier *ident, StringLiteral *val)
{
  auto ret = new (dataSegEntryPool.Alloc()) DataSegEntry(ident, val);
  ret->m_pool = &dataSegEntryPool;
  return ret;
}

QuadAddr *QuadAddr::New(AddrType type, long data, unsigned off)
{
  auto ret = new (quadAddrPool.Alloc()) QuadAddr(type, data, off);
  ret->m_pool = &quadAddrPool;
  return ret;
}

QuadAddr *
QuadAddr::New(AddrType type, Identifier *str, bool isglb, unsigned off)
{
  auto ret = new (quadAddrPool.Alloc()) QuadAddr(type, str, isglb, off);
  ret->m_pool = &quadAddrPool;
  return ret;
}

QuadAddr *
QuadAddr::New(AddrType type, QuadAddr *master, QuadAddr *minion, bool islval)
{
  auto ret = new (quadAddrPool.Alloc()) QuadAddr(type, master, minion, islval);
  ret->m_pool = &quadAddrPool;
  return ret;
}

QuadAddr *QuadAddr::New(AddrType type, StringLiteral *str, unsigned off)
{
  auto ret = new (quadAddrPool.Alloc()) QuadAddr(type, str, off);
  ret->m_pool = &quadAddrPool;
  return ret;
}

QuadAddr *QuadAddr::New(AddrType type, LabelStmt *data)
{
  auto ret = new (quadAddrPool.Alloc()) QuadAddr(type, data);
  ret->m_pool = &quadAddrPool;
  return ret;
}

QuadAddr *QuadAddr::New(AddrType type, FunctionDecl *data)
{
  auto ret = new (quadAddrPool.Alloc()) QuadAddr(type, data);
  ret->m_pool = &quadAddrPool;
  return ret;
}

Quadruple *
Quadruple::New(QuadOp op, QuadAddr *dst, QuadAddr *arg1, QuadAddr *arg2)
{
  auto ret = new (quadPool.Alloc()) Quadruple(op, dst, arg1, arg2);
  ret->m_pool = &quadPool;
  return ret;
}

FuncInfo::FuncInfo(FunctionDecl *func) : m_func(func)
{
  m_frame = {0, 0x00000000, 0};
  m_entry_label = "$func_" + func->Name() + "_entry";
  m_exit_label = "$func_" + func->Name() + "_exit";
}

FuncInfo::FuncInfo(FunctionDecl *func, Frame &frame)
  : m_frame(frame), m_func(func)
{
  m_entry_label = "$func_" + func->Name() + "_entry";
  m_exit_label = "$func_" + func->Name() + "_exit";
}

void FuncInfo::Add(Quadruple *quad)
{
  assert(quad);
  m_quads.push_back(quad);
}

QuadAddr *FuncInfo::Find(Identifier *ident)
{
  auto quad = m_qamap.find(ident);
  if (quad != m_qamap.end())
    return quad->second;
  return nullptr;
}

std::string FuncInfo::Name()
{
  if (m_func) {
    return m_func->Name();
  }
  else {
    return "unknown";
  }
}

QuadGenerator::QuadGenerator(Parser *parser, FILE *out)
  : Generator(parser, out), m_tempid(0), m_curfunc(nullptr), m_curoffset(0)
{
  if (parser)
    m_glbscope = m_curscope = parser->Unit()->Scope();  // global
  else {
    m_glbscope = m_curscope = nullptr;
  }
}

void QuadGenerator::VisitArraySubscriptExpr(ArraySubscriptExpr *ase)
{
  debug("Visiting Array Subscript Op\n");
  EmitComment("Visiting Array Subscript Op (lval:%d) ", ase->IsLVal());

  Visit(ase->m_lhs);
  auto lhs = m_curqa;
  Visit(ase->m_rhs);
  auto rhs = m_curqa;

  lhs = QuadAddr::New(QuadAddr::AT_ARRAY, lhs, rhs, ase->IsLVal());

  if (ase->IsLVal()) {
    m_curqa = lhs;
  }
  else {
    m_curqa = ase->IsChar() ? NewTempChar() : NewTemp();
    NewQuad(QO_ASSIGN, m_curqa, lhs);
  }
}

void QuadGenerator::VisitBinaryOp(BinaryOp *binary)
{
  debug("Visiting BinaryOp: %s\n", tokenlist[binary->m_op]);
  EmitComment("Visiting BinaryOp: %s", tokenlist[binary->m_op]);

#ifdef CONST_PROPAGATION
  if (binary->IsConst() && binary->m_lhs->IsConst() &&
      binary->m_rhs->IsConst()) {
    m_curqa = QuadAddr::New(QuadAddr::AT_INTL, binary->m_val);
    EmitComment("combining %d %s %d to %d", binary->m_lhs->m_val,
                tokenlist[binary->m_op], binary->m_rhs->m_val, binary->m_val);
    return;
  }
#endif  // CONST_PROPAGATION

  Visit(binary->m_rhs);
  auto rhs = m_curqa;

  Visit(binary->m_lhs);
  auto lhs = m_curqa;

  Quadruple *quad = nullptr;
  switch (binary->m_op) {
  case TokenType::PLUS:
    m_curqa = NewTemp();
    quad = NewQuad(QO_PLUS, m_curqa, lhs, rhs);
    break;
  case TokenType::MINU:
    m_curqa = NewTemp();
    quad = NewQuad(QO_MINU, m_curqa, lhs, rhs);
    break;
  case TokenType::MULT:
    m_curqa = NewTemp();
    quad = NewQuad(QO_MULT, m_curqa, lhs, rhs);
    break;
  case TokenType::DIV:
    m_curqa = NewTemp();
    quad = NewQuad(QO_DIV, m_curqa, lhs, rhs);
    break;
  case TokenType::ASSIGN: {
    // if (lhs->m_type == QuadAddr::AT_ARRAY) {
    //     quad = NewQuad(QO_ASSIGN, lhs, rhs);
    // }
    // else {
    // m_curqa = NewTemp();  // TODO delete
    quad = NewQuad(QO_ASSIGN, lhs, rhs);
    // }
    break;
  }
  case TokenType::LBRACK:
    m_curqa = NewTemp();
    quad = NewQuad(QO_INDEX_ARG, m_curqa, lhs, rhs);
    break;

  // case TokenType::LSS:
  //     m_curqa = NewLabel();
  //     quad = NewQuad(QO_BGE, m_curqa, lhs, rhs);
  //     break;
  default:
    assert(0);
    break;
  }
}

void QuadGenerator::VisitCondition(Condition *cond)
{
  debug("Visiting Condition: %s\n",
        cond->Tok() ? cond->Tok()->m_value.c_str() : "expr");

  // jump dst label
  auto label_qa = m_curqa;
  // only 1 expr
  if (!cond->IsBinary()) {
    Visit(cond->m_lhs);
    auto val = m_curqa;
    NewQuad(QO_BZ, label_qa, val);
    m_curqa = label_qa;
    return;
  }
  // <expr> <cmp op> <expr>
  Visit(cond->m_lhs);
  auto lhs = m_curqa;
  Visit(cond->m_rhs);
  auto rhs = m_curqa;

  switch (cond->m_op) {
  case TokenType::GEQ:
    NewQuad(QO_BLT, label_qa, lhs, rhs);
    break;
  case TokenType::LEQ:
    NewQuad(QO_BGT, label_qa, lhs, rhs);
    break;
  case TokenType::EQL:
    NewQuad(QO_BNE, label_qa, lhs, rhs);
    break;
  case TokenType::NEQ:
    NewQuad(QO_BEQ, label_qa, lhs, rhs);
    break;
  case TokenType::GRE:
    NewQuad(QO_BLE, label_qa, lhs, rhs);
    break;
  case TokenType::LSS:
    NewQuad(QO_BGE, label_qa, lhs, rhs);
    break;
  default:
    debug("not support cmp op yet\n");
    assert(0);
    break;
  }
  m_curqa = label_qa;
}

void QuadGenerator::VisitUnaryOp(UnaryOp *uop)
{
  debug("Visiting Uop: %s\n", uop->Tok()->m_value.c_str());
#ifdef CONST_PROPAGATION
  if (uop->IsConst() && uop->m_operand->IsConst()) {
    m_curqa = QuadAddr::New(QuadAddr::AT_INTL, uop->m_val);
    EmitComment("combining %s %d to %d", tokenlist[uop->m_op],
                uop->m_operand->m_val, uop->m_val);
    return;
  }
#endif  // CONST_PROPAGATION

  Visit(uop->m_operand);
  auto rhs = m_curqa;
  if (uop->m_op == TokenType::MINU) {
    auto zero = QuadAddr::New(QuadAddr::AT_INTL, (long)0);
    m_curqa = NewTemp();
    NewQuad(QO_MINU, m_curqa, zero, rhs);
  }
}

void QuadGenerator::VisitCallExpr(CallExpr *funcCall)
{
  debug("Visiting CallExpr\n");
  EmitComment("Visiting CallExpr\n");

  // set isleaf false
  m_curfunc->m_isleaf = false;
  m_curfunc->m_frame.mask |= 0x80000000;
  m_curfunc->m_frame.size += 4;

  // build args
  int args_build_offset = 0;
  std::vector<QuadAddr *> arglist;
  for (auto arg : funcCall->Args()) {
    Visit(arg);
    arglist.push_back(m_curqa);
    // auto qa = QuadAddr::New(QuadAddr::AT_IDENT, m_curqa);
  }
  for (auto arg : arglist) {
    auto dst = QuadAddr::New(QuadAddr::AT_OFFSET, args_build_offset);
    args_build_offset += 4;
    NewQuad(QO_PUSH, arg, dst);
  }
  // update arg building area size
  if (args_build_offset > m_curfunc->m_argbuildsz) {
    m_curfunc->m_argbuildsz = args_build_offset;
  }

  auto ident = funcCall->Designator();
  auto qa = QuadAddr::New(QuadAddr::AT_IDENT, ident);
  m_curqa = ident->IsChar() ? NewTempChar() : NewTemp();
  NewQuad(QO_CALL, m_curqa, qa);
  // Visit(ident->m_decl);
}

QuadAddr *QuadGenerator::Find(Identifier *ident)
{
  auto quad = m_curfunc->Find(ident);
  if (quad == nullptr) {
    auto res = m_glb_identtab.find(ident);
    if (res != m_curfunc->m_qamap.end())
      return res->second;
    return nullptr;
  }
  return quad;
}

void QuadGenerator::VisitIdentifier(Identifier *ident)
{
  debug("Visiting Identifier: %s\n", ident->Name().c_str());
  EmitComment("Visiting Identifier: %s", ident->Name().c_str());
  // FIXED: could be AT_OFFSET, identifier may be in the stack
  // Reversion:  is a temp var, won't work
  // Reversion: should find ident first, do not necessarily new
  // m_curqa = QuadAddr::New(QuadAddr::AT_IDENT, ident, m_curoffset);
  m_curqa = Find(ident);
  assert(m_curqa);
}

void QuadGenerator::VisitIntegerLiteral(IntegerLiteral *cons)
{
  debug("Visiting Integer Literal: %ld\n", cons->Val());
  EmitComment("Visiting Integer Literal: %ld", cons->Val());
  m_curqa = QuadAddr::New(QuadAddr::AT_INTL, cons->Val());
}

void QuadGenerator::VisitCharLiteral(CharLiteral *cons)
{
  debug("Visiting Char Literal: %u\n", cons->Val());
  EmitComment("Visiting Char Literal: %u", cons->Val());
  m_curqa = QuadAddr::New(QuadAddr::AT_CHARL, cons->Val());
}
void QuadGenerator::VisitStringLiteral(StringLiteral *cons)
{
  debug("***Visiting String Literal: %s\n", cons->Val().c_str());
  EmitComment("Visiting String Literal: %s", cons->Val().c_str());
  m_curqa = QuadAddr::New(QuadAddr::AT_STR, cons);
}
void QuadGenerator::VisitTempVar(TempVar *tempVar) {}
void QuadGenerator::VisitDecl(Decl *init) {}
void QuadGenerator::VisitDeclStmt(DeclStmt *declStmt)
{
  debug("******Visiting Decl Stmt\n");
  // m_curfunc->m_func = declStmt->GetFuncDecl();
}

// scanf <ident>
void QuadGenerator::VisitScanfStmt(ScanfStmt *scanfStmt)
{
  debug("Visiting Scanf Stmt\n");
  for (auto ident : scanfStmt->m_idents) {
    auto qa = Find(ident);
    assert(qa);
    NewQuad(QO_SCAN, qa);
  }
}

// printf [<str>][<exp>]
void QuadGenerator::VisitPrintStmt(PrintStmt *printStmt)
{
  debug("Visiting Print Stmt\n");
  EmitComment("Visiting Print Stmt");
  auto str = printStmt->Str();
  auto exp = printStmt->Exp();
  QuadAddr *exp_qa = nullptr;

  if (exp) {
    VisitExpr(exp);
    exp_qa = m_curqa;
  }
  if (str) {
    auto qa = QuadAddr::New(QuadAddr::AT_STR, str);
    NewQuad(QO_PRINT, qa);
    if (exp == nullptr) {
      str->m_sval.append("\\n");
    }
  }
  if (exp) {
    // calculate expr first
    NewQuad(QO_PRINT, exp_qa);

    auto lf = StringLiteral::New("\\n");
    auto qa = QuadAddr::New(QuadAddr::AT_STR, lf);
    NewQuad(QO_PRINT, qa);  // builtin line feed
  }
}

/*
 * if !condition goto else
 *   <then-stmt>
 *   [j else-end]
 * else:
 *   [<else-stmt>]
 * [else-end:]
 */
void QuadGenerator::VisitIfStmt(IfStmt *ifStmt)
{
  debug("Visiting If Stmt\n");
  EmitComment("Visiting If Stmt");
  assert(ifStmt->m_cond && ifStmt->m_then);

  m_curqa = NewLabel();
  Visit(ifStmt->m_cond);
  auto else_qa = m_curqa;

  Visit(ifStmt->m_then);

  // goto exit
  QuadAddr *exit_qa = nullptr;
  if (ifStmt->m_else) {
    exit_qa = NewLabel();
    NewQuad(QO_GOTO, exit_qa);
  }
  // else_label:
  NewQuad(QO_LABEL, else_qa);

  if (ifStmt->m_else) {
    Visit(ifStmt->m_else);
    NewQuad(QO_LABEL, exit_qa);
  }
}

// for (m_init_ident = m_init_expr;
//      m_cond;
//      m_inc_lhs = m_inc_rhs + m_inc)
//     m_body
void QuadGenerator::VisitForStmt(ForStmt *forStmt)
{
  debug("Visiting For Stmt\n");
  EmitComment("Visiting For Stmt");
  assert(forStmt->m_init_ident && forStmt->m_init_expr);
  assert(forStmt->m_cond);
  assert(forStmt->m_inc && forStmt->m_inc_lhs && forStmt->m_inc_rhs);
  assert(forStmt->m_body);

  // init
  Visit(forStmt->m_init_ident);
  auto init_ident_qa = m_curqa;
  Visit(forStmt->m_init_expr);
  auto init_expr_qa = m_curqa;
  NewQuad(QO_ASSIGN, init_ident_qa, init_expr_qa);

  // for begin
  auto for_begin = NewLabel();
  NewQuad(QO_LABEL, for_begin);

  m_curqa = NewLabel();
  Visit(forStmt->m_cond);
  auto for_end = m_curqa;

  // for body
  auto for_body = NewLabel();
  NewQuad(QO_LABEL, for_body);

  Visit(forStmt->m_body);

  Visit(forStmt->m_inc_rhs);
  auto inc_rhs_qa = m_curqa;
  Visit(forStmt->m_inc);
  auto inc_qa = m_curqa;

  Visit(forStmt->m_inc_lhs);
  // auto inc_lhs_qa = m_curqa;
  // auto inc_res = NewTemp();
  NewQuad(QO_PLUS, m_curqa, inc_rhs_qa, inc_qa);

  // NewQuad(QO_ASSIGN, inc_lhs_qa, inc_res);
  // NewQuad(QO_GOTO, for_begin);
  m_curqa = for_body;
  Visit(forStmt->m_cond);
  // reverse condition
  auto temp = m_curfunc->m_quads.back()->m_op * -1;
  m_curfunc->m_quads.back()->m_op = (QuadOp)temp;

  NewQuad(QO_LABEL, for_end);
}

/*
 * while_begin:
 *   if !condition goto while_end
 *   body
 *   goto while_begin
 * while_end:
 */
void QuadGenerator::VisitWhileStmt(WhileStmt *whileStmt)
{
  debug("Visiting While Stmt\n");
  EmitComment("Visiting While Stmt");
  assert(whileStmt->m_cond && whileStmt->m_body);

  // while begin
  auto while_begin = NewLabel();
  NewQuad(QO_LABEL, while_begin);

  m_curqa = NewLabel();
  Visit(whileStmt->m_cond);
  auto while_end = m_curqa;

  // while body
  auto while_body = NewLabel();
  NewQuad(QO_LABEL, while_body);

  Visit(whileStmt->m_body);

  m_curqa = while_body;
  Visit(whileStmt->m_cond);
  auto temp = m_curfunc->m_quads.back()->m_op * -1;
  m_curfunc->m_quads.back()->m_op = (QuadOp)temp;
  // NewQuad(QO_GOTO, while_begin);
  NewQuad(QO_LABEL, while_end);
}

/*
 * do_begin:
 *   body
 *   if condition goto do_begin
 * [do_end:]
 */
void QuadGenerator::VisitDoStmt(DoStmt *doStmt)
{
  debug("Visiting do Stmt\n");
  EmitComment("Visiting do Stmt");

  auto do_begin = NewLabel();
  NewQuad(QO_LABEL, do_begin);

  Visit(doStmt->m_body);

  m_curqa = do_begin;
  Visit(doStmt->m_cond);
  auto temp = m_curfunc->m_quads.back()->m_op * -1;
  m_curfunc->m_quads.back()->m_op = (QuadOp)temp;

  // auto do_end = NewLabel();
  // NewQuad(QO_LABEL, do_end);
}

void QuadGenerator::VisitJumpStmt(JumpStmt *jumpStmt) {}
void QuadGenerator::VisitReturnStmt(ReturnStmt *returnStmt)
{
  debug("Visiting Return Stmt\n");
  EmitComment("Visiting Return Stmt");
  if (returnStmt->m_expr) {
    VisitExpr(returnStmt->m_expr);
  }
  else {
    m_curqa = nullptr;
  }
  auto ls = LabelStmt::New(m_curfunc->m_exit_label);
  auto ret_label = QuadAddr::New(QuadAddr::AT_LABEL, ls);
  NewQuad(QO_RETURN, m_curqa, ret_label);
}
void QuadGenerator::VisitLabelStmt(LabelStmt *labelStmt) {}
void QuadGenerator::VisitNullStmt(NullStmt *emptyStmt) {}
void QuadGenerator::VisitCompoundStmt(CompoundStmt *compStmt)
{
  debug("Visiting Compound Stmt size: %zu\n", compStmt->Size());
  EmitComment("Visiting Compound Stmt size: %zu", compStmt->Size());
  for (auto decl : compStmt->m_decls) {
    assert(decl);
    Visit(decl);
  }
  for (auto stmt : compStmt->m_stmts) {
    assert(stmt);
    Visit(stmt);
  }
}

static void mapoffset(QuadAddr *qa, int frame_size, int save_size)
{
  if (!qa) {
    debug("warning: mapping nullptr!\n");
    return;
  }
  if (qa->m_resolved == false) {
    qa->m_offset = frame_size - save_size - qa->m_offset;
    qa->m_resolved = true;
  }
}

void QuadGenerator::VisitFunctionDecl(FunctionDecl *funcDecl)
{
  debug("Visiting Function Decl %s\n", funcDecl->Name().c_str());
  EmitComment("Visiting Function Decl %s", funcDecl->Name().c_str());

  m_curfunc = new FuncInfo(funcDecl);
  m_tempid = 0;
  m_curfunc->m_argbuildsz = 0;
  // m_curoffset = 0;

  Emit("define " + funcDecl->QuadStr() + "{", 0);
  for (auto param : funcDecl->m_params) {
    Visit(param);
  }

  assert(funcDecl->Body());
  Visit(funcDecl->Body());

  auto save_size = m_curfunc->m_frame.size;
  m_curfunc->m_frame.size += m_curoffset + m_curfunc->m_argbuildsz;  // opt

  Emit("}", 0);
  Emit(".frame " + std::to_string(m_curfunc->m_frame.size));
  debug(".frame %d (40 + %d + %d)\n", m_curfunc->m_frame.size, m_curoffset,
        m_curfunc->m_argbuildsz);

  // resolve curfunc temp & local vars offset
  for (auto slot : m_curfunc->m_quads) {
    if (slot->m_op == QO_PARAM) {
      if (slot->m_dst->m_resolved == false) {
        slot->m_dst->m_offset = m_curfunc->m_frame.size + slot->m_dst->m_offset;
        slot->m_dst->m_resolved = true;
      }
      continue;
    }
    if (slot->m_dst) {
      if (slot->m_dst->m_type == QuadAddr::AT_ARRAY) {
        // if (slot->m_dst->m_minion->m_type )
        mapoffset(slot->m_dst->m_minion, m_curfunc->m_frame.size, save_size);
      }
      if (slot->m_dst->m_resolved == false) {
        slot->m_dst->m_offset =
          m_curfunc->m_frame.size - save_size - slot->m_dst->m_offset;
        slot->m_dst->m_resolved = true;
      }
    }
    if (slot->m_arg1) {
      if (slot->m_arg1->m_type == QuadAddr::AT_ARRAY) {
        mapoffset(slot->m_arg1->m_minion, m_curfunc->m_frame.size, save_size);
      }
      if (slot->m_arg1->m_resolved == false) {
        slot->m_arg1->m_offset =
          m_curfunc->m_frame.size - save_size - slot->m_arg1->m_offset;
        slot->m_arg1->m_resolved = true;
      }
    }
    if (slot->m_arg2) {
      if (slot->m_arg2->m_resolved == false) {
        slot->m_arg2->m_offset =
          m_curfunc->m_frame.size - save_size - slot->m_arg2->m_offset;
        slot->m_arg2->m_resolved = true;
      }
    }
  }

  m_funcs.push_back(m_curfunc);
}

void QuadGenerator::VisitVarDecl(VarDecl *varDecl)
{
  debug("Visiting VarDecl %s\n", varDecl->Name().c_str());
  EmitComment("Visiting VarDecl %s", varDecl->Name().c_str());

  if (m_curfunc) {
    m_curoffset += varDecl->Width();
    auto qa =
      QuadAddr::New(QuadAddr::AT_IDENT, varDecl->m_name, false, m_curoffset);
    m_curfunc->m_qamap[varDecl->m_name] = qa;
    auto reg = m_curfunc->AllocReg(qa);
    debug("%s get reg %s", qa->Str().c_str(), regs[reg]);

    // m_curfunc->m_frame.size += varDecl->Width();
    Emit(varDecl->QuadStr() + "(" + std::to_string(qa->m_offset) + ")");
    // local const need to be initialized
    if (varDecl->IsConstQualified()) {
      auto type = varDecl->IsChar() ? QuadAddr::AT_CHARL : QuadAddr::AT_INTL;
      auto init_val = QuadAddr::New(type, varDecl->GetVal());
      NewQuad(QO_ASSIGN, qa, init_val);
    }
  }

  else if (m_curfunc == nullptr) {  // global scope
    auto entry = DataSegEntry::New(varDecl->m_name, DataSegEntry::EC_WORD,
                                   varDecl->GetVal(), varDecl->Num());
    m_data_entries.push_back(entry);
  }
}

void QuadGenerator::VisitParmVarDecl(ParmVarDecl *parmVarDecl)
{
  debug("Visiting Param VarDecl: %s\n", parmVarDecl->Name().c_str());
  EmitComment("Visiting Param VarDecl: %s", parmVarDecl->Name().c_str());

  m_curqa =
    QuadAddr::New(QuadAddr::AT_IDENT, parmVarDecl->m_name, false, m_curoffset);
  auto reg = m_curfunc->AllocReg(m_curqa);
  debug("%s get reg %s", m_curqa->Str().c_str(), regs[reg]);

  NewQuad(QO_PARAM, m_curqa);
  // Emit(parmVarDecl->QuadStr() + "(" + std::to_string(qa->m_offset) + ")");

  if (m_curfunc) {
    m_curoffset += 4;
    m_curfunc->m_qamap[parmVarDecl->m_name] = m_curqa;
  }
}

void QuadGenerator::VisitTranslationUnitDecl(TranslationUnitDecl *unit)
{
  debug("visiting tu, total decls: %d\n", unit->ExtDecls().size());

  for (auto var : unit->VarDecls()) {
    // global var don't need `offset`
    auto qa = QuadAddr::New(QuadAddr::AT_IDENT, var->m_name, true);
    m_glb_identtab[var->m_name] = qa;

    assert(m_curfunc == nullptr);
    Visit(var);
  }
  assert(m_curoffset == 0);
  for (auto extDecl : unit->ExtDecls()) {
    Visit(extDecl);
    m_curoffset = 0;
    // m_curqa = nullptr;
  }
}

void QuadGenerator::Gen()
{
  Emit("########## C0 Intermediate Code generated by c0c ##########", 0);
  VisitTranslationUnitDecl(m_parser->Unit());
}

Quadruple *
QuadGenerator::NewQuad(QuadOp op, QuadAddr *dst, QuadAddr *arg1, QuadAddr *arg2)
{
  auto quad = Quadruple::New(op, dst, arg1, arg2);
  m_curfunc->Add(quad);
  EmitQuad(quad);
  return quad;
}

QuadAddr *QuadGenerator::NewTemp()
{
  m_curoffset += 4;
  auto qa = QuadAddr::New(QuadAddr::AT_TMP, NextTemp(), m_curoffset);
  return qa;
}
QuadAddr *QuadGenerator::NewTempChar()
{
  m_curoffset += 4;
  auto qa = QuadAddr::New(QuadAddr::AT_TMPCH, NextTemp(), m_curoffset);
  return qa;
}

QuadAddr *QuadGenerator::NewLabel()
{
  auto qa = QuadAddr::New(QuadAddr::AT_LABEL, LabelStmt::New());
  return qa;
}

std::string QuadAddr::Str()
{
  std::string ret;

  switch (m_type) {
  case AT_IDENT: {
    auto ident = reinterpret_cast<Identifier *>(m_data);
    ret = ident->Name() + "(" + std::to_string(m_offset) + ")";
    break;
  }
  case AT_STR: {
    auto sl = reinterpret_cast<StringLiteral *>(m_data);
    ret = sl->Label() + " \"" + sl->Val() + "\"";
    break;
  }
  case AT_CHARL:
  case AT_INTL:
    ret = std::to_string(m_data);
    break;
  case AT_TMPCH:
  case AT_TMP:
    ret = "@T" + std::to_string(m_data) + "(" + std::to_string(m_offset) + ")";
    break;

  case AT_OFFSET:
    ret = "(" + std::to_string(m_data) + ")";
    break;

  case AT_LABEL: {
    auto ls = reinterpret_cast<LabelStmt *>(m_data);
    ret = ls->Repr();
    break;
  }
  case AT_ARRAY: {
    auto cast = reinterpret_cast<Identifier *>(m_data);
    auto lr = m_islval ? "l" : "r";
    ret = cast->Name() + "[" + m_minion->Str() + "] " + lr + "value";
    break;
  }
  default:
    assert(0);
    break;
  }
  return ret;
}

std::string Quadruple::Str()
{
  std::string ret;

  switch (m_op) {
  case QO_PARAM:
    ret += "param var " + m_dst->Str();
    break;
  case QO_PRINT:
    ret += "printf " + m_dst->Str();
    break;
  case QO_SCAN:
    ret += "scanf " + m_dst->Str();
    break;
  case QO_LABEL:
    ret += /* no '\n' here! */ m_dst->Str() + ":";
    break;
  case QO_ASSIGN:
    ret += "assign " + m_dst->Str() + " = " + m_arg1->Str();
    break;
  case QO_PLUS:
    ret +=
      "plus " + m_dst->Str() + " = " + m_arg1->Str() + " + " + m_arg2->Str();
    break;
  case QO_MINU:
    ret +=
      "minu " + m_dst->Str() + " = " + m_arg1->Str() + " - " + m_arg2->Str();
    break;
  case QO_MULT:
    ret +=
      "mult " + m_dst->Str() + " = " + m_arg1->Str() + " * " + m_arg2->Str();
    break;
  case QO_DIV:
    ret +=
      "div " + m_dst->Str() + " = " + m_arg1->Str() + " / " + m_arg2->Str();
    break;
  case QO_INDEX_ARG:
    ret += "index_arg " + m_dst->Str() + " = " + m_arg1->Str() + "[" +
           m_arg2->Str() + "] ";
    break;
  case QO_RETURN:
    ret = "ret ";
    if (m_dst) {
      ret += m_dst->Str() + " ";
    }
    ret += "(to " + m_arg1->Str() + ")";
    break;
  case QO_PUSH:
    ret = "push " + m_dst->Str() + ", " + m_arg1->Str();
    break;
  case QO_CALL:
    ret = "call " + m_dst->Str() + " = call " + m_arg1->Str();
    break;
  case QO_GOTO:
    ret = "goto " + m_dst->Str();
    break;
  case QO_BZ:
    ret = "bz " + m_dst->Str() + ", " + m_arg1->Str();
    break;
  case QO_BLT:
    ret = "blt " + m_dst->Str() + ", " + m_arg1->Str() + ", " + m_arg2->Str();
    break;
  case QO_BLE:
    ret = "ble " + m_dst->Str() + ", " + m_arg1->Str() + ", " + m_arg2->Str();
    break;
  case QO_BGT:
    ret = "bgt " + m_dst->Str() + ", " + m_arg1->Str() + ", " + m_arg2->Str();
    break;
  case QO_BGE:
    ret = "bge " + m_dst->Str() + ", " + m_arg1->Str() + ", " + m_arg2->Str();
    break;
  case QO_BEQ:
    ret = "beq " + m_dst->Str() + ", " + m_arg1->Str() + ", " + m_arg2->Str();
    break;
  case QO_BNE:
    ret = "bne " + m_dst->Str() + ", " + m_arg1->Str() + ", " + m_arg2->Str();
    break;
  default:
    break;
  }
  return ret;
}

Gpr FuncInfo::AllocReg(QuadAddr *qa)
{
  for (auto i = 0; i < 8; ++i) {
    if (m_regpool[i].m_bind == nullptr) {
      m_regpool[i].m_bind = qa;
      qa->m_bind = m_regpool[i].m_name;
      return m_regpool[i].m_name;
    }
  }
  return Gpr::zero;
}

void FuncInfo::FreeReg(Gpr name)
{
  m_regpool[name - Gpr::t0].m_bind = nullptr;
}
