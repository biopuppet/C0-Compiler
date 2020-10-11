#include "ast.h"
#include "error.h"
#include "memory_pool.h"
#include "parser.h"
#include "token.h"
#include "type.h"
#include "visitor.h"

static MemoryPoolImp<BinaryOp> binaryOpPool;
static MemoryPoolImp<Condition> conditionPool;
static MemoryPoolImp<CallExpr> callPool;
static MemoryPoolImp<ArraySubscriptExpr> arraySubscriptExprPool;
static MemoryPoolImp<Decl> initializationPool;
static MemoryPoolImp<Identifier> identifierPool;
static MemoryPoolImp<CharLiteral> charPool;
static MemoryPoolImp<StringLiteral> stringPool;
static MemoryPoolImp<IntegerLiteral> integerPool;
static MemoryPoolImp<TempVar> tempVarPool;
static MemoryPoolImp<UnaryOp> unaryOpPool;

static MemoryPoolImp<NullStmt> emptyStmtPool;
static MemoryPoolImp<DeclStmt> declStmtPool;
static MemoryPoolImp<ScanfStmt> scanfStmtPool;
static MemoryPoolImp<PrintStmt> printStmtPool;
static MemoryPoolImp<IfStmt> ifStmtPool;
static MemoryPoolImp<ForStmt> forStmtPool;
static MemoryPoolImp<WhileStmt> whileStmtPool;
static MemoryPoolImp<DoStmt> doStmtPool;
static MemoryPoolImp<JumpStmt> jumpStmtPool;
static MemoryPoolImp<ReturnStmt> returnStmtPool;
static MemoryPoolImp<LabelStmt> labelStmtPool;
static MemoryPoolImp<CompoundStmt> compoundStmtPool;
static MemoryPoolImp<FunctionDecl> funcDeclPool;
static MemoryPoolImp<VarDecl> varDeclPool;
static MemoryPoolImp<ParmVarDecl> parmVarDeclPool;

/*
 * Accept
 */

void Decl::Accept(Visitor *v)
{
  v->VisitDecl(this);
}

void NullStmt::Accept(Visitor *v)
{
  // Nothing to do
}

void DeclStmt::Accept(Visitor *v)
{
  v->VisitDeclStmt(this);
}

void ScanfStmt::Accept(Visitor *v)
{
  v->VisitScanfStmt(this);
}

void PrintStmt::Accept(Visitor *v)
{
  v->VisitPrintStmt(this);
}

void LabelStmt::Accept(Visitor *v)
{
  v->VisitLabelStmt(this);
}

void IfStmt::Accept(Visitor *v)
{
  v->VisitIfStmt(this);
}

void ForStmt::Accept(Visitor *v)
{
  v->VisitForStmt(this);
}

void WhileStmt::Accept(Visitor *v)
{
  v->VisitWhileStmt(this);
}

void DoStmt::Accept(Visitor *v)
{
  v->VisitDoStmt(this);
}

void JumpStmt::Accept(Visitor *v)
{
  v->VisitJumpStmt(this);
}

void ReturnStmt::Accept(Visitor *v)
{
  v->VisitReturnStmt(this);
}

void CompoundStmt::Accept(Visitor *v)
{
  v->VisitCompoundStmt(this);
}

void BinaryOp::Accept(Visitor *v)
{
  v->VisitBinaryOp(this);
}

void Condition::Accept(Visitor *v)
{
  v->VisitCondition(this);
}

void UnaryOp::Accept(Visitor *v)
{
  v->VisitUnaryOp(this);
}

void CallExpr::Accept(Visitor *v)
{
  v->VisitCallExpr(this);
}

void ArraySubscriptExpr::Accept(Visitor *v)
{
  v->VisitArraySubscriptExpr(this);
}

void Identifier::Accept(Visitor *v)
{
  v->VisitIdentifier(this);
}
void IntegerLiteral::Accept(Visitor *v)
{
  v->VisitIntegerLiteral(this);
}
void CharLiteral::Accept(Visitor *v)
{
  v->VisitCharLiteral(this);
}
void StringLiteral::Accept(Visitor *v)
{
  v->VisitStringLiteral(this);
}

void TempVar::Accept(Visitor *v)
{
  v->VisitTempVar(this);
}

void FunctionDecl::Accept(Visitor *v)
{
  v->VisitFunctionDecl(this);
}
void VarDecl::Accept(Visitor *v)
{
  v->VisitVarDecl(this);
}
void ParmVarDecl::Accept(Visitor *v)
{
  v->VisitParmVarDecl(this);
}

void TranslationUnitDecl::Accept(Visitor *v)
{
  v->VisitTranslationUnitDecl(this);
}

BinaryOp *BinaryOp::New(const Token *tok, Expr *lhs, Expr *rhs)
{
  return New(tok, tok->m_type, lhs, rhs);
}

