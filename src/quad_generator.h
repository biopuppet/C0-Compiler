#ifndef C0C_QUAD_GENERATOR_H
#define C0C_QUAD_GENERATOR_H

#include "generator.h"

#include <cassert>
#include <map>
#include <string>
#include <vector>

using IdentTab = std::map<Identifier *, QuadAddr *>;

enum QuadOp {
    QO_LABEL = 0,  // just label (must be global)
    QO_BEQ = 1,    // beq %1, %2, label
    QO_BNE = -1,   // bne %1, %2, label
    QO_BGE = 2,    // beq %1, %2, label
    QO_BLT = -2,   // beq %1, %2, label
    QO_BGT = 3,    // beq %1, %2, label
    QO_BLE = -3,   // beq %1, %2, label
    QO_BZ = 4,     // bz %1, label
    QO_BNZ = -4,   // bnz %1, label
    QO_GOTO = 5,   // goto label
    QO_PARAM,      // ()
    QO_PRINT,      // printf *
    QO_SCAN,       // scanf *
    // QO_STORE,      // dst <=
    QO_ASSIGN,     // dst = arg1 (could be array)
    QO_PLUS,       // dst = arg1 + arg2
    QO_MINU,       // dst = arg1 - arg2
    QO_MULT,       // dst = arg1 * arg2
    QO_DIV,        // dst = arg1 / arg2
    QO_INDEX_ARG,  // dst = arg1[arg2]
    // QO_INDEX_DST,  // dst[arg1] = arg2
    QO_RETURN,  // ret [i32/c8]
    QO_PUSH,    // push param
    QO_CALL,    // ret(dst) = call func(arg1)

};

class QuadAddr
{
public:
    enum AddrType {
        AT_IDENT,   // data points to Identifier Table entry
        AT_STR,     // data points to string literal
        AT_OFFSET,  // var in [$fp + m_offset] ?
        AT_INTL,    // integer/char literals like 187, 'c', etc.
        AT_CHARL,   // char literals like 'c', etc.
        AT_TMP,     // compiler generated id
        AT_TMPCH,   // compiler generated id
        AT_LABEL,   // regular label
        AT_ARRAY,   // array[x]
    };

    std::string Str();

    static QuadAddr *New(AddrType type, long data, unsigned off = 0);
    static QuadAddr *
    New(AddrType type, Identifier *ident, bool isglb = false, unsigned off = 0);
    static QuadAddr *
    New(AddrType type, QuadAddr *master, QuadAddr *minion, bool islval);
    static QuadAddr *New(AddrType type, StringLiteral *str, unsigned off = 0);
    static QuadAddr *New(AddrType type, LabelStmt *data);
    static QuadAddr *New(AddrType type, FunctionDecl *data);

    QuadAddr() {}
    QuadAddr(AddrType type, long data, unsigned off = 0)
        : m_data(data), m_type(type), m_offset(off), m_resolved(false)
    {
    }
    QuadAddr(AddrType type, Identifier *data, bool isglb, unsigned off = 0)
        : m_data(reinterpret_cast<intptr_t>(data)), m_type(type), m_offset(off),
          m_isglb(isglb), m_resolved(false)
    {
    }
    QuadAddr(AddrType type, QuadAddr *master, QuadAddr *minion, bool islval)
        : m_data(master->m_data), m_minion(minion), m_type(type),
          m_offset(master->m_offset), m_isglb(master->m_isglb),
          m_islval(islval), m_resolved(false)
    {
    }
    QuadAddr(AddrType type, StringLiteral *str, unsigned off = 0)
        : m_data(reinterpret_cast<intptr_t>(str)), m_type(type), m_offset(off),
          m_isglb(false), m_resolved(true)
    {
    }
    QuadAddr(AddrType type, LabelStmt *data)
        : m_data(reinterpret_cast<intptr_t>(data)), m_type(type), m_offset(0),
          m_isglb(false), m_resolved(true)
    {
    }
    // ret label
    QuadAddr(AddrType type, FunctionDecl *data)
        : m_data(reinterpret_cast<intptr_t>(data)), m_type(type), m_offset(0),
          m_isglb(false), m_resolved(true)
    {
    }

    void SetOffset(unsigned off)
    {
        m_offset = off;
    }
    void SetMinion(QuadAddr *minion)
    {
        m_minion = new QuadAddr(*minion);
    }
    bool IsChar() const
    {
        return m_type == AT_TMPCH || m_type == AT_CHARL;
    }

