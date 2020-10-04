#ifndef C0C_AST_H
#define C0C_AST_H

#include "error.h"
#include "memory_pool.h"
#include "token.h"
#include "type.h"

#include <cassert>
#include <list>
#include <memory>
#include <set>
#include <string>

class Visitor;
template <typename T>
class Evaluator;
class AddrEvaluator;
class CodeGenerator;

class Scope;
class Parser;

// Statements
class Stmt;
class NullStmt;
class DeclStmt;
class ScanfStmt;
class PrintStmt;
class LabelStmt;
class IfStmt;
class ForStmt;
class DoStmt;
class WhileStmt;
class ReturnStmt;
class JumpStmt;
class CompoundStmt;

// Expressions
class Expr;
class BinaryOp;
class Condition;
class UnaryOp;
class CallExpr;
class ArraySubscriptExpr;
class TempVar;  // depracated
class IntegerLiteral;
class StringLiteral;
class CharLiteral;
class Identifier;
struct Initializer;

// Declarations
class Decl;
class TranslationUnitDecl;
class NamedDecl;
class FunctionDecl;
class VarDecl;
class ParmVarDecl;

using StmtList = std::list<Stmt *>;
using DeclList = std::list<Decl *>;

/*
 * Abstract Syntax Tree Node
 */
class ASTNode
{
public:
    virtual ~ASTNode() {}
    virtual void Accept(Visitor *v) = 0;

protected:
    ASTNode() {}

    MemoryPool *m_pool{nullptr};
};

/*
 * Statements
 */

class Stmt : public ASTNode
{
public:
    virtual ~Stmt() {}

protected:
    Stmt() {}
};

class NullStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static NullStmt *New();
    virtual ~NullStmt() {}
    virtual void Accept(Visitor *v);

protected:
    NullStmt() {}
};

class DeclStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static DeclStmt *New(Decl *decl = nullptr);
    virtual ~DeclStmt() {}
    virtual void Accept(Visitor *v);

    Decl *GetDecl() const
    {
        return m_decl;
    }
    FunctionDecl *GetFuncDecl()
    {
        return reinterpret_cast<FunctionDecl *>(m_decl);
    }

protected:
    DeclStmt(Decl *decl = nullptr) : m_decl(decl) {}
    DeclStmt(Decl *decl, SourceLocation start, SourceLocation end)
        : m_startloc(start), m_endloc(end), m_decl(decl)
    {
    }

    SourceLocation m_startloc, m_endloc;
    Decl *m_decl;
};

/**
 * Compound Statement
 */
class CompoundStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static CompoundStmt *New(StmtList &stmts, ::Scope *scope = nullptr);
    static CompoundStmt *New(::Scope *scope = nullptr);
    virtual ~CompoundStmt() {}
    virtual void Accept(Visitor *v);
    StmtList &Stmts()
    {
        return m_stmts;
    }
    DeclList &Decls()
    {
        return m_decls;
    }
    ::Scope *Scope()
    {
        return m_scope;
    }
    void AddVarDecl(Decl *vd)
    {
        AddDecl(vd);
    }
    void AddDecl(Decl *vd)
    {
        if (vd == nullptr)
            return;
        m_decls.push_back(vd);
    }
    void AddStmt(Stmt *stmt)
    {
        if (stmt == nullptr)
            return;
        m_stmts.push_back(stmt);
    }
    void SetStmtList(StmtList stmts)
    {
        m_stmts.splice(m_stmts.end(), stmts);
        // m_stmts = stmts;
    }
    size_t Size()
    {
        return m_stmts.size() + m_decls.size();
    }
    size_t StmtSize()
    {
        return m_stmts.size();
    }
    size_t DeclSize()
    {
        return m_decls.size();
    }

protected:
    CompoundStmt(const StmtList &stmts, ::Scope *scope = nullptr)
        : m_stmts(stmts), m_scope(scope)
    {
    }
    CompoundStmt(::Scope *scope = nullptr) : m_scope(scope) {}

private:
    DeclList m_decls;
    StmtList m_stmts;
    ::Scope *m_scope;
};

class LabelStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static LabelStmt *New();
    static LabelStmt *New(std::string &label);
    ~LabelStmt() {}
    virtual void Accept(Visitor *v);
    std::string Repr() const
    {
        if (m_tag) {
            return "$BB_" + std::to_string(m_tag);
        }
        else {
            return m_str;
        }
    }