BinaryOp *BinaryOp::New(const Token *tok, int op, Expr *lhs, Expr *rhs)
{
  auto ret = new (binaryOpPool.Alloc()) BinaryOp(tok, op, lhs, rhs);
  ret->m_pool = &binaryOpPool;
  return ret;
}

Condition *Condition::New(Expr *lhs)
{
  auto ret = new (conditionPool.Alloc()) Condition(lhs);
  ret->m_pool = &conditionPool;
  return ret;
}
Condition *Condition::New(Expr *lhs, const Token *tok, Expr *rhs)
{
  auto ret = new (conditionPool.Alloc()) Condition(lhs, tok, rhs);
  ret->m_pool = &conditionPool;
  return ret;
}

/*
 * Unary Operators
 */

UnaryOp *UnaryOp::New(Token *op, Expr *operand, QualType type)
{
  auto ret = new (unaryOpPool.Alloc()) UnaryOp(op, operand, type);
  ret->m_pool = &unaryOpPool;

  return ret;
}

bool UnaryOp::IsLVal()
{
  // Only deref('*') could be lvalue;
  return false;
}

void UnaryOp::TypeChecking()
{
  switch (m_op) {
  case PLUS:
  case MINU:
    return UnaryArithmOpTypeChecking();
  case CAST:
    return CastOpTypeChecking();

  default:
    assert(false);
  }
}

void UnaryOp::AddrOpTypeChecking()
{
  auto funcType = m_operand->Type()->ToFunc();
  if (funcType == nullptr && !m_operand->IsLVal())
    Error(this, "expression must be an lvalue or function designator");
  m_qual_type = PointerType::New(m_operand->Type());
}

void UnaryOp::DerefOpTypeChecking()
{
  auto pointerType = m_operand->Type()->ToPointer();
  if (!pointerType)
    Error(this, "pointer expected for deref operator '*'");
  m_qual_type = pointerType->Derived();
}

void UnaryOp::UnaryArithmOpTypeChecking()
{
  if (PLUS == m_op || MINU == m_op) {
    if (!m_operand->Type()->ToArithm())
      Error(this, "Arithmetic type expected");
    m_qual_type = m_operand->Type();
  }
  else {
    m_qual_type = ArithmType::New(ExprType::T_INT);
  }
}

void UnaryOp::CastOpTypeChecking()
{
  // auto operandType = Type::MayCast(m_operand->Type());

  // // The m_qual_type has been initiated to dest type
  // if (m_qual_type->ToVoid()) {
  //     // The expression becomes a void expression
  // }
  // else if (!m_qual_type->IsScalar() || !operandType->IsScalar()) {
  //     if (!m_qual_type->Compatible(*operandType))
  //         Error(this, "the cast type should be arithemetic type or
  //         pointer");
  // }
}

/*
 * Function Call
 */
CallExpr *CallExpr::New(Identifier *des, const ArgList &args)
{
  return New(des, des->Qual(), args);
}
CallExpr *
CallExpr::New(Identifier *designator, QualType qt, const ArgList &args)
{
  auto ret = new (callPool.Alloc()) CallExpr(designator, qt, args);
  ret->m_pool = &callPool;

  // ret->TypeChecking();
  return ret;
}

void CallExpr::TypeChecking()
{
  // auto pointerType = m_designator->Type()->ToPointer();
  // if (pointerType) {
  //     if (!pointerType->Derived()->ToFunc())
  //         Error(m_designator,
  //               "called object is not a function or function pointer");
  //     // Convert function pointer to function type
  //     // m_designator = UnaryOp::New(DEREF, m_designator);
  // }
  // auto funcType = m_designator->Type()->ToFunc();
  // if (!funcType) {
  //     Error(m_designator,
  //           "called object is not a function or function pointer");
  // }
  // else if (!funcType->Derived()->ToVoid() &&
  //          !funcType->Derived()->Complete()) {
  //     Error(m_designator, "invalid use of incomplete return type");
  // }

  // auto arg = m_args.begin();
  // for (auto param : funcType->Params()) {
  //     if (arg == m_args.end())
  //         Error(this, "too few arguments for function call");
  //     *arg = Expr::MayCast(*arg, param->Type());
  //     ++arg;
  // }
  // if (arg != m_args.end())
  //     Error(this, "too many arguments for function call");

  // m_qual_type = funcType->Derived();
}

/*
 * Array Subscript Expression
 */
ArraySubscriptExpr *
ArraySubscriptExpr::New(Identifier *lhs, Expr *rhs, QualType type, bool islval)
{
  auto ret = new (arraySubscriptExprPool.Alloc())
    ArraySubscriptExpr(lhs, rhs, type, islval);
  ret->m_pool = &arraySubscriptExprPool;
  return ret;
}