    intptr_t m_data;
    QuadAddr *m_minion{nullptr};
    AddrType m_type;
    unsigned m_offset;  // can't be static, That'd be unsafe!
    Gpr m_bind;
    MemoryPool *m_pool{nullptr};
    bool m_isglb;
    bool m_islval{false};
    bool m_resolved;
};

class Quadruple
{
public:
    static Quadruple *New(QuadOp op,
                          QuadAddr *dst = nullptr,
                          QuadAddr *arg1 = nullptr,
                          QuadAddr *arg2 = nullptr);
    std::string Str();

    Quadruple() {}
    Quadruple(QuadOp bop,
              QuadAddr *dst = nullptr,
              QuadAddr *arg1 = nullptr,
              QuadAddr *arg2 = nullptr)
        : m_dst(dst), m_arg1(arg1), m_arg2(arg2), m_op(bop)
    {
        // assert(m_dst->m_type != QuadAddr::AT_INTL);
    }

    QuadAddr *m_dst;
    QuadAddr *m_arg1;
    QuadAddr *m_arg2;
    QuadOp m_op;
    MemoryPool *m_pool{nullptr};
};

/* Stack Frame Layout
 * ------- top -------
 *   saved registers
 * -------------------
 *   local variables
 *   & temporaries
 * -------------------
 *       ......
 * -------------------
 *    args building
 * ------- end ------- <^- caller's frame
 *   saved registers
 * -------------------
 *       ......
 * ------------------- <^- callee's frame
 */
typedef struct Frame
{
    int size;
    uint32_t mask;
    int placeholder;
} Frame;

// enum FrameMask {
//     FM_RA = 0x80000000,
//     FM_FP = 0x40000000,

//     FM_S7 = 0x00800000,
//     FM_S6 = 0x00400000,
//     FM_S5 = 0x00200000,
//     FM_S4 = 0x00100000,
//     FM_S3 = 0x00080000,
//     FM_S2 = 0x00040000,
//     FM_S1 = 0x00020000,
//     FM_S0 = 0x00010000,
// };
class FuncInfo
{
public:
    using QuadList = std::vector<Quadruple *>;

    FuncInfo *New()
    {
        return nullptr;  // TODO
    }
    FuncInfo(FunctionDecl *func);
    FuncInfo(FunctionDecl *func, Frame &frame);
    ~FuncInfo() {}

    std::string Name();
    void Add(Quadruple *quad);
    QuadAddr *Find(Identifier *ident);

    Gpr AllocReg(QuadAddr *qa);

    void FreeReg(Gpr name);

public:
    struct Reg m_regpool[8] = {{Gpr::s0, 0, nullptr}, {Gpr::s1, 0, nullptr},
                               {Gpr::s2, 0, nullptr}, {Gpr::s3, 0, nullptr},
                               {Gpr::s4, 0, nullptr}, {Gpr::s5, 0, nullptr},
                               {Gpr::s6, 0, nullptr}, {Gpr::s7, 0, nullptr}};
    // mask + a0~a3 + outgoing args + local var
    Frame m_frame;
    QuadList m_quads;
    IdentTab m_qamap;
    FunctionDecl *m_func;
    int m_parmnum{0};
    std::string m_entry_label;  // not neccessary
    std::string m_exit_label;   // not neccessary

    unsigned m_argbuildsz = 0;
    bool m_isleaf{true};
};

class QuadGenerator : public Generator, public Visitor
{
public:
    using FuncInfoList = std::vector<FuncInfo *>;

    QuadGenerator(Parser *parser = nullptr, FILE *out = nullptr);
    // explicit QuadGenerator(Quadruple quad) : m_quad(quad) {}
    ~QuadGenerator() {}

    void SetQuad(Quadruple quad)
    {
        m_quad = quad;
    }

    virtual void Visit(ASTNode *node)
    {
        node->Accept(this);
    }
    void VisitExpr(Expr *expr)
    {
        expr->Accept(this);
    }
    void VisitStmt(Stmt *stmt)
    {
        stmt->Accept(this);
    }
    virtual void VisitBinaryOp(BinaryOp *binary);
    virtual void VisitCondition(Condition *cond);
    virtual void VisitUnaryOp(UnaryOp *unary);
    virtual void VisitCallExpr(CallExpr *funcCall);
    virtual void VisitArraySubscriptExpr(ArraySubscriptExpr *ase);
    virtual void VisitIdentifier(Identifier *ident);
    virtual void VisitIntegerLiteral(IntegerLiteral *cons);
    virtual void VisitCharLiteral(CharLiteral *cons);
    virtual void VisitStringLiteral(StringLiteral *cons);
    virtual void VisitTempVar(TempVar *tempVar);