protected:
    LabelStmt() : m_tag(GenTag()) {}
    LabelStmt(std::string &label) : m_tag(0), m_str(label) {}

private:
    static int GenTag()
    {
        static int tag = 0;
        return ++tag;
    }

    int m_tag;
    std::string m_str;
};

class ScanfStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    using IdentList = std::list<Identifier *>;
    static ScanfStmt *New(IdentList &idents);
    static ScanfStmt *New();
    virtual ~ScanfStmt() {}
    virtual void Accept(Visitor *v);

protected:
    ScanfStmt(IdentList &idents) : m_idents(idents) {}
    ScanfStmt() {}

    IdentList m_idents;
};

class PrintStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static PrintStmt *New(Expr *expr);
    static PrintStmt *New(StringLiteral *sl, Expr *expr = nullptr);
    // static PrintStmt *New(StringLiteral *sl);
    virtual ~PrintStmt() {}
    virtual void Accept(Visitor *v);
    Expr *Exp() const
    {
        return m_expr;
    }
    StringLiteral *Str() const
    {
        return m_str;
    }

protected:
    // PrintStmt(Expr *expr) : m_expr(expr) {}
    PrintStmt(StringLiteral *sl, Expr *expr = nullptr) : m_str(sl), m_expr(expr)
    {
    }
    PrintStmt(Expr *expr) : m_str(nullptr), m_expr(expr) {}
    // PrintStmt(StringLiteral *sl) : m_str(sl) {}

    StringLiteral *m_str;
    Expr *m_expr;
};

/*
 * if (<cond-expr>)
 *   <then-stmt>
 * else
 *   <else-stmt>
 */
class IfStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static IfStmt *New(Condition *cond, Stmt *then, Stmt *els = nullptr);
    virtual ~IfStmt() {}
    virtual void Accept(Visitor *v);

protected:
    IfStmt(Condition *cond, Stmt *then, Stmt *els = nullptr)
        : m_cond(cond), m_then(then), m_else(els)
    {
    }

private:
    Condition *m_cond;
    Stmt *m_then;
    Stmt *m_else;
};

class ForStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static ForStmt *New(Identifier *init_ident,
                        Expr *init_expr,
                        Expr *cond,
                        Identifier *inc_lhs,
                        Identifier *inc_rhs,
                        IntegerLiteral *inc,
                        Stmt *body);
    virtual ~ForStmt() {}
    virtual void Accept(Visitor *v);
    Expr *Cond() const
    {
        return m_cond;
    }
    Stmt *Body() const
    {
        return m_body;
    }

protected:
    ForStmt(Identifier *init_ident,
            Expr *init_expr,
            Expr *cond,
            Identifier *inc_lhs,
            Identifier *inc_rhs,
            IntegerLiteral *inc,
            Stmt *body)
        : m_init_ident(init_ident), m_init_expr(init_expr), m_cond(cond),
          m_inc_lhs(inc_lhs), m_inc_rhs(inc_rhs), m_inc(inc), m_body(body)
    {
    }
    // for (m_init_ident = m_init_expr;
    //      m_cond;
    //      m_inc_lhs = m_inc_rhs + m_inc)
    //     m_body
    Stmt *m_init;  // depracated
    Identifier *m_init_ident;
    Expr *m_init_expr;
    Expr *m_cond;
    Identifier *m_inc_lhs, *m_inc_rhs;
    IntegerLiteral *m_inc;
    Stmt *m_body;
};

class WhileStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static WhileStmt *New(Condition *cond, Stmt *body);
    virtual ~WhileStmt() {}
    virtual void Accept(Visitor *v);
    Condition *Cond() const
    {
        return m_cond;
    }
    Stmt *Body() const
    {
        return m_body;
    }

protected:
    WhileStmt(Condition *cond, Stmt *body) : m_cond(cond), m_body(body) {}

    Condition *m_cond;
    Stmt *m_body;
};

class DoStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static DoStmt *New(Stmt *body, Condition *cond);
    virtual ~DoStmt() {}
    virtual void Accept(Visitor *v);
    Condition *Cond() const
    {
        return m_cond;
    }
    Stmt *Body() const
    {
        return m_body;
    }

protected:
    DoStmt(Stmt *body, Condition *cond) : m_body(body), m_cond(cond) {}

    Stmt *m_body;
    Condition *m_cond;
};

class JumpStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static JumpStmt *New(LabelStmt *label);
    virtual ~JumpStmt() {}
    virtual void Accept(Visitor *v);
    void SetLabel(LabelStmt *label)
    {
        m_label = label;
    }

protected:
    JumpStmt(LabelStmt *label) : m_label(label) {}

private:
    LabelStmt *m_label;
};

/*
 * return['('<表达式>')']
 */
class ReturnStmt : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static ReturnStmt *New(FunctionDecl *func, Expr *expr = nullptr);
    virtual ~ReturnStmt() {}
    virtual void Accept(Visitor *v);

protected:
    ReturnStmt(FunctionDecl *func, ::Expr *expr = nullptr)
        : m_func(func), m_expr(expr)
    {
    }

private:
    FunctionDecl *m_func;
    ::Expr *m_expr;
};

struct Initializer
{
    Initializer(Type *type, int offset, Expr *expr)
        : m_type(type), m_offset(offset), m_expr(expr)
    {
    }

    bool operator<(const Initializer &rhs) const;

    // It could be the object it self or, it will be the member
    // that was initialized
    Type *m_type;
    int m_offset;
    Expr *m_expr;
};

class Factor
{
    virtual ~Factor() {}

    virtual bool IsLVal();
    virtual void TypeChecking();
    bool IsVoid() const
    {
        return m_expr_type & ExprType::T_VOID;
    }

    bool IsInt() const
    {
        return m_expr_type & ExprType::T_INT;
    }
    bool IsChar() const
    {
        return m_expr_type & ExprType::T_CHAR;
    }
    bool IsNonVoid() const
    {
        return IsInt() || IsChar();
    }

protected:
    Factor(const Token *tok, ExprType expr_type = ExprType::T_INVALID)
        : m_tok(tok), m_expr_type(expr_type)
    {
    }

    const Token *m_tok;
    ExprType m_expr_type;
};

class Term
{
public:
    virtual ~Term() {}

    virtual bool IsLVal();
    virtual void TypeChecking();

    const Token *Tok() const
    {
        return m_tok;
    }
    void SetTok(const Token *tok)
    {
        m_tok = tok;
    }

    // static Term *MayCast(Term *expr);
    // static Term *MayCast(Term *expr, QualType desType);
    virtual bool IsNullPointerConstant() const
    {
        return false;
    }

    bool IsVoid() const
    {
        return m_expr_type & ExprType::T_VOID;
    }

    bool IsInt() const
    {
        return m_expr_type & ExprType::T_INT;
    }
    bool IsChar() const
    {
        return m_expr_type & ExprType::T_CHAR;
    }
    bool IsNonVoid() const
    {
        return IsInt() || IsChar();
    }

protected:
    // You can construct a expression without specifying a type,
    // then the type should be evaluated in TypeChecking()
    Term(const Token *tok, QualType type) : m_tok(tok) {}
    Term(const Token *tok, ExprType expr_type, QualType qual_type)
        : m_tok(tok), m_expr_type(expr_type)
    {
    }

    const Token *m_tok;
    ExprType m_expr_type;

private:
    struct FactorPair
    {
        Factor *m_factor;
        BinaryOp *m_bop;
    };
    using FactorPairList = std::deque<struct FactorPair>;
    FactorPairList m_factor_pair_list;
};

/*
 * Expr
 *  BinaryOp
 *  UnaryOp
 *  CallExpr
 *  IntegerLiteral
 *  StringLiteral
 *  CharLiteral
 *  Identifier
 *  TempVar
 */

class Expr : public Stmt
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;
    friend class LValGenerator;

public:
    virtual ~Expr() {}
    ::Type *Type()
    {
        return m_qual_type.GetPtr();
    }
    virtual bool IsLVal()
    {
        return false;
    }
    virtual void TypeChecking() {}
    virtual void Accept(Visitor *v) {}

    const Token *Tok() const
    {
        return m_tok;
    }
    void SetTok(const Token *tok)
    {
        m_tok = tok;
    }

    // static Expr *MayCast(Expr *expr);
    // static Expr *MayCast(Expr *expr, QualType desType);
    virtual bool IsNullPointerConstant() const
    {
        return false;
    }
    bool IsConstQualified() const
    {
        return m_qual_type.IsConstQualified();
    }
    bool IsChar()
    {
        if (auto type = Type()->ToFunc()) {
            return type->IsChar();
        }
        else if (auto type = Type()->ToArray()) {
            return type->IsChar();
        }
        else if (auto type = Type()->ToArithm()) {
            return type->IsChar();
        }
        return false;
    }
    bool IsInt()
    {
        if (auto type = Type()->ToFunc()) {
            return type->IsInteger();
        }
        else if (auto type = Type()->ToArray()) {
            return type->IsInteger();
        }
        else if (auto type = Type()->ToArithm()) {
            return type->IsInteger();
        }
        return false;
    }
    bool IsVoid()
    {
        if (auto type = Type()->ToFunc()) {
            return type->IsVoid();
        }
        return false;
    }
    bool IsNonVoid()
    {
        return !IsVoid();
    }
    QualType Qual() const
    {
        return m_qual_type;
    }

    // You can construct a expression without specifying a type,
    // then the type should be evaluated in TypeChecking()
    // Expr(const Token *tok, QualType type, long val = 0, bool isconst = false) : m_tok(tok), m_qual_type(type) {}