ArraySubscriptExpr *ArraySubscriptExpr::New(Identifier *lhs,
                                            Expr *rhs,
                                            QualType type,
                                            bool islval,
                                            SourceLocation loc)
{
  auto ret = new (arraySubscriptExprPool.Alloc())
    ArraySubscriptExpr(lhs, rhs, type, islval, loc);
  ret->m_pool = &arraySubscriptExprPool;
  return ret;
}

/*
 * Identifier
 */
Identifier *
Identifier::New(const Token *tok, QualType type, enum Linkage linkage)
{
  auto ret = new (identifierPool.Alloc()) Identifier(tok, type, linkage);
  ret->m_pool = &identifierPool;
  return ret;
}

void Decl::AddInit(Initializer init)
{
  // init.m_expr = Expr::MayCast(init.m_expr, init.m_type);

  // auto res = m_inits.insert(init);
  // if (!res.second) {
  //     m_inits.erase(res.first);
  //     m_inits.insert(init);
  // }
}

/*
 * Character Literal
 */
CharLiteral *CharLiteral::New(const Token *tok, int tag, unsigned val)
{
  auto type = ArithmType::New(tag);
  auto ret = new (charPool.Alloc()) CharLiteral(tok, type, val);
  ret->m_pool = &charPool;
  return ret;
}

/*
 * String Literal
 */
StringLiteral *StringLiteral::New(const std::string &val)
{
  auto ret = new (stringPool.Alloc()) StringLiteral(val);
  ret->m_pool = &stringPool;
  return ret;
}

StringLiteral *StringLiteral::New(const Token *tok)
{
  auto ret = new (stringPool.Alloc()) StringLiteral(tok);
  ret->m_pool = &stringPool;
  return ret;
}

std::string StringLiteral::SValRepr() const
{
  std::vector<char> buf(4 * m_sval.size() + 1);
  for (size_t i = 0; i < m_sval.size(); ++i) {
    int c = m_sval[i];
    sprintf(&buf[i * 4], "\\x%1x%1x", (c >> 4) & 0xf, c & 0xf);
  }
  return std::string(buf.begin(), buf.end() - 1);
}

IntegerLiteral *IntegerLiteral::New(const Token *tok, int tag, long val)
{
  auto type = ArithmType::New(tag);
  auto ret = new (integerPool.Alloc()) IntegerLiteral(tok, type, val);
  ret->m_pool = &integerPool;
  return ret;
}

IntegerLiteral *IntegerLiteral::New(const Token *tok, int tag)
{
  auto type = ArithmType::New(tag);
  auto ret = new (integerPool.Alloc()) IntegerLiteral(tok, type);
  ret->m_pool = &integerPool;
  return ret;
}

/*
 * TempVar
 */

TempVar *TempVar::New(QualType type)
{
  auto ret = new (tempVarPool.Alloc()) TempVar(type);
  ret->m_pool = &tempVarPool;
  return ret;
}

/*
 * Statement
 */

NullStmt *NullStmt::New()
{
  auto ret = new (emptyStmtPool.Alloc()) NullStmt();
  ret->m_pool = &emptyStmtPool;
  return ret;
}

DeclStmt *DeclStmt::New(Decl *decl)
{
  auto ret = new (declStmtPool.Alloc()) DeclStmt(decl);
  ret->m_pool = &declStmtPool;
  return ret;
}

ScanfStmt *ScanfStmt::New()
{
  auto ret = new (scanfStmtPool.Alloc()) ScanfStmt();
  ret->m_pool = &scanfStmtPool;
  return ret;
}

ScanfStmt *ScanfStmt::New(IdentList &idents)
{
  auto ret = new (scanfStmtPool.Alloc()) ScanfStmt(idents);
  ret->m_pool = &scanfStmtPool;
  return ret;
}

PrintStmt *PrintStmt::New(Expr *expr)
{
  auto ret = new (printStmtPool.Alloc()) PrintStmt(expr);
  ret->m_pool = &printStmtPool;
  return ret;
}

// PrintStmt *PrintStmt::New(StringLiteral *sl)
// {
//     auto ret = new (printStmtPool.Alloc()) PrintStmt(sl);
//     ret->m_pool = &printStmtPool;
//     return ret;
// }

PrintStmt *PrintStmt::New(StringLiteral *sl, Expr *expr)
{
  auto ret = new (printStmtPool.Alloc()) PrintStmt(sl, expr);
  ret->m_pool = &printStmtPool;
  return ret;
}

// The else stmt could be null
IfStmt *IfStmt::New(Condition *cond, Stmt *then, Stmt *els)
{
  auto ret = new (ifStmtPool.Alloc()) IfStmt(cond, then, els);
  ret->m_pool = &ifStmtPool;
  return ret;
}

