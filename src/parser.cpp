#include "parser.h"
#include "ast.h"
#include "error.h"
#include "iobase.h"
#include "lexer.h"
#include "scope.h"

#include <cstdarg>
#include <cstring>

#ifdef PRINT_OUTPUT
extern std::ofstream outstream;
#endif  // PRINT_OUTPUT

#if defined(PRINT_OUTPUT) && defined(PRINT_PARSER)
#ifndef DEBUG_PARSE_END
#define DEBUG_PARSE_END(x)                                                     \
    do {                                                                       \
        outstream << "<" << x << ">" << std::endl;                             \
    } while (0)
#endif  // !DEBUG_PARSE_END
#endif  // PRINT_OUTPUT && PRINT_PARSER

#ifdef DEBUG_PARSER

static std::string indent = "";

void Parser::Peek()
{
    outstream << "*********peeking**********" << std::endl;
    for (auto it = m_ts.begin(); it != m_ts.end(); ++it) {
        outstream << **it << std::endl;
    }
}

#define DEBUG_PARSE_BEGIN(x)                                                   \
    do {                                                                       \
        outstream << indent << "{" << x << std::endl;                          \
        indent.append("  ");                                                   \
    } while (0)

#ifdef DEBUG_PARSE_END
#undef DEBUG_PARSE_END
#endif
#ifndef DEBUG_PARSE_END
#define DEBUG_PARSE_END(x)                                                     \
    do {                                                                       \
        outstream << "<" << x << ">" << std::endl;                             \
        indent.pop_back();                                                     \
        indent.pop_back();                                                     \
        outstream << indent << x << "}" << std::endl;                          \
        Peek();                                                                \
    } while (0)
#endif  // DEBUG_PARSE_END

#endif  // DEBUG_PARSER

Token *Parser::AssertFront(const TokenType type)
{
    int ret = Assert(0, type);
    if (!ret) {
        return m_ts.PrintFront();
    }
    return nullptr;
}

int Parser::Assert(size_t n, const TokenType type)
{
    auto ft = m_ts.At(n);
    if (ft->m_type != type) {
        if (type == SEMICN) {
            // TODO prev() is enough?
            if (!n) {
                Error(m_ts.prev(), 'k');
            }
            else {
                Error(m_ts[n - 1], 'k');
            }
        }
        else if (type == WHILETK) {
            Error(m_ts[n], 'n');
        }
        else if (type == RPARENT) {
            Error(m_ts[n], 'l');
        }
        else if (type == RBRACK) {
            Error(m_ts[n], 'm');
        }
        else {
            // Error(ft, "expected %s, we got %s.", tokenlist[(type)],
            //       tokenlist[ft->m_type]);
        }
        return -1;
    }
    return 0;
}

/**
 * @brief Check whether front tokens in TokenStream equals va_args.
 * @param num Number of tokens to be checked.
 * @param va_arg(s) Tokens to be checked.
 * @return 0 if all check out, Otherwise the position that first mismatch.
 */
int Parser::CheckTokens(const int num, ...)
{
    va_list args;
    va_start(args, num);

    for (int i = 0; i < num; ++i) {
        if (m_ts[i]->m_type != (TokenType)va_arg(args, int)) {
            return i + 1;
        }
    }

    va_end(args);
    return 0;
}

#ifdef EXTRA_MIDEXAM

/*
 * @template
 * @Grammar:
 *
 * @References:
 * @First = {}
 * @Follow = {}
 */
int Parser::ParseMultDivOperator()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("template");
#endif

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("template");
#endif
    return 0;
}

#endif  // EXTRA_MIDEXAM

#ifdef EXTRA_SWITCH
/*
 * @缺省
 * @Grammar:
 *   default : <语句>|<空>
 * @References:
 * @First = {}
 * @Follow = {}
 */
int Parser::ParseDefaultStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("缺省");
#endif

    AssertFront(DEFAULTTK);
    AssertFront(COLON);
    if (m_ts.GetFrontType() != RBRACE) {  // epsilon
        ParseStmt();
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("缺省");
#endif
    return 0;
}

/*
 * @情况子语句
 * @Grammar:
 *   case<常量>:<语句>
 * @References:
 * @First = {}
 * @Follow = {}
 */
int Parser::ParseCaseStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("情况子语句");
#endif

    AssertFront(CASETK);
    ParseConstant();
    AssertFront(COLON);
    ParseStmt();

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("情况子语句");
#endif
    return 0;
}

/*
 * @情况表
 * @Grammar:
 *   <情况子语句>{<情况子语句>}
 * @References:
 * @First = {}
 * @Follow = {}
 */
int Parser::ParseCaseList()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("情况表");
#endif

    while (MatchFront(CASETK)) {
        ParseCaseStmt();
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("情况表");
#endif
    return 0;
}

/*
 * @情况语句
 * @Grammar:
 *   switch '('<表达式>')' '{'<情况表><缺省>'}'
 * @References: <语句>
 * @First = {}
 * @Follow = {}
 */
int Parser::ParseSwitchStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("情况语句");
#endif
    AssertFront(SWITCHTK);
    AssertFront(LPARENT);
    ParseExpr();
    AssertFront(RPARENT);

    AssertFront(LBRACE);
    ParseCaseList();
    ParseDefaultStmt();
    AssertFront(RBRACE);

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("情况语句");
#endif
    return 0;
}
#endif  // EXTRA_SWITCH

/*
 * @加法运算符
 * @Grammar:
 *   + | -
 * @References:
 * @First = {}
 * @Follow = {}
 */
int Parser::ParsePlusMinuOperator()
{
    TokenType type = m_ts.GetFrontType();

    if (type != PLUS && type != MINU) {
        // TODO error handling
        return -1;
    }
    m_ts.PrintFront();
    return type;
}

/*
 * @类型标识符
 * @Grammar:
 *   int | char
 * @References:
 * @First = {}
 * @Follow = {}
 */
ExprType Parser::ParseTypeIdentifier()
{
    TokenType type = m_ts.GetFrontType();
    auto ret = ExprType::T_INVALID;

    switch (type) {
    case TokenType::INTTK:
        ret = ExprType::T_INT;
        break;
    case TokenType::CHARTK:
        ret = ExprType::T_CHAR;
        break;
    default:
        return ret;
    }
    m_ts.PrintFront();
    return ret;
}

/*
 * @字符串
 * @Grammar:
 *   <类型标识符>(<标识符>|<标识符>'['<无符号整数>']')
 *   {,(<标识符>|<标识符>'['<无符号整数>']' )}
 * @References:
 * @First = {}
 * @Follow = {}
 */
StringLiteral *Parser::ParseString()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("字符串");
#endif

    auto tok = AssertFront(STRCON);

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("字符串");
#endif
    return StringLiteral::New(tok);
}

/*
 * @无符号整数
 * @Grammar: INTCON
 * @References: <整数> <步长> <变量定义>
 * @First = {}
 * @Follow = {}
 */
IntegerLiteral *Parser::ParseUnsignedIntegerLiteral()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("无符号整数");
#endif
    auto res = AssertFront(INTCON);
    if (res != nullptr) {
        const char *prev = res->m_value.c_str();
        // debug("INTCON: %s", prev);
        /* has leading zero(s) */
        if (strlen(prev) > 1 && prev[0] == '0') {
            Error(res, 'a');
        }
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("无符号整数");
#endif
    return IntegerLiteral::New(res, ExprType::T_INT);
}