protected:
    const Token *m_tok;
    QualType m_qual_type;

#ifdef CONST_PROPAGATION
public:
    bool IsConst() const
    {
        return m_isconst;
    }
    void PropagateVal(int val)
    {
        m_val = val;
    }
    Expr(const Token *tok, QualType type, long val = 0, bool isconst = false)
        : m_tok(tok), m_qual_type(type), m_val(val), m_isconst(isconst)
    {
    }
    int m_val;
    bool m_isconst{false};
#endif  // CONST_PROPAGATION
};

/*
 * '+', '-', '*', '/', '<', '>'
 * '=',(复合赋值运算符被拆分为两个运算)
 * '==', '!=', '<=', '>=',
 * '['(下标运算符),
 */
class BinaryOp : public Expr
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;
    friend class LValGenerator;
    friend class Decl;

public:
    static BinaryOp *New(const Token *tok, Expr *lhs, Expr *rhs);
    static BinaryOp *New(const Token *tok, int op, Expr *lhs, Expr *rhs);
    virtual ~BinaryOp() {}
    virtual void Accept(Visitor *v);

    // Member ref operator is a lvalue
    virtual bool IsLVal()
    {
        switch (m_op) {
        case '[':
            return true;
        default:
            return false;
        }
    }
    ArithmType *Convert();

    virtual void TypeChecking() {}

protected:
    // op is TokenType
    BinaryOp(const Token *tok, int op, Expr *lhs, Expr *rhs)
        : Expr(tok, QualType(ArithmType::New(ExprType::T_INT))), m_op(op),
          m_lhs(lhs), m_rhs(rhs)
    {
        #ifdef CONST_PROPAGATION
        // debug("here");
        if (lhs && lhs->IsConst() && rhs && rhs->IsConst()) {
            switch (tok->m_type) {
            case PLUS:
                m_val = lhs->m_val + rhs->m_val;
                m_isconst = true;
                break;
            case MINU:
                m_val = lhs->m_val - rhs->m_val;
                m_isconst = true;
                break;
            case MULT:
                m_val = lhs->m_val * rhs->m_val;
                m_isconst = true;
                break;
            case DIV:
                m_val = lhs->m_val / rhs->m_val;
                m_isconst = true;
                break;
            default:
                debug("cannot combine %d %d due to op: %s", lhs->m_val,
                      rhs->m_val, tokenlist[tok->m_type]);
                m_isconst = false;
                break;
            }
        }
        #endif // CONST_PROPAGATION
    }

    int m_op;  // TokenType
    Expr *m_lhs;
    Expr *m_rhs;
};

class Condition : public BinaryOp
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;
    friend class LValGenerator;
    friend class Decl;

public:
    static Condition *New(Expr *lhs);
    static Condition *New(Expr *lhs, const Token *tok, Expr *rhs);
    virtual ~Condition() {}
    virtual void Accept(Visitor *v);
    virtual bool IsLVal()
    {
        return false;
    }
    virtual void TypeChecking() {}

    bool IsBinary() const
    {
        return !(m_op == NATK || m_rhs == nullptr);
    }

protected:
    Condition(Expr *lhs) : BinaryOp(nullptr, NATK, lhs, nullptr) {}
    Condition(Expr *lhs, const Token *tok, Expr *rhs)
        : BinaryOp(tok, tok->m_type, lhs, rhs)
    {
    }
};

/*
 * Unary Operator:
 * '+'  (PLUS)
 * '-'  (MINUS)
 * CAST // like (int)3
 */
class UnaryOp : public Expr
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;
    friend class LValGenerator;