ForStmt *ForStmt::New(Identifier *init_ident,
                      Expr *init_expr,
                      Expr *cond,
                      Identifier *inc_lhs,
                      Identifier *inc_rhs,
                      IntegerLiteral *inc,
                      Stmt *body)
{
  auto ret = new (forStmtPool.Alloc())
    ForStmt(init_ident, init_expr, cond, inc_lhs, inc_rhs, inc, body);
  ret->m_pool = &forStmtPool;
  return ret;
}

WhileStmt *WhileStmt::New(Condition *cond, Stmt *body)
{
  auto ret = new (whileStmtPool.Alloc()) WhileStmt(cond, body);
  ret->m_pool = &whileStmtPool;
  return ret;
}

DoStmt *DoStmt::New(Stmt *body, Condition *cond)
{
  auto ret = new (doStmtPool.Alloc()) DoStmt(body, cond);
  ret->m_pool = &doStmtPool;
  return ret;
}

CompoundStmt *CompoundStmt::New(std::list<Stmt *> &stmts, ::Scope *scope)
{
  auto ret = new (compoundStmtPool.Alloc()) CompoundStmt(stmts, scope);
  ret->m_pool = &compoundStmtPool;
  return ret;
}

CompoundStmt *CompoundStmt::New(::Scope *scope)
{
  auto ret = new (compoundStmtPool.Alloc()) CompoundStmt(scope);
  ret->m_pool = &compoundStmtPool;
  return ret;
}

JumpStmt *JumpStmt::New(LabelStmt *label)
{
  auto ret = new (jumpStmtPool.Alloc()) JumpStmt(label);
  ret->m_pool = &jumpStmtPool;
  return ret;
}

ReturnStmt *ReturnStmt::New(FunctionDecl *func, Expr *expr)
{
  auto ret = new (returnStmtPool.Alloc()) ReturnStmt(func, expr);
  ret->m_pool = &returnStmtPool;
  return ret;
}

LabelStmt *LabelStmt::New(std::string &label)
{
  auto ret = new (labelStmtPool.Alloc()) LabelStmt(label);
  ret->m_pool = &labelStmtPool;
  return ret;
}
LabelStmt *LabelStmt::New()
{
  auto ret = new (labelStmtPool.Alloc()) LabelStmt();
  ret->m_pool = &labelStmtPool;
  return ret;
}

FunctionDecl *
FunctionDecl::New(Identifier *ident, Scope *scope, LabelStmt *retLabel)
{
  auto ret = new (funcDeclPool.Alloc()) FunctionDecl(ident, scope, retLabel);
  ret->m_pool = &funcDeclPool;
  return ret;
}

VarDecl *VarDecl::New(Identifier *ident, QualType qt)
{
  auto ret = new (varDeclPool.Alloc()) VarDecl(ident, qt);
  ret->m_pool = &varDeclPool;
  return ret;
}

VarDecl *
VarDecl::New(Identifier *ident, QualType qt, Token *uop, CharLiteral *cl)
{
  auto ret = new (varDeclPool.Alloc()) VarDecl(ident, qt, uop, cl);
  ret->m_pool = &varDeclPool;
  return ret;
}

VarDecl *
VarDecl::New(Identifier *ident, QualType qt, Token *uop, IntegerLiteral *il)
{
  auto ret = new (varDeclPool.Alloc()) VarDecl(ident, qt, uop, il);
  ret->m_pool = &varDeclPool;
  return ret;
}

VarDecl *VarDecl::New(Identifier *ident, QualType qt, CharLiteral *cl)
{
  auto ret = new (varDeclPool.Alloc()) VarDecl(ident, qt, cl);
  ret->m_pool = &varDeclPool;
  return ret;
}

VarDecl *VarDecl::New(Identifier *ident, QualType qt, IntegerLiteral *il)
{
  auto ret = new (varDeclPool.Alloc()) VarDecl(ident, qt, il);
  ret->m_pool = &varDeclPool;
  return ret;
}

ParmVarDecl *ParmVarDecl::New(Identifier *ident, FunctionDecl *fd)
{
  auto ret = new (parmVarDeclPool.Alloc()) ParmVarDecl(ident, fd);
  ret->m_pool = &parmVarDeclPool;
  return ret;
}

ParmVarDecl *ParmVarDecl::New(Identifier *ident, QualType qt, FunctionDecl *fd)
{
  auto ret = new (parmVarDeclPool.Alloc()) ParmVarDecl(ident, qt, fd);
  ret->m_pool = &parmVarDeclPool;
  return ret;
}

bool Initializer::operator<(const Initializer &rhs) const
{
  if (m_offset < rhs.m_offset)
    return true;
  return (m_offset == rhs.m_offset);
}