/*
 * @步长
 * @Grammar:
 *   <无符号整数>
 * @References:
 * @First = {}
 * @Follow = {}
 */
IntegerLiteral *Parser::ParseForIncrement()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("步长");
#endif

    auto ret = ParseUnsignedIntegerLiteral();

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("步长");
#endif
    return ret;
}

/*
 * @整数
 * @Grammar:
 *   [+ | -] <无符号整数>
 * @References: <常量定义> <因子>
 * @First = {+, -, 0-9}
 * @Follow = {',', ';', Follow(因子)}
 */
IntegerLiteral *Parser::ParseIntegerLiteral()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("整数");
#endif
    TokenType type = m_ts.GetFrontType();
    auto sign = 1;
    IntegerLiteral *il = nullptr;

    if (type == PLUS || type == MINU) {
        m_ts.PrintFront();
        sign = type == MINU ? -1 : sign;
    }
    il = ParseUnsignedIntegerLiteral();
    // il->SetVal(sign * il->Val());
    il = IntegerLiteral::New(nullptr, ExprType::T_INT, sign * il->Val());

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("整数");
#endif
    return il;
}

#ifdef EXTRA_SWITCH

/*
 * @常量
 * @Grammar:
 *   <整数> | <字符>
 * @References:
 * @First = {+, -, intcon, charcon}
 * @Follow = {',', ';', Follow(因子)}
 */
int Parser::ParseConstant()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("常量");
#endif

    TokenType type = m_ts.GetFrontType();
    if (type == PLUS || type == MINU || type == INTCON) {
        ParseIntegerLiteral();
    }
    else if (type == CHARCON) {
        ParseCharLiteral();
    }
    else {
        Error("Expected Constant!");
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("常量");
#endif
    return 0;
}
#endif  // EXTRA_SWITCH

/*
 * @*字符型常量定义
 * @Grammar:
 *   char<标识符>=<字符>{,<标识符>=<字符>}
 * @References: <程序> <>
 * @First = {int, char}
 * @Follow = {}
 */
bool Parser::ParseConstCharDef(CompoundStmt *cstmt)
{
    assert(MatchFront(TokenType::CHARTK));
    do {
        m_ts.PrintFront();
        auto tok = AssertFront(IDENFR);
        /* first look up in current scope, Error() if redef */
        auto res = m_curscope->FindInCurScope(tok->m_value);
        if (res != nullptr) {
            Error(tok, 'b');  // redefinition
        }
        else {
            // debug("not found %s in curscope, ready for insertion.\n",
            //       tok->m_value.c_str());
        }
        auto type = ArithmType::New(ExprType::T_CHAR);
        auto ident = Identifier::New(tok, QualType(type, Qualifier::CONST));
        assert(ident->IsConstQualified());
        m_curscope->Insert(tok->m_value, ident);
        debug("insert const var %s into scope", tok->m_value.c_str());

        AssertFront(ASSIGN);
        if (m_ts.GetFrontType() != CHARCON) {
            Error(m_ts[0], 'o');
            while (m_ts.GetFrontType() != SEMICN) {
                m_ts.FlushFront();
            }
            return false;
        }
        auto char_literal = ParseCharLiteral();
        if (char_literal == nullptr) {
            if (ParseIntegerLiteral() == nullptr) {
                Error(m_ts.prev(), 'o');
            }
            while (m_ts.GetFrontType() != SEMICN) {
                m_ts.FlushFront();
            }
            return false;
        }
        if (m_curscope->IsGlobalScope()) {
            auto decl = VarDecl::New(ident, type, char_literal);
            m_unit->AddVarDecl(decl);
            ident->SetDecl(decl);
            debug("tu: add (const) " ANSI_CYAN
                  "VarDecl(%s, %#X, \'%c\')\n" ANSI_RESET,
                  ident->Name().c_str(), type->Tag(), char_literal->Val());
        }
        else if (m_curscope->IsFuncScope()) {
            auto decl = VarDecl::New(ident, type, char_literal);
            cstmt->AddVarDecl(decl);
            ident->SetDecl(decl);
            debug("cstmt: add (const)" ANSI_CYAN
                  " VarDecl(%s, %#X, \'%c\')\n" ANSI_RESET,
                  ident->Name().c_str(), type->Tag(), char_literal->Val());
        }

    } while (MatchFront(COMMA));
    return true;
}

/*
 * @*整型常量定义
 * @Grammar:
 *   int<标识符>=<整数>{,<标识符>=<整数>}
 * @References: <程序> <>
 * @First = {int, char}
 * @Follow = {}
 */
bool Parser::ParseConstIntDef(CompoundStmt *cstmt)
{
    assert(MatchFront(TokenType::INTTK));
    do {
        m_ts.PrintFront();
        AssertFront(IDENFR);
        /* first look up in current scope, Error() if redef */
        auto res = m_curscope->FindInCurScope(m_ts.prev()->m_value);
        if (res != nullptr) {
            Error(m_ts.prev(), 'b');  // redefinition
        }
        else {
            // debug("not found %s in curscope, ready for insertion.\n",
            //       m_ts.prev()->m_value.c_str());
        }
        auto type = ArithmType::New(ExprType::T_INT);
        auto ident =
            Identifier::New(m_ts.prev(), QualType(type, Qualifier::CONST));
        assert(ident->IsConstQualified());
        m_curscope->Insert(m_ts.prev()->m_value, ident);
        debug("insert const var %s into scope", m_ts.prev()->m_value.c_str());

        AssertFront(ASSIGN);
        if (m_ts.GetFrontType() != MINU && m_ts.GetFrontType() != PLUS &&
            m_ts.GetFrontType() != INTCON) {
            Error(m_ts[0], 'o');
            while (m_ts.GetFrontType() != SEMICN) {
                m_ts.FlushFront();
            }
            return false;
        }
        auto int_literal = ParseIntegerLiteral();
        if (int_literal == nullptr) {
            if (ParseCharLiteral() == nullptr) {
                Error(m_ts[0], 'o');
            }
            while (m_ts.GetFrontType() != SEMICN) {
                m_ts.FlushFront();
            }
            return false;
        }

        auto decl = VarDecl::New(ident, type, int_literal);
        ident->SetDecl(decl);
        // global var decl
        if (m_curscope->IsGlobalScope()) {
            m_unit->AddVarDecl(decl);
            debug("tu: add (const) " ANSI_CYAN
                  "VarDecl(%s, %#X, %d)\n" ANSI_RESET,
                  ident->Name().c_str(), type->Tag(), int_literal->Val());
        }
        // function var decl
        else if (m_curscope->IsFuncScope()) {
            cstmt->AddVarDecl(decl);
            debug("cstmt: add (const) " ANSI_CYAN
                  "VarDecl(%s, %#X, %d)\n" ANSI_RESET,
                  ident->Name().c_str(), type->Tag(), int_literal->Val());
        }
    } while (MatchFront(COMMA));
    return true;
}