public:
    static UnaryOp *New(Token *op, Expr *operand, QualType type = nullptr);
    virtual ~UnaryOp() {}
    virtual void Accept(Visitor *v);
    virtual bool IsLVal();
    // ArithmType *Convert();
    void TypeChecking();
    void IncDecOpTypeChecking();
    void AddrOpTypeChecking();
    void DerefOpTypeChecking();
    void UnaryArithmOpTypeChecking();
    void CastOpTypeChecking();

protected:
    UnaryOp(Token *op, Expr *operand, QualType type = nullptr)
        : Expr(op, type), m_op(op->Type()), m_operand(operand)
    {
    #ifdef CONST_PROPAGATION
        if (m_operand && m_operand->IsConst()) {
            switch (m_op)
            {
            case PLUS:
                m_val = m_operand->m_val;
                m_isconst = true;
                break;
            case MINU:
                m_val = -m_operand->m_val;
                m_isconst = true;
                break;
            default:
                break;
            }
        }
    #endif // CONST_PROPAGATION
    }
    TokenType m_op;
    Expr *m_operand;
};

class CallExpr : public Expr
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    using ArgList = std::vector<Expr *>;

public:
    static CallExpr *New(Identifier *designator, const ArgList &args);
    static CallExpr *
    New(Identifier *designator, QualType qt, const ArgList &args);
    ~CallExpr() {}
    virtual void Accept(Visitor *v);

    // A function call is ofcourse not lvalue
    virtual bool IsLVal()
    {
        return false;
    }
    ArgList &Args()
    {
        return m_args;
    }
    Identifier *Designator()
    {
        return m_designator;
    }
    const std::string &Name() const
    {
        return m_tok->m_value;
    }

    virtual void TypeChecking();

protected:
    CallExpr(Identifier *des, QualType qt, const ArgList &args)
        : Expr(nullptr, qt), m_designator(des), m_args(args)
    {
    }

    Identifier *m_designator;
    ArgList m_args;
};

class CharLiteral : public Expr
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static CharLiteral *New(const Token *tok, int tag, unsigned val);
    virtual ~CharLiteral() {}
    virtual void Accept(Visitor *v);
    virtual bool IsLVal()
    {
        return false;
    }
    virtual void TypeChecking() {}

    unsigned Val() const
    {
        return m_cval;
    }
    SourceLocation GetLocation() const
    {
        return m_loc;
    }
    SourceLocation GetBeginLoc() const
    {
        return m_loc;
    }
    SourceLocation GetEndLoc() const
    {
        return m_loc;
    }
    void SetLocation(SourceLocation loc)
    {
        m_loc = loc;
    }

    void SetValue(unsigned val)
    {
        m_cval = val;
    }

    CharLiteral(const Token *tok, QualType type)
        : Expr(tok, type, tok->m_value[0], true), m_loc(tok->m_loc), m_cval(tok->m_value[0])
    {
    }
    CharLiteral(const Token *tok, QualType type, unsigned val)
        : Expr(tok, type, val, true), m_loc(tok->m_loc), m_cval(val)
    {
    }
    CharLiteral(const Token *tok, QualType type, SourceLocation loc)
        : Expr(tok, type, tok->m_value[0], true), m_loc(loc), m_cval(tok->m_value[0])
    {
    }
    // explict CharLiteral constructor
    CharLiteral(unsigned val, QualType type, SourceLocation loc)
        : Expr(nullptr, type, val, true), m_loc(loc), m_cval(val)
    {
    }

private:
    SourceLocation m_loc;
    unsigned m_cval;
};

class StringLiteral : public ASTNode
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static StringLiteral *New(const std::string &val);
    static StringLiteral *New(const Token *tok);
    virtual ~StringLiteral() {}
    virtual void Accept(Visitor *v);
    virtual bool IsLVal()
    {
        return false;
    }
    virtual void TypeChecking() {}

    const std::string &Val() const
    {
        return m_sval;
    }

    const std::string Label() const
    {
        return "strlabel_" + std::to_string(m_id);
    }

    std::string SValRepr() const;

    size_t GenId()
    {
        static size_t id = 0;
        return id++;
    }

protected:
    StringLiteral(const Token *tok) : m_sval(tok->m_value), m_id(GenId()) {}
    StringLiteral(const std::string &val) : m_sval(val), m_id(GenId()) {}
    StringLiteral(const Token *tok, SourceLocation loc)
        : m_loc(loc), m_sval(tok->m_value), m_id(GenId())
    {
    }
    StringLiteral(const std::string &val, SourceLocation loc)
        : m_loc(loc), m_sval(val), m_id(GenId())
    {
    }

    SourceLocation m_loc;
    std::string m_sval;
    size_t m_id;
};

