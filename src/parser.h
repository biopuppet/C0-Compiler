#ifndef C0C_PARSER_H
#define C0C_PARSER_H

#include "ast.h"
#include "debug.h"
#include "scope.h"
#include "token.h"

class Lexer;
class Parser {
  using ParamList = std::vector<Identifier *>;
  friend class Generator;

public:
  TranslationUnitDecl *Analyse()
  {
    return ParseTranslationUnitDecl();
  }
  TranslationUnitDecl *Unit() const
  {
    return m_unit;
  }

  explicit Parser(Lexer *_lexer)
    : m_ts(_lexer), m_curscope(new Scope(nullptr, S_FILE)),
      m_unit(TranslationUnitDecl::New()), m_curfunc(nullptr)
  {
  }
  Parser() = delete;
  ~Parser() {}

private:
  TokenStream m_ts;
  Scope *m_curscope;
  TranslationUnitDecl *m_unit;
  FunctionDecl *m_curfunc;

#ifdef DEBUG_PARSER
  void Peek();
#endif
  int CheckTokens(const int num, ...);
  int Assert(size_t n, const TokenType type);
  Token *AssertFront(const TokenType type);
  bool MatchFront(const TokenType type)
  {
    return m_ts.GetFrontType() == type;
  }

#ifdef EXTRA_MIDEXAM

#endif  // EXTRA_MIDEXAM

#ifdef EXTRA_SWITCH
  int ParseSwitchStmt();
  int ParseCaseList();
  int ParseCaseStmt();
  int ParseDefaultStmt();

  int ParseConstant();
#endif  // EXTRA_SWITCH

  TranslationUnitDecl *ParseTranslationUnitDecl();  // 程序

  int ParseConstVarDecl(CompoundStmt *cstmt = nullptr);  // 常量声明 常量说明
  int ParseVarDecl(CompoundStmt *cstmt = nullptr);  // 变量声明 变量说明
  FunctionDecl *ParseNonvoidFunctionDecl();         // 有返回值函数定义
  FunctionDecl *ParseVoidFunctionDecl();            // 无返回值函数定义
  FunctionDecl *ParseMain();                        // 主函数

  bool ParseConstCharDef(CompoundStmt *cstmt);
  bool ParseConstIntDef(CompoundStmt *cstmt);
  bool ParseConstVarDef(CompoundStmt *cstmt = nullptr);  // 常量定义

  int ParseVarDef(CompoundStmt *cstmt);            // 变量定义
  Identifier *ParseFunctionReturnType();           // 声明头部
  int ParseFunctionParamList(FunctionDecl *func);  // 参数表
  CompoundStmt *ParseCompoundStmt();               // 复合语句
  // int ParseIdentifier();              // 标识符

  IntegerLiteral *ParseIntegerLiteral();  // 整数
  ExprType ParseTypeIdentifier();         // 类型标识符 := int | char
  IntegerLiteral *ParseUnsignedIntegerLiteral();  // 无符号整数
  StmtList ParseStmtList();                       // 语句列 := {语句}
  // int ParseAlpha();           // 字母 := [_a-zA-Z]
  // int ParseDigit();           // 数字 := 0 | 非零数字

  // int ParseNonzeroDigit(); // 非零数字
  Stmt *ParseStmt();  // 语句

  IfStmt *ParseIfStmt();  // 条件语句

  DoStmt *ParseDoStmt();        // 循环语句
  WhileStmt *ParseWhileStmt();  // 循环语句
  ForStmt *ParseForStmt();      // 循环语句
  Stmt *ParseLoopStmt();        // 循环语句

  CallExpr *ParseNonvoidFunctionCall();  // 有返回值函数调用语句
  CallExpr *ParseVoidFunctionCall();     // 有返回值函数调用语句
  BinaryOp *ParseAssignStmt();           // 赋值语句
  ScanfStmt *ParseScanfStmt();           // 读语句
  PrintStmt *ParsePrintStmt();           // 写语句
  // 返回语句
  ReturnStmt *ParseReturnStmt();
  // 条件: Expr | Expr BinaryOp Expr
  Condition *ParseCondition();
  Expr *ParseExpr();                    // 表达式
  IntegerLiteral *ParseForIncrement();  // 步长 := 无符号整数
  CallExpr::ArgList ParseValueParamList(Identifier *ident);   // 值参数表
  CallExpr::ArgList ParseValueParamList(FunctionDecl *func);  // 值参数表
  StringLiteral *ParseString();                               // 字符串

  // int ParseCompareOperator();  // 关系运算符
  Expr *ParseTerm();            // 项
  int ParsePlusMinuOperator();  // 加法运算符

  Expr *ParseFactor();         // 因子
  int ParseMultDivOperator();  // 乘法运算符

  /**
   * @字符
   * @Grammar: CHARCON
   * @References: <>
   * @First = {}
   * @Follow = {}
   */
  CharLiteral *ParseCharLiteral()  // 字符
  {
    auto res = AssertFront(CHARCON);
    return CharLiteral::New(res, ExprType::T_CHAR, res->m_value[0]);
  }
};

#endif  // !C0C_PARSER_H