/*
 * @常量定义
 * @Grammar:
 *   int<标识符>=<整数>{,<标识符>=<整数>}
 *   | char<标识符>=<字符>{,<标识符>=<字符>}
 * @References: <程序> <>
 * @First = {int, char}
 * @Follow = {}
 */
bool Parser::ParseConstVarDef(CompoundStmt *cstmt)
{
    TokenType first_type = m_ts.GetFrontType();
    if (first_type != CHARTK && first_type != INTTK) {
        Error("const defintion should start with \'int\' or \'char\'");
        return false;
    }
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("常量定义");
#endif
    auto ret = first_type == TokenType::CHARTK ? ParseConstCharDef(cstmt) :
                                                 ParseConstIntDef(cstmt);
#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("常量定义");
#endif
    return ret;
}

/*
 * @常量说明
 * @Grammar:
 *   const <常量定义>;
 *   {const <常量定义>;}
 * @References: <程序> <复合语句>
 * @First = {const}
 * @Follow = {int, char, First(语句), void}
 */
int Parser::ParseConstVarDecl(CompoundStmt *cstmt)
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("常量说明");
#endif
    auto ret = false;
    do {
        AssertFront(CONSTTK);           // const
        ret = ParseConstVarDef(cstmt);  // int a = 10
        AssertFront(SEMICN);            // ;
    } while (MatchFront(CONSTTK));

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("常量说明");
#endif
    return 0;
}

/*
 * @变量定义
 * @Grammar:
 *   <类型标识符>(<标识符>|<标识符>'['<无符号整数>']')
 *   {,(<标识符>|<标识符>'['<无符号整数>']' )}
 * @References: <变量说明>
 * @First = {}
 * @Follow = {;}
 */
int Parser::ParseVarDef(CompoundStmt *cstmt)
{
    auto first_type = m_ts.GetFrontType();
    if (!MatchFront(INTTK) && !MatchFront(CHARTK)) {
        // TODO error handling
        return -1;
    }
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("变量定义");
#endif

    do {
        m_ts.PrintFront();

        auto tok = AssertFront(IDENFR);
        /* first look up in current scope, Error() if redef */
        auto res = m_curscope->FindInCurScope(tok->m_value);
        if (res != nullptr) {
            Error(tok, 'b');  // redefinition
        }
        else {
            // debug("not found %s in curscope, ready for insertion.\n",
            //       tok->m_value.c_str());
        }

        auto etype = first_type == INTTK ? ExprType::T_INT : ExprType::T_CHAR;
        Type *type = nullptr;
        if (MatchFront(LBRACK)) {
            m_ts.PrintFront();                         // [
            auto len = ParseUnsignedIntegerLiteral();  // <unsigned int>
            AssertFront(RBRACK);                       // ]
            auto element_type = QualType(ArithmType::New(etype));
            type = ArrayType::New(len->Val(), element_type);
        }
        else {
            type = ArithmType::New(etype);
        }
        auto qt = QualType(type);
        auto ident = Identifier::New(tok, type);

        m_curscope->Insert(tok->m_value, ident);
        debug("insert var %s into scope %#X", tok->m_value.c_str(), m_curscope);

        // no assignment allowed in non-const var decl
        auto decl = VarDecl::New(ident, qt);
        ident->SetDecl(decl);
        if (m_curscope->IsGlobalScope()) {
            m_unit->AddVarDecl(decl);
            debug("tu: add decl " ANSI_CYAN "VarDecl(%s, %s)\n" ANSI_RESET,
                  ident->Name().c_str(), qt->Str().c_str());
        }
        // function var decl
        else if (m_curscope->IsFuncScope()) {
            cstmt->AddVarDecl(decl);
            debug("cstmt: add decl " ANSI_CYAN "VarDecl(%s, %s)\n" ANSI_RESET,
                  ident->Name().c_str(), qt->Str().c_str());
        }
    } while (MatchFront(COMMA));

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("变量定义");
#endif
    return 0;
}

/*
 * @变量说明
 * @Grammar:
 *   <变量定义>;{<变量定义>;}
 * @References: <程序> <复合语句>
 * @First = {int/char IDENFR '['/','/';'}
 * @Follow = {int/char IDENFR '(', void, First(语句), '}'}
 */
int Parser::ParseVarDecl(CompoundStmt *cstmt)
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("变量说明");
#endif

    do {
        ParseVarDef(cstmt);
        AssertFront(SEMICN);

        if (MatchFront(INTTK) || MatchFront(CHARTK)) {
            if (m_ts.GetType(1) == IDENFR && m_ts.GetType(2) == LPARENT) {
                break;
            }
        }
    } while (MatchFront(INTTK) || MatchFront(CHARTK));

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("变量说明");
#endif
    return 0;
}

/*
 * @声明头部
 * @Grammar:
 *   (int | char)<标识符>
 * @References: <有返回值函数定义>
 * @First = {int, char}
 * @Follow = {'('}
 */
Identifier *Parser::ParseFunctionReturnType()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("声明头部");
#endif
    Identifier *ident = nullptr;
    auto type = ParseTypeIdentifier();
    auto tok = AssertFront(IDENFR);
    if (tok->m_type == IDENFR) {
        auto derived_type = QualType(ArithmType::New(type));
        auto qt = QualType(FuncType::New(derived_type, 0));
        ident = Identifier::New(tok, qt);
    }
#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("声明头部");
#endif
    return ident;
}

/*
 * @参数表
 * @Grammar:
 *    <类型标识符><标识符>{,<类型标识符><标识符>}| <空>
 * @References: <有返回值函数定义> <无返回值函数定义>
 * @First = {int, char, epsilon}
 * @Follow = {')'}
 */
int Parser::ParseFunctionParamList(FunctionDecl *func)
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("参数表");
#endif

    auto func_type = func->FuncType();
    FuncType::ParamList params;
    for (;;) {
        if (MatchFront(RPARENT)) {
            break;  // epsilon
        }
        auto expr_type = ParseTypeIdentifier();
        assert(expr_type != ExprType::T_INVALID);
        auto tok = AssertFront(IDENFR);
        if (tok != nullptr) {
            /* first look up in current scope, Error() if redef */
            auto res = m_curscope->FindInCurScope(tok->m_value);
            if (res != nullptr) {
                Error(tok, 'b');  // redefinition
            }
            else {
                // debug("not found %s in curscope, ready for insertion.\n",
                //       tok->m_value.c_str());
            }
            auto qt = QualType(ArithmType::New(expr_type));
            auto ident = Identifier::New(tok, qt);
            m_curscope->Insert(tok->m_value, ident);
            debug("Scope:%#X insert %s\n", m_curscope, tok->m_value.c_str());

            auto param = ParmVarDecl::New(ident);
            ident->SetDecl(param);
            func->AddParam(param);  // for param checking
            params.push_back(param);
            assert(func->ParamNum() > 0);
            // Error("%s: %d %d\n", ident->Name().c_str(), ident->IsInt(),
            //       ident->IsChar());
        }
        if (!MatchFront(COMMA)) {
            break;
        }
        AssertFront(COMMA);
    }
    func_type->SetParams(params);

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("参数表");
#endif
    return 0;
}

/*
 * @无返回值函数定义
 * @Grammar:
 *   void <标识符>'('<参数表>')''{'<复合语句>'}'
 * @References: <程序>
 * @First = {void}
 * @Follow = {}
 */