    virtual void VisitDecl(Decl *init);
    virtual void VisitDeclStmt(DeclStmt *declStmt);
    virtual void VisitScanfStmt(ScanfStmt *scanfStmt);
    virtual void VisitPrintStmt(PrintStmt *printStmt);
    virtual void VisitIfStmt(IfStmt *ifStmt);
    virtual void VisitForStmt(ForStmt *forStmt);
    virtual void VisitWhileStmt(WhileStmt *whileStmt);
    virtual void VisitDoStmt(DoStmt *doStmt);
    virtual void VisitJumpStmt(JumpStmt *jumpStmt);
    virtual void VisitReturnStmt(ReturnStmt *returnStmt);
    virtual void VisitLabelStmt(LabelStmt *labelStmt);
    virtual void VisitNullStmt(NullStmt *emptyStmt);
    virtual void VisitCompoundStmt(CompoundStmt *compStmt);
    virtual void VisitFunctionDecl(FunctionDecl *funcDecl);
    virtual void VisitVarDecl(VarDecl *varDecl);
    virtual void VisitParmVarDecl(ParmVarDecl *parmVarDecl);
    virtual void VisitTranslationUnitDecl(TranslationUnitDecl *unit);

    Quadruple *NewQuad(QuadOp op,
                       QuadAddr *dst = nullptr,
                       QuadAddr *arg1 = nullptr,
                       QuadAddr *arg2 = nullptr);
    QuadAddr *NewTemp();
    QuadAddr *NewTempChar();
    QuadAddr *NewLabel();

    void Gen();

    void EmitQuad(Quadruple *quad)
    {
        Emit(quad->Str(), quad->m_op == QuadOp::QO_LABEL ? 0 : 4);
    }

    void EmitLabel(const std::string &label)
    {
        Emit("\n" + label + ":", 0);
    }
    void EmitScanf(Identifier *ident)
    {
        Emit("scanf " + ident->Name());
        EmitBlankLine();
    }

    size_t NextTemp()
    {
        return ++m_tempid;
    }
    size_t CurTemp()
    {
        return m_tempid;
    }
    QuadAddr *Find(Identifier *ident);

    Quadruple m_quad;
    FuncInfoList m_funcs;
    IdentTab m_glb_identtab;
    DataSegTab m_data_entries;

protected:
    size_t m_tempid = 0;
    FuncInfo *m_curfunc;
    unsigned m_curoffset = 0;
    QuadAddr *m_curqa;
    // QuadAddr *m_curqa2;
    LabelStmt *m_curdst = nullptr;
    bool m_curlval = false;

    Scope *m_curscope;
    Scope *m_glbscope;
};

struct DataSegEntry
{
    enum EntryClass {
        EC_WORD,
        EC_SPACE,
        EC_ASCIIZ,
    };

    static DataSegEntry *New(Identifier *ident,
                             enum EntryClass ec,
                             long val = 0,
                             unsigned num = 1,
                             int align = 4);
    static DataSegEntry *New(Identifier *ident, StringLiteral *val);
    static DataSegEntry *New(QuadAddr *qa);

    // .data <label>: .<class> <value> : <num>
    DataSegEntry(Identifier *ident,
                 enum EntryClass ec,
                 long val = 0,
                 unsigned num = 1,
                 int align = 4)
        : m_val(val), m_ident(ident), m_class(ec), m_num(num > 0 ? num : 1),
          m_align(align)
    {
    }
    DataSegEntry(Identifier *ident, StringLiteral *val)
        : m_ident(ident), m_class(EC_ASCIIZ), m_num(0), m_align(0)
    {
        m_val = reinterpret_cast<intptr_t>(val);
    }
    ~DataSegEntry() {}

    std::string Repr();

    intptr_t m_val;  // StringLiteral *, long, char
    Identifier *m_ident;
    int m_class;
    int m_num;
    int m_align;
    QuadAddr *m_qa;
    MemoryPool *m_pool{nullptr};
};

#endif  // !C0C_QUAD_GENERATOR_H