class IntegerLiteral : public Expr
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static IntegerLiteral *New(const Token *tok, int tag, long val);
    static IntegerLiteral *New(const Token *tok, int tag);
    virtual ~IntegerLiteral() {}
    virtual void Accept(Visitor *v);
    virtual bool IsLVal()
    {
        return false;
    }
    virtual void TypeChecking() {}

    long Val() const
    {
        return m_ival;
    }
    void SetVal(long val)
    {
        m_ival = val;
    }

    // std::string SValRepr() const;
    std::string Repr() const
    {
        return std::string(".LC") + std::to_string(m_ival);
    }

protected:
    IntegerLiteral(const Token *tok, QualType type)
        : Expr(tok, type, std::stoi(tok->m_value), true),
          m_ival(std::stoi(tok->m_value))
    {
    }
    IntegerLiteral(const Token *tok, QualType type, long val)
        : Expr(tok, type, val, true), m_ival(val)
    {
    }
    IntegerLiteral(long val, QualType type, SourceLocation loc)
        : Expr(nullptr, type, val, true), m_ival(val)
    {
    }

    SourceLocation m_loc;
    long m_ival;
};

class TempVar : public Expr
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static TempVar *New(QualType type);
    virtual ~TempVar() {}
    virtual void Accept(Visitor *v);
    virtual bool IsLVal()
    {
        return true;
    }
    virtual void TypeChecking() {}

protected:
    TempVar(QualType type) : Expr(nullptr, type), m_tag(GenTag()) {}

private:
    static int GenTag()
    {
        static int tag = 0;
        return ++tag;
    }

    int m_tag;
};

enum Linkage {
    L_NONE,
    L_EXTERNAL,
    L_INTERNAL,
};

class Identifier : public Expr
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;
    friend class LValGenerator;
    using ParamList = std::vector<Identifier *>;

public:
    static Identifier *
    New(const Token *tok, QualType type, Linkage linkage = L_NONE);

    virtual ~Identifier() {}
    virtual void Accept(Visitor *v);
    virtual bool IsLVal()
    {
        return false;
    }

    // An identifier can be:
    //   variable, function, label.
    Identifier *ToTypeName()
    {
        // A typename has no linkage
        // And a function has external or internal linkage
        // if (m_linkage != L_NONE)
        //     return nullptr;
        return this;
    }
    bool IsArray()
    {
        return m_qual_type->ToArray();
    }
    long IntValue()
    {
        return std::stoi(m_tok->m_value);
    }
    char CharValue()
    {
        return m_tok->m_value[0];
    }
    virtual const std::string Name() const
    {
        return m_tok->m_value;
    }
    enum Linkage Linkage() const
    {
        return m_linkage;
    }
    void SetLinkage(enum Linkage linkage)
    {
        m_linkage = linkage;
    }
    void SetDecl(Decl *decl)
    {
        m_decl = decl;
    }

    virtual void TypeChecking() {}
    void SetParamList(ParamList &param_list)
    {
        m_param_list = param_list;
    }
    ParamList m_param_list;
    Decl *m_decl;

protected:
    Identifier(const Token *tok, QualType type, enum Linkage linkage = L_NONE)
        : Expr(tok, type), m_linkage(linkage)
    {
    }
    // test
    Identifier(const std::string &name, QualType qt)
        : Expr(new Token(IDENFR, name), qt)
    {
    }
    // An identifier has property linkage
    enum Linkage m_linkage;
};

/*
 * <Identifier>[<Expr>]
 */
class ArraySubscriptExpr : public Expr
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;
    friend class LValGenerator;

public:
    static ArraySubscriptExpr *
    New(Identifier *lhs, Expr *rhs, QualType type, bool islval);
    static ArraySubscriptExpr *New(Identifier *lhs,
                                   Expr *rhs,
                                   QualType type,
                                   bool islval,
                                   SourceLocation loc);

    virtual ~ArraySubscriptExpr() {}
    virtual void Accept(Visitor *v);
    virtual bool IsLVal()
    {
        return m_islval;
    }

    virtual void TypeChecking() {}
    bool IsChar() const
    {
        return m_lhs->IsChar();
    }