FunctionDecl *Parser::ParseVoidFunctionDecl()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("无返回值函数定义");
#endif

    AssertFront(VOIDTK);
    auto func_tok = AssertFront(IDENFR);
    if (func_tok == nullptr) {
        return nullptr;
    }
    auto ret_type = QualType(ArithmType::New(ExprType::T_VOID));
    auto ident = Identifier::New(
        func_tok, QualType(FuncType::New(ret_type, ExprType::F_NORETURN)));
    auto func = FunctionDecl::New(ident, nullptr);

    m_curfunc = func;
    ident->SetDecl(func);

    ident->Type()->ToFunc()->SetFuncDecl(func);
    assert(ident->Type()->ToFunc());
    assert(ident->Type()->ToFunc()->IsVoid());
    assert(ident->Type()->ToFunc()->GetFuncDecl() != nullptr);
    assert(func->FuncType()->IsVoid());

    m_curscope->Insert(func_tok->m_value, ident);  // void
    debug("tu %#X: insert nonvoid func " ANSI_GREEN "%s\n" ANSI_RESET,
          m_curscope, ident->Name().c_str());
    m_curscope = new Scope(m_curscope, S_VOID_FUNC);
    m_curscope->Insert(func_tok->m_value, ident);  // void
    debug("void func%#X: insert nonvoid func " ANSI_GREEN "%s\n" ANSI_RESET,
          m_curscope, ident->Name().c_str());
    AssertFront(LPARENT);
    ParseFunctionParamList(func);
    AssertFront(RPARENT);
    AssertFront(LBRACE);
    func->SetBody(ParseCompoundStmt());
    AssertFront(RBRACE);

    func->SetScope(m_curscope);
    // m_curscope->Peek();

    // Scope *temp = m_curscope;
    m_curscope = m_curscope->parent();
    m_curfunc = nullptr;
    // debug("scope now is %#X\n", m_curscope);
    // delete temp;

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("无返回值函数定义");
#endif
    return func;
}

/*
 * @有返回值函数定义
 * @Grammar:
 *   <声明头部>'('<参数表>')''{'<复合语句>'}'
 * @References: <程序>
 * @First = {int, char}
 * @Follow = {}
 */
FunctionDecl *Parser::ParseNonvoidFunctionDecl()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("有返回值函数定义");
#endif

    auto ident = ParseFunctionReturnType();
    auto func = FunctionDecl::New(ident);
    m_curfunc = func;
    ident->SetDecl(func);
    auto func_type = ident->Type()->ToFunc();
    assert(func_type->IsNonVoid());
    func_type->SetFuncDecl(func);

    m_curscope->Insert(ident->Name(), ident);  // Nonvoid
    debug("tu%#X: insert nonvoid func " ANSI_GREEN "%s\n" ANSI_RESET,
          m_curscope, ident->Name().c_str());

    auto scope_type = func_type->IsChar() ? ScopeType::S_CHAR_FUNC :
                                            ScopeType::S_INT_FUNC;
    m_curscope = new Scope(m_curscope, scope_type);
    m_curscope->Insert(ident->Name(), ident);  // Nonvoid

    AssertFront(LPARENT);
    ParseFunctionParamList(func);
    AssertFront(RPARENT);
    AssertFront(LBRACE);
    func->SetBody(ParseCompoundStmt());
    AssertFront(RBRACE);

    if (!m_curscope->RetFlag()) {
        Error(m_ts.prev(), 'h');
    }

    func->SetScope(m_curscope);
    // m_curscope->Peek();

    m_curfunc = nullptr;
    // Scope *temp = m_curscope;
    m_curscope = m_curscope->parent();
    // debug("scope now is %#X\n", m_curscope);
    // delete temp;

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("有返回值函数定义");
#endif
    return func;
}

/*
 * @复合语句
 * @Grammar:
 *   [<常量说明>][<变量说明>]<语句列>
 * @References: <主函数> <有返回值函数定义> <无返回值函数定义>
 * @First = {First(常量说明), First(变量说明), First(语句), '}'}
 * @Follow = {'}'}
 */
CompoundStmt *Parser::ParseCompoundStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("复合语句");
#endif
    auto cstmt = CompoundStmt::New(m_curscope);

    if (MatchFront(CONSTTK)) {
        ParseConstVarDecl(cstmt);
    }
    if (MatchFront(INTTK) ||
        MatchFront(CHARTK)) {             // forwarding scan: DO NOT PRINT
        if (m_ts.GetType(1) == IDENFR) {  // int/char << identifier <<
            if (m_ts.GetType(2) == SEMICN || m_ts.GetType(2) == COMMA ||
                m_ts.GetType(2) == LBRACK) {  // int/char << identifier << ;
                ParseVarDecl(cstmt);
            }
            else {
                Assert(2, SEMICN);
                if (m_ts.GetType(2) == INTTK || m_ts.GetType(2) == CHARTK) {
                    m_ts.FlushFront();
                    m_ts.FlushFront();
                    ParseVarDecl(cstmt);
                }
                // m_ts.clear();
            }
        }
        // TODO error handling not identifier
    }
    cstmt->SetStmtList(ParseStmtList());
    // assert(cstmt->Size() > 0);

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("复合语句");
#endif
    return cstmt;
}

/*
 * @语句列
 * @Grammar:
 *   {<语句>}
 * @References: <复合语句> <语句>
 * @First = {First(语句)={if, while, do, for, '{', IDENFR, printf, scanf,
 * return, ;}, epsilon}
 * @Follow = {'}'}
 */
StmtList Parser::ParseStmtList()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("语句列");
#endif
    auto ret = StmtList();
    auto type = NATK;

    while ((type = m_ts.GetFrontType()) != RBRACE) {
        switch (type) {
        case IFTK:
        case WHILETK:
        case DOTK:
        case FORTK:
        case LBRACE:
        case IDENFR:
        case PRINTFTK:
        case SCANFTK:
        case RETURNTK:
        case SEMICN:
#ifdef EXTRA_SWITCH
        case SWITCHTK:
#endif  // EXTRA_SWITCH
        {
            auto stmt = ParseStmt();
            // assert(stmt != nullptr);
            if (stmt)
                ret.push_back(stmt);
            break;
        }

        case RBRACE:
            debug("right brace\n");
            break;
        default:

            // TODO error handling
            break;
        }
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("语句列");
#endif
    return ret;
}

/*
 * @语句
 * @Grammar:
 *   <条件语句>|<循环语句>|'{'<语句列>'}'|<有返回值函数调用语句>;|<空>;|
 *   <无返回值函数调用语句>;|<赋值语句>;|<读语句>;|<写语句>;|<返回语句>;
 * @References: <循环语句> <条件语句> <语句列>
 * @First(语句) = {if, while, do, for, '{', IDENFR, printf, scanf, return, ;}
 * @Follow = {'}', ...}
 */
