#ifndef C0C_VISITOR_H
#define C0C_VISITOR_H

class BinaryOp;
class Condition;
class UnaryOp;
class CallExpr;
class ArraySubscriptExpr;
class Identifier;
class StringLiteral;
class CharLiteral;
class IntegerLiteral;
class TempVar;

class Decl;
class IfStmt;
class JumpStmt;
class ReturnStmt;
class LabelStmt;
class NullStmt;
class CompoundStmt;
class FunctionDecl;
class TranslationUnitDecl;

class Visitor {
public:
  virtual ~Visitor() {}
  virtual void VisitBinaryOp(BinaryOp *binary) = 0;
  virtual void VisitCondition(Condition *cond) = 0;
  virtual void VisitUnaryOp(UnaryOp *unary) = 0;
  virtual void VisitCallExpr(CallExpr *funcCall) = 0;
  virtual void VisitArraySubscriptExpr(ArraySubscriptExpr *ase) = 0;
  virtual void VisitIdentifier(Identifier *ident) = 0;
  virtual void VisitIntegerLiteral(IntegerLiteral *cons) = 0;
  virtual void VisitCharLiteral(CharLiteral *cons) = 0;
  virtual void VisitStringLiteral(StringLiteral *cons) = 0;
  virtual void VisitTempVar(TempVar *tempVar) = 0;

  virtual void VisitDecl(Decl *init) = 0;
  virtual void VisitDeclStmt(DeclStmt *declStmt) = 0;
  virtual void VisitScanfStmt(ScanfStmt *scanfStmt) = 0;
  virtual void VisitPrintStmt(PrintStmt *printStmt) = 0;
  virtual void VisitIfStmt(IfStmt *ifStmt) = 0;
  virtual void VisitForStmt(ForStmt *forStmt) = 0;
  virtual void VisitWhileStmt(WhileStmt *whileStmt) = 0;
  virtual void VisitDoStmt(DoStmt *doStmt) = 0;
  virtual void VisitJumpStmt(JumpStmt *jumpStmt) = 0;
  virtual void VisitReturnStmt(ReturnStmt *returnStmt) = 0;
  virtual void VisitLabelStmt(LabelStmt *labelStmt) = 0;
  virtual void VisitNullStmt(NullStmt *emptyStmt) = 0;
  virtual void VisitCompoundStmt(CompoundStmt *compStmt) = 0;
  virtual void VisitFunctionDecl(FunctionDecl *funcDecl) = 0;
  virtual void VisitVarDecl(VarDecl *varDecl) = 0;
  virtual void VisitParmVarDecl(ParmVarDecl *parmVarDecl) = 0;
  virtual void VisitTranslationUnitDecl(TranslationUnitDecl *unit) = 0;
};

#endif  // !C0C_VISITOR_H