protected:
    ArraySubscriptExpr(Identifier *lhs, Expr *rhs, QualType qt, bool islval)
        : Expr(lhs->Tok(), qt), m_lhs(lhs), m_rhs(rhs), m_islval(islval)
    {
    }
    ArraySubscriptExpr(Identifier *lhs,
                       Expr *rhs,
                       QualType qt,
                       bool islval,
                       SourceLocation loc)
        : Expr(lhs->Tok(), qt), m_loc(loc), m_lhs(lhs), m_rhs(rhs),
          m_islval(islval)
    {
    }
    SourceLocation m_loc;
    Identifier *m_lhs;
    Expr *m_rhs;
    bool m_islval;
};
/*
 * Declaration
 */

using InitList = std::set<Initializer>;

class Decl : public ASTNode
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    // static Decl *New(Object *obj);
    virtual ~Decl() {}
    virtual void Accept(Visitor *v);
    InitList &Inits()
    {
        return m_inits;
    }
    void AddInit(Initializer init);

protected:
    Decl() {}

    InitList m_inits;
    SourceLocation m_loc;
};

class NamedDecl : public Decl
{
public:
    std::string Name() const
    {
        return m_name->Name();
    }

    void SetName(Identifier *name)
    {
        m_name = name;
    }

    enum Linkage Linkage()
    {
        return m_name->Linkage();
    }

    QualType Type() const
    {
        return m_decl_type;
    }

protected:
    NamedDecl() {}
    NamedDecl(Identifier *ident, QualType qt) : m_name(ident), m_decl_type(qt)
    {
    }
    virtual ~NamedDecl() {}

    Identifier *m_name;
    QualType m_decl_type;
};

class FunctionDecl : public NamedDecl
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    using ParamList = std::vector<ParmVarDecl *>;

public:
    static FunctionDecl *New(Identifier *ident,
                             Scope *scope = nullptr,
                             LabelStmt *retLabel = nullptr);
    virtual ~FunctionDecl() {}
    virtual void Accept(Visitor *v);
    ::FuncType *FuncType()
    {
        return m_name->Type()->ToFunc();
    }

    std::string QuadStr()
    {
        return FuncType()->QuadStr(Name());
    }
    CompoundStmt *Body()
    {
        return m_body;
    }
    void SetBody(CompoundStmt *body)
    {
        m_body = body;
    }
    void SetScope(Scope *scope)
    {
        m_scope = scope;
    }

    void AddParam(ParmVarDecl *param)
    {
        m_params.push_back(param);
    }
    size_t ParamNum() const
    {
        return m_params.size();
    }

    Scope *m_scope;
    ParamList m_params;

protected:
    FunctionDecl(Identifier *ident,
                 Scope *scope = nullptr,
                 LabelStmt *retLabel = nullptr)
        : NamedDecl(ident, QualType(ident->Type()->ToFunc())), m_scope(scope),
          m_retlabel(retLabel)
    {
    }

    LabelStmt *m_retlabel;
    CompoundStmt *m_body;
};

/**
 * Variable Declaration (Atomic)
 * @memeber ArithmType(): int, char.
 * int a[3], b, c;
 * const char d = 'e';
 */
class VarDecl : public NamedDecl
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static VarDecl *New(Identifier *ident, QualType qt);
    static VarDecl *
    New(Identifier *ident, QualType qt, Token *uop, CharLiteral *cl);
    static VarDecl *
    New(Identifier *ident, QualType qt, Token *uop, IntegerLiteral *il);
    static VarDecl *New(Identifier *ident, QualType qt, CharLiteral *cl);
    static VarDecl *New(Identifier *ident, QualType qt, IntegerLiteral *il);
    virtual ~VarDecl() {}
    virtual void Accept(Visitor *v);
    ::ArithmType *ArithmType()
    {
        return m_name->Type()->ToArithm();
    }

    std::string QuadStr()
    {
        if (IsConstQualified()) {
            return "const " + m_name->Type()->Str() + " " + Name() + " = " +
                   std::to_string(GetVal());
        }
        else {
            return "var " + m_name->Type()->Str() + " " + Name();
        }
    }
    bool IsConstQualified() const
    {
        return m_name->IsConstQualified();
    }
    long GetVal()
    {
        if (IsConstQualified() && IsChar()) {
            return m_cl->Val();
        }
        else if (IsConstQualified() && IsInt()) {
            return m_il->Val();
        }
        return 0;
    }
    bool IsChar()
    {
        assert(ArithmType());
        return ArithmType()->IsChar();
    }
    bool IsInt()
    {
        assert(ArithmType());
        return ArithmType()->IsInteger();
    }
    size_t Width()
    {
        if (auto arr = m_name->Type()->ToArray()) {
            debug("arr : %d", arr->Width());
            return arr->Width();
        }
        else {
            return 4;
        }
    }
    size_t Num()
    {
        return Width() / 4;
    }