Stmt *Parser::ParseStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("语句");
#endif
    Stmt *ret = nullptr;
    switch (m_ts.GetFrontType()) {
#ifdef EXTRA_SWITCH
    case SWITCHTK:
        return ParseSwitchStmt();
#endif  // EXTRA_SWITCH
    case IFTK:
        ret = ParseIfStmt();
        break;
    case WHILETK:
    case DOTK:
    case FORTK:
        ret = ParseLoopStmt();
        break;

    case LBRACE: {
        m_ts.PrintFront();
        auto cstmt = CompoundStmt::New(m_curscope);
        cstmt->SetStmtList(ParseStmtList());
        ret = cstmt;
        AssertFront(RBRACE);
        break;
    }
    case PRINTFTK:
        ret = ParsePrintStmt();
        AssertFront(SEMICN);
        break;
    case SCANFTK:
        ret = ParseScanfStmt();
        AssertFront(SEMICN);
        break;
    case RETURNTK:
        ret = ParseReturnStmt();
        AssertFront(SEMICN);
        break;
    case IDENFR: {
        auto ident = m_curscope->Find(m_ts.GetName(0));
        if (ident == nullptr) {
            Error(m_ts[0], 'c');
            debug("curscope: %#X\n", m_curscope);
            while (m_ts.GetType(0) != SEMICN) {
                m_ts.FlushFront();
            }
        }
        else if (!CheckTokens(2, IDENFR, ASSIGN) ||
                 !CheckTokens(2, IDENFR, LBRACK)) {  // a = 1 or a[xx] = 1
            ret = ParseAssignStmt();
            AssertFront(SEMICN);
        }
        else if (!CheckTokens(2, IDENFR, LPARENT))  // a(1)
        {
            auto func = ident->Type()->ToFunc();
            if (func->IsNonVoid() || ident->Type()->ToVoid()) {
                assert(ident->IsNonVoid());
                ret = ParseNonvoidFunctionCall();
            }
            else if (ident->IsVoid()) {
                ret = ParseVoidFunctionCall();
            }
            else {
                assert(0);
            }
            // TODO symbol table
            AssertFront(SEMICN);
        }
        else {
            Error("Drift identifier");
            return nullptr;
        }
        break;
    }

    case SEMICN: {
        m_ts.PrintFront();
        ret = NullStmt::New();
        break;
    }
    case LPARENT:  // (
    case RPARENT:  // )
    case NATK:
    case EOFTK:
    case INTCON:
    case CHARCON:
    case STRCON:
    case CONSTTK:
    case INTTK:
    case CHARTK:
    case VOIDTK:  // void
    case MAINTK:  // main
    case ELSETK:  // else
    case PLUS:    // +
    case MINU:    // -
    case MULT:    // *
    case DIV:     // /
    case LSS:     // <
    case LEQ:     // <=
    case GRE:     // >
    case GEQ:     // >=
    case EQL:     // ==
    case NEQ:     // !=
    case ASSIGN:  // =
    case COMMA:   // ,
    case LBRACK:  // [
    case RBRACK:  // ]
    case RBRACE:  // }
    default:
        break;
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("语句");
#endif
    return ret;
}

/*
 * @因子
 * @Grammar:
 *   <标识符>｜<标识符>'['<表达式>']'|'('<表达式>')'｜
 *   <整数>|<字符>｜<有返回值函数调用语句>
 * @References: <项>
 * @First = {IDENFR, '(', INTCON, CHARCON}
 * @Follow = {+, -, *, /, ], compare_op}
 * @Return 1: int 2: char
 */
Expr *Parser::ParseFactor()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("因子");
#endif

    Expr *ret = nullptr;
    switch (m_ts.GetFrontType()) {
    case PLUS:  // TODO ?
    case MINU:
    case INTCON:
        ret = ParseIntegerLiteral();
        break;
    case CHARCON:
        ret = ParseCharLiteral();
        break;
    case LPARENT:
        m_ts.PrintFront();
        ret = ParseExpr();
        AssertFront(RPARENT);
        break;
    case IDENFR:
        // function call
        if (!CheckTokens(2, IDENFR, LPARENT)) {
            ret = ParseNonvoidFunctionCall();
            // assert(ret != nullptr);
        }
        // array[expr], variable
        else {
            auto tok = AssertFront(IDENFR);
            auto ident = m_curscope->Find(tok->m_value);
            if (ident == nullptr) {
                Error(tok, 'c');
                // ret = nullptr;
            }
            else {
                // array
                if (MatchFront(LBRACK)) {
                    m_ts.PrintFront();
                    auto expr = ParseExpr();
                    if (!expr->IsInt()) {
                        Error(m_ts.prev(), 'i');
                    }
                    AssertFront(RBRACK);
                    // FIXME: shouldn't use IsInt()
                    auto expr_type =
                        ident->IsInt() ? ExprType::T_INT : ExprType::T_CHAR;
                    auto elem_type = QualType(ArithmType::New(expr_type));
                    auto type = ArrayType::New(expr, elem_type);
                    ret = ArraySubscriptExpr::New(ident, expr, type, false);
                    // ret = BinaryOp::New(lb_tok, ident, expr);
                }
                // regular variable
                else {
                    ret = ident;
                }
            }
        }
        break;
    default:
        // TODO error handling
        break;
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("因子");
#endif
    return ret;
}

/*
 * @项
 * @Grammar:
 *   <因子>{<乘法运算符><因子>}
 * @References: <表达式>
 * @First = {First(因子)={}}
 * @Follow = {'+', -, *, /}
 * @AST: 3 * 2 / 1
 *  BinaryOperator 'int' '/'
 *  |-BinaryOperator 'int' '*'
 *  | |-IntegerLiteral 'int' 3
 *  | `-IntegerLiteral 'int' 2
 *  `-IntegerLiteral 'int' 1
 */
Expr *Parser::ParseTerm()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("项");
#endif

    auto lhs = ParseFactor();
    // assert(lhs != nullptr);
    TokenType type = m_ts.GetFrontType();
    while (Token::IsMultDivOp(type)) {
        auto tok = m_ts.PrintFront();
        auto rhs = ParseFactor();
        auto bop = BinaryOp::New(tok, lhs, rhs);
        type = m_ts.GetFrontType();
        lhs = bop;
        // ret = 1;
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("项");
#endif
    return lhs;
}

/*
 * @表达式
 * @Grammar:
 *   [+|-]<项>{<加法运算符><项>}
 * @References: <循环语句> <条件语句>
 * @First(表达式) = {+, -, IDENFR, INTCON, CHARCON}
 * @Follow = {')', ';'}
 * @AST: +3 * 2 / 1 + 1 / 8 + 9;
 * BinaryOperator 'int' '+'
 * |-BinaryOperator 'int' '+'
 * | |-BinaryOperator 'int' '/'
 * | | |-BinaryOperator 'int' '*'
 * | | | |-UnaryOperator 'int' prefix '+'
 * | | | | `-IntegerLiteral 'int' 3
 * | | | `-IntegerLiteral 'int' 2
 * | | `-IntegerLiteral 'int' 1
 * | `-BinaryOperator 'int' '/'
 * |   |-IntegerLiteral 'int' 1
 * |   `-IntegerLiteral 'int' 8
 * `-IntegerLiteral 'int' 9
 *
 *  (a + int1Function1(b));
 */
Expr *Parser::ParseExpr()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("表达式");
#endif

    // auto expr = new Expr(m_ts[0], ExprType::T_INVALID);
    Token *uop_tok = nullptr;

    if (Token::IsPlusMinusOp(m_ts.GetFrontType())) {
        uop_tok = m_ts.PrintFront();
    }
    auto lhs = ParseTerm();
    if (uop_tok) {
        auto qt = QualType(lhs->Type());
        auto uop = UnaryOp::New(uop_tok, lhs, qt);  // TODO: add qual type
        lhs = uop;
    }
    // expr->SetType((lhs->Type()->ToArithm()->Tag()));

    while (Token::IsPlusMinusOp(m_ts.GetFrontType())) {
        auto bop_tok = m_ts.PrintFront();
        auto rhs = ParseTerm();
        // assert(rhs && lhs);
        if (!rhs || !lhs) {
            return nullptr;
        }
        auto bop = BinaryOp::New(bop_tok, lhs, rhs);
        lhs = bop;
    }
    // expr->SetType(ExprType::T_INT);  // more than 1 term => int

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("表达式");
#endif

    return lhs;
}

/*
 * @条件
 * @Grammar:
 *   <表达式><关系运算符><表达式> |<表达式>
 * @References: <循环语句> <条件语句>
 * @First = {First(表达式)={+, -, IDENFR, INTCON, CHARCON}}
 * @Follow = {')', ';'}
 */
Condition *Parser::ParseCondition()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("条件");
#endif

    Condition *cond = nullptr;
    auto lhs = ParseExpr();
    if (lhs == nullptr) {
        while (m_ts.GetFrontType() != SEMICN &&
               m_ts.GetFrontType() != RPARENT) {
            m_ts.FlushFront();
        }
        return nullptr;
    }
    if (!lhs->IsInt()) {
        Error(m_ts.prev(), 'f');
    }
    if (Token::IsCmpOp(m_ts.GetFrontType())) {
        auto bop_tok = m_ts.PrintFront();
        auto rhs = ParseExpr();
        if (rhs == nullptr) {
            while (m_ts.GetFrontType() != SEMICN &&
                   m_ts.GetFrontType() != RPARENT) {
                m_ts.FlushFront();
            }
            return nullptr;
        }
        if (!rhs->IsInt()) {
            Error(bop_tok, 'f');
        }
        cond = Condition::New(lhs, bop_tok, rhs);
        // lhs = cond;
    }
    else {
        // Error(m_ts[0], 'a'); // ???
        cond = Condition::New(lhs);
        assert(!cond->IsBinary());
        while (m_ts.GetType(0) != RPARENT && m_ts.GetType(0) != SEMICN) {
            m_ts.FlushFront();
        }
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("条件");
#endif
    assert(cond);
    return cond;
}

/*
 * @条件语句
 * @Grammar:
 *   if '('<条件>')'<语句>[else<语句>]
 * @References: <语句>
 * @First = {if}
 * @Follow = {'}'}
 */
IfStmt *Parser::ParseIfStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("条件语句");
#endif

    AssertFront(IFTK);
    AssertFront(LPARENT);
    auto condi = ParseCondition();
    AssertFront(RPARENT);
    auto then_stmt = ParseStmt();
    Stmt *else_stmt = nullptr;
    if (MatchFront(ELSETK)) {
        m_ts.PrintFront();
        else_stmt = ParseStmt();
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("条件语句");
#endif
    return IfStmt::New(condi, then_stmt, else_stmt);
}

/*
 * @*for语句
 * @Grammar:
 *   for'('<标识符>=<表达式>;<条件>;<标识符>=<标识符>(+|-)<步长>')'<语句>
 * @References: <语句>
 * @First(语句) = {for}
 * @Follow(循环语句) = {Follow(语句)}
 */
ForStmt *Parser::ParseForStmt()
{
    Identifier *init_ident = nullptr, *inc_rhs = nullptr, *inc_lhs = nullptr;

    AssertFront(TokenType::FORTK);         // for
    AssertFront(LPARENT);                  // (
    if (auto tok = AssertFront(IDENFR)) {  // i
        init_ident = m_curscope->Find(tok->m_value);
        if (init_ident == nullptr) {
            Error(tok, 'c');
        }
        else if (init_ident->IsConstQualified()) {
            Error(tok, 'j');
        }
    }
    AssertFront(ASSIGN);           // =
    auto init_expr = ParseExpr();  // 1*2
    AssertFront(SEMICN);           // ;

    auto cond = ParseCondition();  // i < 5
    AssertFront(SEMICN);           // ;

    if (auto tok = AssertFront(IDENFR)) {  // i
        inc_lhs = m_curscope->Find(tok->m_value);
        if (inc_lhs == nullptr) {
            Error(tok, 'c');
        }
        else if (inc_lhs->IsConstQualified()) {
            Error(tok, 'j');
        }
    }
    AssertFront(ASSIGN);                   // =
    if (auto tok = AssertFront(IDENFR)) {  // i
        inc_rhs = m_curscope->Find(tok->m_value);
        if (inc_rhs == nullptr) {
            Error(tok, 'c');
        }
    }
    auto pmop = ParsePlusMinuOperator();
    // assert(pmop > 0);                // +/-
    auto inc = ParseForIncrement();  // 1
    if (pmop == TokenType::MINU) {
        // inc->SetVal(-inc->Val());
        inc = IntegerLiteral::New(nullptr, ExprType::T_INT, -inc->Val());
    }
    AssertFront(RPARENT);  // )
    auto body = ParseStmt();

    // assert(cond != nullptr);
    // assert(body != nullptr);
    return ForStmt::New(init_ident, init_expr, cond, inc_lhs, inc_rhs, inc,
                        body);
}

/*
 * @*while语句
 * @Grammar:
 *   while '('<条件>')'<语句>
 * @References: <语句>
 * @First(语句) = {while}
 * @Follow(循环语句) = {Follow(语句)}
 */
WhileStmt *Parser::ParseWhileStmt()
{
    AssertFront(TokenType::WHILETK);  // while
    AssertFront(LPARENT);             // (
    auto cond = ParseCondition();     // a[i]>0
    AssertFront(RPARENT);             // )
    auto body = ParseStmt();          // {a[i] = a[i] - 1;}

    // assert(cond != nullptr);
    // assert(body != nullptr);
    return WhileStmt::New(cond, body);
}

/*
 * @*do-while语句
 * @Grammar:
 *   do<语句>while '('<条件>')'
 * @References: <语句>
 * @First(语句) = {do}
 * @Follow(循环语句) = {Follow(语句)}
 */
DoStmt *Parser::ParseDoStmt()
{
    AssertFront(TokenType::DOTK);  // do
    auto body = ParseStmt();       // i = i + 1
    AssertFront(WHILETK);          // while
    if (MatchFront(LPARENT)) {
        m_ts.PrintFront();  // (
    }
    Condition *cond = nullptr;
    if (MatchFront(PLUS) || MatchFront(MINU) || MatchFront(IDENFR) ||
        MatchFront(INTCON) || MatchFront(CHARCON) || MatchFront(LPARENT)) {
        cond = ParseCondition();  // i < 5
        if (cond == nullptr) {
            if (MatchFront(RPARENT)) {
                m_ts.FlushFront();  // )
            }
        }
    }
    if (MatchFront(RPARENT)) {
        m_ts.PrintFront();  // )
    }

    assert(body != nullptr);
    // assert(cond != nullptr);

    return DoStmt::New(body, cond);
}