protected:
    // <qt> <ident>
    VarDecl(Identifier *ident, QualType qt) : NamedDecl(ident, qt) {}

    // <qt> <ident> = <uop><literal>
    VarDecl(Identifier *ident, QualType qt, Token *uop, CharLiteral *cl)
        : NamedDecl(ident, qt), m_cl(cl)
    {
        m_uop = UnaryOp::New(uop, cl);
    }
    VarDecl(Identifier *ident, QualType qt, Token *uop, IntegerLiteral *il)
        : NamedDecl(ident, qt), m_il(il)
    {
        m_uop = UnaryOp::New(uop, il);
    }

    // <qt> <ident> = <literal>
    VarDecl(Identifier *ident, QualType qt, CharLiteral *cl)
        : NamedDecl(ident, qt), m_cl(cl)
    {
    }
    VarDecl(Identifier *ident, QualType qt, IntegerLiteral *il)
        : NamedDecl(ident, qt), m_il(il)
    {
    }

    UnaryOp *m_uop;  // + -
    union
    {
        IntegerLiteral *m_il;  // 37
        CharLiteral *m_cl;     // 'c'
    };
};

/*
 * Function paramter declaration
 * (<类型标识符><标识符>)*
 *   int foo(int a, char b)
 * @member m_func: FunctionDecl it belongs to
 */
class ParmVarDecl : public VarDecl
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

public:
    static ParmVarDecl *New(Identifier *ident, FunctionDecl *fd = nullptr);
    static ParmVarDecl *
    New(Identifier *ident, QualType qt, FunctionDecl *fd = nullptr);
    virtual ~ParmVarDecl() {}
    virtual void Accept(Visitor *v);
    ::ArithmType *ArithmType()
    {
        return m_name->Type()->ToArithm();
    }
    bool IsConstQualified() const
    {
        return m_name->IsConstQualified();
    }
    FunctionDecl *GetFunc() const
    {
        return m_func;
    }
    void SetFunc(FunctionDecl *func)
    {
        m_func = func;
    }

protected:
    /// <fd>(<qt> <ident>, ...)
    /// NOTE: initialization not allowed here.
    ///       const parm not allowed here.
    ParmVarDecl(Identifier *ident, FunctionDecl *fd = nullptr)
        : VarDecl(ident, QualType(ident->Type()->ToArithm())), m_func(fd)
    {
    }
    ParmVarDecl(Identifier *ident, QualType qt, FunctionDecl *fd = nullptr)
        : VarDecl(ident, qt), m_func(fd)
    {
    }

    FunctionDecl *m_func;
};

class TranslationUnitDecl : public Decl
{
    template <typename T>
    friend class Evaluator;
    friend class AddrEvaluator;
    friend class CodeGenerator;
    friend class QuadGenerator;

    using ExtDeclList = std::list<ASTNode *>;
    using VarDeclList = std::list<VarDecl *>;

public:
    static TranslationUnitDecl *New()
    {
        return new TranslationUnitDecl();
    }
    virtual ~TranslationUnitDecl() {}
    virtual void Accept(Visitor *v);
    void Add(ASTNode *extDecl)
    {
        m_ext_decls.push_back(extDecl);
    }
    void AddVarDecl(VarDecl *vd)
    {
        m_glb_decls.push_back(vd);
    }
    ExtDeclList &ExtDecls()
    {
        return m_ext_decls;
    }
    const ExtDeclList &ExtDecls() const
    {
        return m_ext_decls;
    }
    VarDeclList &VarDecls()
    {
        return m_glb_decls;
    }
    const VarDeclList &VarDecls() const
    {
        return m_glb_decls;
    }

    void SetScope(Scope *scope)
    {
        m_scope = scope;
    }
    Scope *Scope() const
    {
        return m_scope;
    }

private:
    TranslationUnitDecl() {}

    ExtDeclList m_ext_decls;
    VarDeclList m_glb_decls;
    ::Scope *m_scope;
};

#endif  // !C0C_AST_H