/*
 * @循环语句
 * @Grammar:
 *   while '('<条件>')'<语句>| do<语句>while '('<条件>')' |
 *   for'('<标识符>=<表达式>;<条件>;<标识符>=<标识符>(+|-)<步长>')'<语句>
 * @References: <语句>
 * @First(语句) = {while, do, for}
 * @Follow(循环语句) = {Follow(语句)}
 */
Stmt *Parser::ParseLoopStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("循环语句");
#endif
    Stmt *ret = nullptr;
    switch (m_ts.GetFrontType()) {
    case DOTK:
        ret = ParseDoStmt();
        break;
    case WHILETK:
        ret = ParseWhileStmt();
        break;
    case FORTK:
        ret = ParseForStmt();
        break;
    default:
        Error("unknown loop statement: %s\n", m_ts[0]->m_value.c_str());
        break;
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("循环语句");
#endif
    return ret;
}

/*
 * @写语句
 * @Grammar:
 *   printf '(' <字符串>,<表达式> ')'|
 *   printf '('<字符串> ')'| printf '('<表达式>')'
 * @References: <语句>
 * @First(语句) = {printf}
 * @Follow = {';'}
 */
PrintStmt *Parser::ParsePrintStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("写语句");
#endif
    Expr *expr = nullptr;
    StringLiteral *sl = nullptr;

    AssertFront(PRINTFTK);
    AssertFront(LPARENT);
    if (MatchFront(STRCON)) {
        sl = ParseString();
        if (MatchFront(COMMA)) {
            m_ts.PrintFront();
            expr = ParseExpr();
        }
        AssertFront(RPARENT);
    }
    else {
        expr = ParseExpr();
        AssertFront(RPARENT);
    }

    PrintStmt *ret = nullptr;

    if (expr != nullptr && sl != nullptr) {
        ret = PrintStmt::New(sl, expr);
    }
    else if (expr) {
        ret = PrintStmt::New(expr);
    }
    else if (sl) {
        ret = PrintStmt::New(sl);
    }
    else {
        Error("ParsePrintStmt: not found expr nor string literal.");
        while (m_ts.GetFrontType() != SEMICN) {
            m_ts.FlushFront();
        }
        return nullptr;
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("写语句");
#endif
    assert(ret);
    return ret;
}

/*
 * @读语句
 * @Grammar:
 *   scanf '('<标识符>{,<标识符>}')'
 * @References: <语句>
 * @First(读语句) = {scanf}
 * @Follow = {';'}
 */
ScanfStmt *Parser::ParseScanfStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("读语句");
#endif

    ScanfStmt::IdentList idents;

    AssertFront(SCANFTK);
    if (!MatchFront(LPARENT)) {
        Error("should be '('");
    }
    do {
        m_ts.PrintFront();
        if (auto tok = AssertFront(IDENFR)) {  // i
            auto ident = m_curscope->Find(tok->m_value);
            if (ident == nullptr) {
                Error(tok, 'c');
            }
            else {
                idents.push_back(ident);
            }
        }
    } while (MatchFront(COMMA));
    AssertFront(RPARENT);

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("读语句");
#endif
    return ScanfStmt::New(idents);
}

/*
 * @返回语句
 * @Grammar:
 *   return['('<表达式>')']
 * @References: <语句>
 * @First(返回语句) = {return}
 * @Follow = {';'}
 */
ReturnStmt *Parser::ParseReturnStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("返回语句");
#endif

    ReturnStmt *rstmt = nullptr;

    AssertFront(RETURNTK);
    if (MatchFront(LPARENT)) {
        /* error g */
        if (m_curscope->Type() == S_VOID_FUNC) {
            Error(m_ts[0], 'g');
        }
        m_ts.PrintFront();
        auto expr = ParseExpr();
        assert(expr != nullptr);
        if (m_curscope->Type() == S_INT_FUNC) {
            if (!expr->IsInt()) {
                Error(m_ts.prev(), 'h');
            }
            else {
                m_curscope->SetRetFlag(1);
            }
        }
        else if (m_curscope->Type() == S_CHAR_FUNC) {
            if (!expr->IsChar()) {
                Error(m_ts.prev(), 'h');
            }
            else {
                m_curscope->SetRetFlag(1);
            }
        }
        AssertFront(RPARENT);
        assert(m_curfunc);
        rstmt = ReturnStmt::New(m_curfunc, expr);
    }
    else if (MatchFront(SEMICN)) {
        if (m_curscope->Type() == S_INT_FUNC ||
            m_curscope->Type() == S_CHAR_FUNC) {
            Error(m_ts[0], 'h');
        }
        assert(m_curfunc);
        rstmt = ReturnStmt::New(m_curfunc);
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("返回语句");
#endif
    assert(rstmt);
    return rstmt;
}

/*
 * @赋值语句
 * @Grammar:
 *   <标识符>=<表达式>|<标识符>'['<表达式>']'=<表达式>
 * @References: <语句>
 * @First(语句) = {IDENFR}
 * @Follow = {';'}
 */
BinaryOp *Parser::ParseAssignStmt()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("赋值语句");
#endif

    auto lhs_tok = AssertFront(IDENFR);
    Expr *lhs = nullptr;
    Identifier *ident = nullptr;

    if (lhs_tok != nullptr) {
        ident = m_curscope->Find(lhs_tok->m_value);
        lhs = ident;
        // undefined identifier
        if (lhs == nullptr) {
            Error(m_ts.prev(), 'c');
        }
        // lhs can't be const qualified
        else if (lhs->IsConstQualified()) {
            Error(m_ts.prev(), 'j');
        }
    }
    if (MatchFront(LBRACK)) {
        m_ts.PrintFront();
        auto subscript_expr = ParseExpr();
        if (!subscript_expr->IsInt()) {
            Error(m_ts.prev(), 'i');
        }
        AssertFront(RBRACK);
        auto expr_type = ident->IsInt() ? ExprType::T_INT : ExprType::T_CHAR;
        auto elem_type = QualType(ArithmType::New(expr_type));
        auto type = ArrayType::New(subscript_expr, elem_type);

        lhs = ArraySubscriptExpr::New(ident, subscript_expr, type, true);
        // lhs = BinaryOp::New(lbrack_tok, lhs, subscript_expr);
    }
    auto ass_tok = AssertFront(ASSIGN);
    auto rhs = ParseExpr();
    if (rhs == nullptr) {
        while (m_ts.GetFrontType() != SEMICN) {
            m_ts.FlushFront();
        }
        return nullptr;
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("赋值语句");
#endif
    assert(lhs);
    assert(ass_tok);
    assert(rhs);
    return BinaryOp::New(ass_tok, lhs, rhs);
}

/*
 * @值参数表
 * @Grammar:
 *   <表达式>{,<表达式>}｜<空>
 * @References: <函数调用语句>
 * @First = {First(表达式)={+, -, IDENFR, INTCON, CHARCON}}
 * @Follow = {')'}
 * @Emit: (push <expr>)*
 */
CallExpr::ArgList Parser::ParseValueParamList(Identifier *ident)
{
    return ParseValueParamList(ident->Type()->ToFunc()->GetFuncDecl());
}

CallExpr::ArgList Parser::ParseValueParamList(FunctionDecl *func)
{
    CallExpr::ArgList arglist;
    if (MatchFront(RPARENT)) {
        return arglist;
    }
    // assert(MatchFront(RPARENT));
    assert(func != nullptr);
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("值参数表");
#endif

    int i = 0;
    for (;;) {
        auto expr = ParseExpr();
        // assert(expr != nullptr);
        if (expr == nullptr) {
            while (m_ts.GetFrontType() != RPARENT) {
                m_ts.FlushFront();
            }
            return arglist;
        }
        arglist.push_back(expr);
        // still have params left to match
        if (func->ParamNum() > i) {
            auto param_type = func->m_params[i]->Type()->ToArithm();
            if (!(expr->IsInt() && param_type->IsInteger()) &&
                !(expr->IsChar() && param_type->IsChar())) {
                Error("internal%s: %d %d\n %d %d\n",
                      expr->Tok()->m_value.c_str(), expr->IsInt(),
                      expr->IsChar(), param_type->IsInteger(),
                      param_type->IsChar());
                Error(m_ts.prev(), 'e');
            }
        }
        else {  // too much params!
            Error("too much parameters");
            Error(m_ts.prev(), 'd');
        }
        ++i;

        if (!MatchFront(COMMA)) {
            break;
        }
        m_ts.PrintFront();
    }
    if (func != nullptr && func->m_params.size() > i) {  // match unfinished
        Error("not enough parameters");
        Error(m_ts.prev(), 'd');
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("值参数表");
#endif
    return arglist;
}

/*
 * @无返回值函数调用语句
 * @Grammar:
 *   <标识符>'('<值参数表>')'
 * @References:
 * @First = {}
 * @Follow = {}
 */
CallExpr *Parser::ParseVoidFunctionCall()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("无返回值函数调用语句");
#endif

    Identifier *ident = nullptr;
    FunctionDecl *func = nullptr;

    auto tok = AssertFront(IDENFR);
    if (tok) {
        ident = m_curscope->Find(tok->m_value);
        debug("cur: %#X\n\n", m_curscope);
        if (ident == nullptr) {  // undefined void function
            Error(tok, 'c');
        }
        else {
            assert(ident->IsVoid());
            assert(ident->Type()->ToFunc());
            func = ident->Type()->ToFunc()->GetFuncDecl();
            assert(func != nullptr);
        }
    }
    AssertFront(LPARENT);
    auto args = ParseValueParamList(ident);
    AssertFront(RPARENT);

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("无返回值函数调用语句");
#endif
    assert(ident);
    return CallExpr::New(ident, args);
}

/*
 * @有返回值函数调用语句
 * @Grammar:
 *   <标识符>'('<值参数表>')'
 * @References: <因子>
 * @First = {}
 * @Follow = {}
 */
CallExpr *Parser::ParseNonvoidFunctionCall()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("有返回值函数调用语句");
#endif
    CallExpr::ArgList args;
    auto tok = AssertFront(IDENFR);
    auto ident = m_curscope->Find(tok->m_value);
    if (ident == nullptr) {
        Error(tok, 'c');
        return nullptr;
    }
    else {
        AssertFront(LPARENT);
        assert(ident->Type()->ToFunc() != nullptr);
        assert(ident->Type()->ToFunc()->GetFuncDecl() != nullptr);
        assert(ident->IsNonVoid());
        args = ParseValueParamList(ident);
        AssertFront(RPARENT);
    }

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("有返回值函数调用语句");
#endif
    // assert(ident);
    return CallExpr::New(ident, args);
}

/*
 * @主函数
 * @Grammar:
 *   void main'('')' '{'<复合语句>'}'
 * @References: <程序>
 * @First = {void}
 * @Follow = {epsilon}
 */
FunctionDecl *Parser::ParseMain()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("主函数");
#endif

    AssertFront(VOIDTK);  // void
    Identifier *main_ident = nullptr;
    if (MatchFront(MAINTK)) {
        auto dtype = ArithmType::New(ExprType::T_VOID);
        auto type = FuncType::New(dtype, F_NORETURN);
        main_ident = Identifier::New(m_ts[0], QualType(type));
        // assert(ident->Type()->ToVoid() != nullptr);
        m_curscope->Insert(m_ts.GetName(0), main_ident);  // void
        debug("insert void main\n", m_ts.GetName(0).c_str());
    }
    // m_curscope->Peek();
    m_curscope = new Scope(m_curscope, S_VOID_FUNC);
    debug("main scope: %#X\n", m_curscope);

    auto ret = FunctionDecl::New(main_ident, m_curscope);
    main_ident->SetDecl(ret);
    m_curfunc = ret;

    AssertFront(MAINTK);   // main
    AssertFront(LPARENT);  // (
    AssertFront(RPARENT);  // )
    AssertFront(LBRACE);   // {
    ret->SetBody(ParseCompoundStmt());
    AssertFront(RBRACE);  // }

    // m_curscope->Peek();

    // Scope *temp = m_curscope;
    m_curscope = m_curscope->parent();
    debug("scope now is %#X\n", m_curscope);
    // delete temp;
    m_curfunc = nullptr;

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("主函数");
#endif
    return ret;
}

/*
 * @程序
 * @Grammar:
 *   [<常量说明>] [<变量说明>]
 *   {<有返回值函数定义> | <无返回值函数定义>} <主函数>
 * @References:
 * @First = {}
 * @Follow = {}
 */
TranslationUnitDecl *Parser::ParseTranslationUnitDecl()
{
#ifdef DEBUG_PARSE_BEGIN
    DEBUG_PARSE_BEGIN("程序");
#endif

    if (MatchFront(CONSTTK)) {
        ParseConstVarDecl();
    }
    // forwarding scan: DO NOT PRINT
    if (MatchFront(INTTK) || MatchFront(CHARTK)) {
        // int/char << identifier <<
        if (m_ts.GetType(1) == IDENFR) {
            // int/char << identifier << ;
            if (m_ts.GetType(2) == SEMICN || m_ts.GetType(2) == COMMA ||
                m_ts.GetType(2) == LBRACK) {
                ParseVarDecl();
            }
            // int/char << identifier << (
            else if (m_ts.GetType(2) == LPARENT) {
                m_unit->Add(ParseNonvoidFunctionDecl());
            }
            // TODO error handling
        }
        // TODO error handling not identifier
    }
    while (MatchFront(VOIDTK) || MatchFront(INTTK) || MatchFront(CHARTK)) {
        if (MatchFront(VOIDTK)) {
            if (m_ts.GetType(1) == MAINTK) {
                break;
            }
            else {
                m_unit->Add(ParseVoidFunctionDecl());
            }
        }
        else {
            m_unit->Add(ParseNonvoidFunctionDecl());
        }
    }
    m_unit->Add(ParseMain());
    m_unit->SetScope(m_curscope);
    // m_curscope->Peek();

#ifdef DEBUG_PARSE_END
    DEBUG_PARSE_END("程序");
#endif
    return m_unit;
}

#ifdef DEBUG_PARSE_BEGIN
#undef DEBUG_PARSE_BEGIN
#endif

#ifdef DEBUG_PARSE_END
#undef DEBUG_PARSE_END
#endif
