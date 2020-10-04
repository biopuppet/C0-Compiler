#include "generator.h"
#include "debug.h"
#include "mips_isa.h"
#include "parser.h"
#include "quad_generator.h"
#include "token.h"

#include <cstdarg>
#include <iostream>

void Generator::EmitComment(const char *format, ...)
{
    fprintf(m_outstream, "\n    # ");

    va_list args;
    va_start(args, format);
    vfprintf(m_outstream, format, args);
    va_end(args);

    fputc('\n', m_outstream);
}

void CodeGenerator::EmitDirective(int dir)
{
    EmitBlankLine();
    switch (dir) {
    case D_TEXT:
        Emit(".text", 0);
        break;
    case D_DATA:
        Emit(".data", 0);
        break;
    default:
        break;
    }
}

void CodeGenerator::EmitPush(Gpr reg)
{
    EmitComment("push %s", regs[reg]);
    EmitStore(reg, Gpr::sp);
    Emit("addiu", sp, sp, -4);
}

void CodeGenerator::EmitPop(Gpr reg)
{
    EmitComment("pop %s", regs[reg]);
    Emit("addiu", sp, sp, 4);
    EmitLoad(reg, Gpr::sp);
}

void CodeGenerator::EmitLabel(const std::string &label)
{
    EmitBlankLine();
    Emit(label + ":", 0);
}

// exit(10), ...
void CodeGenerator::EmitSyscall(unsigned service_no)
{
    EmitBlankLine();
    if (service_no == SC_EXIT) {
        EmitComment("Exit(10): terminate execution");
    }
    Emit("li", Gpr::v0, service_no);
    Emit("syscall");
    EmitBlankLine();
}

void CodeGenerator::EmitSyscall(unsigned service_no, const std::string &arg0)
{
    // EmitBlankLine();
    switch (service_no) {
    case SC_PRINT_INT: {
        EmitComment("print integer(1), $a0 = integer to print");
        // move vs. add:
        //   move 2nd operand can only be reg, add 3rd op is extended to reg &
        //   imm
        // if (isdigit(arg0[0]) || arg0[0] == '$')
        Emit("add", regs[Gpr::a0], regs[Gpr::zero], arg0);
        // else if (isalpha(arg0[0]) || arg0[0] == '_')
        //     Emit("lw", regs[Gpr::a0], arg0);
        // Emit("la", Gpr::a0, "__builtin_line_feed__");
        // Emit("li", Gpr::v0, service_no);
        // Emit("syscall");
        Emit("li", regs[Gpr::v0], service_no);
        Emit("syscall");
        // EmitSyscall(4, "__builtin_line_feed__");
        break;
    }
    case SC_PRINT_STR: {
        EmitComment("print string(4), $a0 = string addr");
        Emit("la", regs[Gpr::a0], arg0);
        Emit("li", regs[Gpr::v0], service_no);
        Emit("syscall");
        break;
    }
    case SC_READ_INT: {
        EmitComment("read int(5), $v0 contains integer read");
        Emit("li", Gpr::v0, service_no);
        Emit("syscall");
        // EmitStore(Gpr::v0, arg0);
        break;
    }
    case SC_PRINT_CHAR: {
        EmitComment("print char(11), $a0 = character to print");
        Emit("addu", regs[Gpr::a0], regs[Gpr::zero], arg0);
        Emit("li", Gpr::v0, service_no);
        Emit("syscall");
        break;
    }
    case SC_READ_CHAR: {
        EmitComment("read char(12), $v0 contains char read");
        Emit("li", regs[Gpr::v0], service_no);
        Emit("syscall");
        // EmitStore(Gpr::v0, arg0);
        break;
    }
    default:
        break;
    }

    // if (isalpha(arg0[0]) || arg0[0] == '_') {
    //     EmitStore(Gpr::v0, arg0);
    // }
    // else {
    //     Emit("move", arg0, Gpr::v0);
    // }
    // EmitBlankLine();
}

Gpr CodeGenerator::VisitQuadAddr(QuadAddr *qa)
{
    static int reg = Gpr::t8;

#ifdef GLOBAL_REG_ALLOCATION
    if (m_curfunc->m_isleaf && qa->m_bind) {
        return qa->m_bind;
    }
#endif  // GLOBAL_REG_ALLOCATION
    reg += 1;
    if (reg > Gpr::t9) {
        reg = Gpr::t8;
    }
    switch (qa->m_type) {
    case QuadAddr::AT_CHARL:
    case QuadAddr::AT_INTL:
        Emit("li", (Gpr)reg, qa->m_data);
        break;
    case QuadAddr::AT_IDENT:
        if (qa->m_isglb) {
            // lw $t1, glbvar
            auto glbvar = reinterpret_cast<Identifier *>(qa->m_data);
            EmitLoad((Gpr)reg, glbvar->Name());
        }
        else {
            // lw $t1, off($sp)
            EmitLoad((Gpr)reg, Gpr::sp, qa->m_offset);
        }
        break;
    case QuadAddr::AT_TMPCH:
    case QuadAddr::AT_TMP:
        EmitLoad((Gpr)reg, Gpr::sp, qa->m_offset);
        break;
    case QuadAddr::AT_LABEL:

        break;
    case QuadAddr::AT_ARRAY: {
        auto reg_idx = VisitQuadAddr(qa->m_minion);
        Emit("sll", (Gpr)reg, reg_idx, 2);
        if (qa->m_islval) {
            // maybe changed
            EmitComment(qa->Str().c_str());
            debug(qa->Str().c_str());
            assert(0);
        }
        // sll $t_idx, $t_idx, 2
        // lw $t_dst, array($t_idx) #array + $t_idx * 4
        else if (qa->m_isglb) {
            auto glbarr = reinterpret_cast<Identifier *>(qa->m_data);
            EmitLoad((Gpr)reg, glbarr->Name() + "(" + regs[reg] + ")");
        }
        //  sll $t_idx, $t_idx, 2
        //  addiu $t_dst, $t_idx, offset
        //  addu $t_idx, $t_dst, $sp
        //  lw $t_dst, ($t_idx)    # array + $t_idx * 4
        else {
            Emit("addiu", (Gpr)reg, (Gpr)reg, qa->m_offset);
            Emit("addu", (Gpr)reg, (Gpr)reg, Gpr::sp);
            EmitLoad((Gpr)reg, (Gpr)reg);
        }
        break;
    }

    default:
        debug("QuadAddr Type: %d\n", qa->m_type);
        assert(0);
        break;
    }

    return (Gpr)reg;
}

void CodeGenerator::GenAssign(QuadAddr *dst, QuadAddr *arg1)
{
    // EmitLoad(Gpr::t0, Gpr::sp, quad->m_arg1->m_offset);
    // EmitStore(Gpr::t0, Gpr::sp, quad->m_dst->m_offset);
    auto src = VisitQuadAddr(arg1);
    auto tmp_reg = src == Gpr::t8 ? Gpr::t9 : Gpr::t8;
    switch (dst->m_type) {
    case QuadAddr::AT_IDENT:
        if (dst->m_isglb) {
            auto glbvar = reinterpret_cast<Identifier *>(dst->m_data);
            EmitStore(src, glbvar->Name());
        }
#ifdef GLOBAL_REG_ALLOCATION
        else if (m_curfunc->m_isleaf && dst->m_bind) {
            Emit("move", dst->m_bind, src);
            // Emit("add", dst->m_bind, Gpr::zero, src);
        }
#endif  // GLOBAL_REG_ALLOCATION
        else {
            EmitStore(src, Gpr::sp, dst->m_offset);
        }
        break;
    case QuadAddr::AT_TMPCH:
    case QuadAddr::AT_TMP:
        EmitStore(src, Gpr::sp, dst->m_offset);
        // Emit("move", Gpr::t8, src);
        break;

    case QuadAddr::AT_ARRAY: {
        // assert(dst->m_islval);
        auto reg_idx = VisitQuadAddr(dst->m_minion);
        Emit("sll", tmp_reg, reg_idx, 2);
        // sll $ti, $ti, 2
        // sw $tx, glist($ti)
        if (dst->m_isglb) {
            auto glbvar = reinterpret_cast<Identifier *>(dst->m_data);
            EmitStore(src, tmp_reg, glbvar->Name());
        }
        // sll $ti, $ti, 2
        // addiu $tx, $ti, offset
        // addu $ti, $tx, $sp
        else {
            Emit("addiu", tmp_reg, tmp_reg, dst->m_offset);
            Emit("addu", tmp_reg, tmp_reg, Gpr::sp);
            EmitStore(src, tmp_reg);
        }
        break;
    }
    case QuadAddr::AT_INTL:
    case QuadAddr::AT_CHARL:
        assert(0);
        break;
    default:
        break;
    }
}

void CodeGenerator::GenAdd(Quadruple *quad)
{
    auto reg1 = VisitQuadAddr(quad->m_arg1);
    auto reg2 = VisitQuadAddr(quad->m_arg2);
#ifdef GLOBAL_REG_ALLOCATION
    if (m_curfunc->m_isleaf) {
        if (quad->m_arg1->m_bind) {
            reg1 = quad->m_arg1->m_bind;
        }
        if (quad->m_arg2->m_bind) {
            reg2 = quad->m_arg2->m_bind;
        }
        if (quad->m_dst->m_bind) {
            Gpr dst = quad->m_dst->m_bind;
            Emit("addu", dst, reg1, reg2);
        }
        else {
            Emit("addu", Gpr::t8, reg1, reg2);
            EmitStore(Gpr::t8, Gpr::sp, quad->m_dst->m_offset);  // opt
        }
        return;
    }
#endif  // GLOBAL_REG_ALLOCATION
        // EmitComment("%s", regs[dst]);
    Emit("addu", Gpr::t8, reg1, reg2);
    EmitStore(Gpr::t8, Gpr::sp, quad->m_dst->m_offset);  // opt
}

void CodeGenerator::GenSub(Quadruple *quad)
{
    auto reg1 = VisitQuadAddr(quad->m_arg1);
    auto reg2 = VisitQuadAddr(quad->m_arg2);
// auto dst = VisitQuadAddr(quad->m_dst);
#ifdef GLOBAL_REG_ALLOCATION
    if (m_curfunc->m_isleaf) {
        if (quad->m_arg1->m_bind) {
            reg1 = quad->m_arg1->m_bind;
        }
        if (quad->m_arg2->m_bind) {
            reg2 = quad->m_arg2->m_bind;
        }
        if (quad->m_dst->m_bind) {
            Gpr dst = quad->m_dst->m_bind;
            Emit("subu", dst, reg1, reg2);
        }
        else {
            Emit("subu", Gpr::t8, reg1, reg2);
            EmitStore(Gpr::t8, Gpr::sp, quad->m_dst->m_offset);  // opt
        }
        return;
    }
#endif  // GLOBAL_REG_ALLOCATION
    Emit("subu", Gpr::t8, reg1, reg2);
    EmitStore(Gpr::t8, Gpr::sp, quad->m_dst->m_offset);  // opt
}

void CodeGenerator::GenMult(Quadruple *quad)
{
    auto reg1 = VisitQuadAddr(quad->m_arg1);
    auto reg2 = VisitQuadAddr(quad->m_arg2);
// auto dst = VisitQuadAddr(quad->m_dst);
#ifdef GLOBAL_REG_ALLOCATION
    if (m_curfunc->m_isleaf) {
        if (quad->m_arg1->m_bind) {
            reg1 = quad->m_arg1->m_bind;
        }
        if (quad->m_arg2->m_bind) {
            reg2 = quad->m_arg2->m_bind;
        }
        if (quad->m_dst->m_bind) {
            Gpr dst = quad->m_dst->m_bind;
            Emit("mul", dst, reg1, reg2);
        }
        else {
            Emit("mul", Gpr::t8, reg1, reg2);
            EmitStore(Gpr::t8, Gpr::sp, quad->m_dst->m_offset);  // opt
        }
        return;
    }
#endif  // GLOBAL_REG_ALLOCATION
    Emit("mul", Gpr::t8, reg1, reg2);
    EmitStore(Gpr::t8, Gpr::sp, quad->m_dst->m_offset);  // opt
}

void CodeGenerator::GenDiv(Quadruple *quad)
{
    auto reg1 = VisitQuadAddr(quad->m_arg1);
    auto reg2 = VisitQuadAddr(quad->m_arg2);
#ifdef GLOBAL_REG_ALLOCATION
    if (m_curfunc->m_isleaf) {
        if (quad->m_arg1->m_bind) {
            reg1 = quad->m_arg1->m_bind;
        }
        if (quad->m_arg2->m_bind) {
            reg2 = quad->m_arg2->m_bind;
        }
        Emit("div", reg1, reg2);
        if (quad->m_dst->m_bind) {
            Emit("mflo", quad->m_dst->m_bind);
        }
        else {
            Emit("mflo", Gpr::t8);
            EmitStore(Gpr::t8, Gpr::sp, quad->m_dst->m_offset);  // opt
        }
        return;
    }
#endif  // GLOBAL_REG_ALLOCATION
    Emit("div", reg1, reg2);
    Emit("mflo", Gpr::t8);
    EmitStore(Gpr::t8, Gpr::sp, quad->m_dst->m_offset);  // opt
}

void CodeGenerator::GenPush(Quadruple *quad)
{
    auto src = VisitQuadAddr(quad->m_dst);  // TODO switch
#ifdef REG_ARGS
    if (quad->m_arg1->m_data <= 12) {
        auto reg = Gpr::a0 + (quad->m_arg1->m_data >> 2);
        Emit("move", (Gpr)(reg), src);
    }
    else {
        EmitStore(src, Gpr::sp, quad->m_arg1->m_data);
    }
#else
    EmitStore(src, Gpr::sp, quad->m_arg1->m_data);
#endif  //
}

void CodeGenerator::GenCall(Quadruple *quad)
{
    // assume args have been pushed
    debug("Gen Func Call...");
    auto ls = reinterpret_cast<Identifier *>(quad->m_arg1->m_data);
    Emit("jal", "$func_" + ls->Name() + "_entry");  // @refactor
    // auto ret = VisitQuadAddr(quad->m_dst);  // temp
    if (ls->IsNonVoid()) {
        EmitStore(Gpr::v0, Gpr::sp, quad->m_dst->m_offset);
    }
}

void CodeGenerator::GenReturn(Quadruple *quad)
{
    debug("Gen Func return...");
    if (quad->m_dst) {
        auto reg1 = VisitQuadAddr(quad->m_dst);
        Emit("move", Gpr::v0, reg1);
    }
#ifdef INLINE_EPILOG
    GenEpilogue(m_curfunc->m_frame);
#else
    Emit("j", m_curfunc->m_exit_label);
#endif
}

/*
 *  .data
 *      mystr_0: .asciiz "atta boy!\n"
 *  .text
 *      la      $a0, mystr_0
 *      li      $v0, 4
 *      syscall
 */
void CodeGenerator::EmitPrint(StringLiteral *sl)
{
    EmitDirective(D_DATA);
    Emit(sl->Label() + ": .asciiz \"" + sl->Val() + "\"");
    EmitDirective(D_TEXT);
    EmitSyscall(SC_PRINT_STR, sl->Label());
    EmitBlankLine();
}

void CodeGenerator::EmitPrint(IntegerLiteral *il)
{
    EmitPrint(std::to_string(il->Val()), 1);
}

void CodeGenerator::EmitPrint(Identifier *ident)
{
    auto name = ident->Name();
    auto sysn = ident->IsChar() ? SC_PRINT_CHAR : SC_PRINT_INT;
    EmitSyscall(sysn, name);
}

void CodeGenerator::EmitPrint(Gpr reg, int sysn)
{
    EmitSyscall(sysn, regs[reg]);
}

void CodeGenerator::EmitPrint(const std::string &arg, int sysn)
{
    EmitSyscall(sysn, arg);
}

void CodeGenerator::EmitPrint(QuadAddr *qa)
{
    switch (qa->m_type) {
    case QuadAddr::AT_STR: {
        auto cast = reinterpret_cast<StringLiteral *>(qa->m_data);
        EmitPrint(cast);
        break;
    }
    case QuadAddr::AT_IDENT: {
        auto cast = reinterpret_cast<Identifier *>(qa->m_data);
        auto sysn = cast->IsChar() ? SC_PRINT_CHAR : SC_PRINT_INT;
        if (qa->m_isglb) {
            EmitLoad(Gpr::t8, cast->Name());
        }
        else {
#ifdef GLOBAL_REG_ALLOCATION
            if (m_curfunc->m_isleaf && qa->m_bind) {
                EmitPrint(qa->m_bind, sysn);
                break;
            }
#endif  // GLOBAL_REG_ALLOCATION
            EmitLoad(Gpr::t8, Gpr::sp, qa->m_offset);
        }
        EmitPrint(Gpr::t8, sysn);
        break;
    }
    case QuadAddr::AT_TMP: {
        EmitLoad(Gpr::t8, Gpr::sp, qa->m_offset);
        EmitPrint(Gpr::t8, SC_PRINT_INT);
        break;
    }
    case QuadAddr::AT_TMPCH: {
        EmitLoad(Gpr::t8, Gpr::sp, qa->m_offset);
        EmitPrint(Gpr::t8, SC_PRINT_CHAR);
        break;
    }
    case QuadAddr::AT_INTL: {
        Emit("li", Gpr::t8, qa->m_data);
        EmitPrint(Gpr::t8, SC_PRINT_INT);
        break;
    }
    case QuadAddr::AT_CHARL: {
        Emit("li", Gpr::t8, qa->m_data);
        EmitPrint(Gpr::t8, SC_PRINT_CHAR);
        break;
    }
    case QuadAddr::AT_ARRAY: {
        EmitComment("wo shi ni die");
        debug("AT_ARRAY!!!");
        assert(0);
    }
    default:
        assert(0);
        break;
    }
}

void CodeGenerator::EmitScanf(Identifier *ident)
{
    auto sysn = ident->IsInt() ? SC_READ_INT :
                                 ident->IsChar() ? SC_READ_CHAR : SC_READ_INT;
    EmitSyscall(sysn, ident->Name());
    EmitStore(Gpr::v0, ident->Name());
    assert(0);
    // if (m_curfunc->m_isleaf)
    // Emit("move", )
}

void CodeGenerator::EmitScanf(QuadAddr *qa)
{
    assert(qa->m_type == QuadAddr::AT_IDENT);
    auto ident = reinterpret_cast<Identifier *>(qa->m_data);
    auto sysn = ident->IsChar() ? SC_READ_CHAR : SC_READ_INT;
    if (qa->m_isglb) {
        EmitSyscall(sysn, ident->Name());
        EmitStore(Gpr::v0, ident->Name());
    }
    else {
        EmitSyscall(sysn, std::to_string(qa->m_offset) + "($sp)");
        EmitStore(Gpr::v0, std::to_string(qa->m_offset) + "($sp)");
#ifdef GLOBAL_REG_ALLOCATION
        if (m_curfunc->m_isleaf && qa->m_bind) {
            Emit("move", qa->m_bind, Gpr::v0);
        }
#endif
    }
}

void CodeGenerator::EmitQuad(Quadruple *quad)
{
    switch (quad->m_op) {
    case QuadOp::QO_LABEL: {
        auto cast = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        EmitLabel(cast->Repr());
        break;
    }
    case QuadOp::QO_PRINT:
        EmitPrint(quad->m_dst);
        break;
    case QuadOp::QO_SCAN:
        EmitScanf(quad->m_dst);
        break;
    case QuadOp::QO_ASSIGN: {
        GenAssign(quad->m_dst, quad->m_arg1);
        break;
    }
    case QuadOp::QO_INDEX_ARG: {
        EmitComment("index arg\n");
        assert(0);
        break;
    }
    case QuadOp::QO_PLUS:
        GenAdd(quad);
        break;
    case QuadOp::QO_MINU:
        GenSub(quad);
        break;
    case QuadOp::QO_MULT:
        GenMult(quad);
        break;
    case QuadOp::QO_DIV:
        GenDiv(quad);
        break;
    case QuadOp::QO_PUSH:
        GenPush(quad);
        break;
    case QuadOp::QO_CALL:
        GenCall(quad);
        break;
    case QuadOp::QO_RETURN: {
        GenReturn(quad);
        break;
    }
    case QuadOp::QO_GOTO: {
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("j", ls->Repr());
        break;
    }
    case QuadOp::QO_BZ: {
        auto reg1 = VisitQuadAddr(quad->m_arg1);
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("beqz", reg1, ls);
        break;
    }
    case QuadOp::QO_BNZ: {
        auto reg1 = VisitQuadAddr(quad->m_arg1);
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("bnez", reg1, ls);
        break;
    }
    case QuadOp::QO_BLT: {
        auto reg1 = VisitQuadAddr(quad->m_arg1);
        auto reg2 = VisitQuadAddr(quad->m_arg2);
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("blt", reg1, reg2, ls);
        break;
    }
    case QuadOp::QO_BLE: {
        auto reg1 = VisitQuadAddr(quad->m_arg1);
        auto reg2 = VisitQuadAddr(quad->m_arg2);
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("ble", reg1, reg2, ls);
        break;
    }
    case QuadOp::QO_BGT: {
        auto reg1 = VisitQuadAddr(quad->m_arg1);
        auto reg2 = VisitQuadAddr(quad->m_arg2);
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("bgt", reg1, reg2, ls);
        break;
    }
    case QuadOp::QO_BGE: {
        auto reg1 = VisitQuadAddr(quad->m_arg1);
        auto reg2 = VisitQuadAddr(quad->m_arg2);
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("bge", reg1, reg2, ls);
        break;
    }
    case QuadOp::QO_BEQ: {
        auto reg1 = VisitQuadAddr(quad->m_arg1);
        auto reg2 = VisitQuadAddr(quad->m_arg2);
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("beq", reg1, reg2, ls);
        break;
    }
    case QuadOp::QO_BNE: {
        auto reg1 = VisitQuadAddr(quad->m_arg1);
        auto reg2 = VisitQuadAddr(quad->m_arg2);
        auto ls = reinterpret_cast<LabelStmt *>(quad->m_dst->m_data);
        Emit("bne", reg1, reg2, ls);
        break;
    }
    case QuadOp::QO_PARAM: {
#ifdef REG_ARGS
        auto reg = (Gpr)(m_curfunc->m_parmnum + Gpr::a0);
        ++m_curfunc->m_parmnum;
        Emit("move", quad->m_dst->m_bind, reg);
        if (!m_curfunc->m_isleaf) {
            EmitStore(reg, Gpr::sp, quad->m_dst->m_offset);
        }
        break;
#else
        break;
#endif  // REG_ARGS
    }
    default:
        debug("Not support QuadOp::%d yet!\n", quad->m_op);
        assert(0);
        break;
    }
}

static int get_ones(uint32_t n)
{
    int cnt = 0;
    do {
        if (n & 1)
            ++cnt;
    } while (n >>= 1);
    return cnt;
}

void CodeGenerator::GenPrologue(Frame &frame)
{
    auto frame_size = frame.size;
    auto frame_mask = frame.mask;
    if (!frame_size) {
        EmitComment("Empty frame, no prologue!");
        return;
    }
    EmitComment(".prologue");

    auto save_size = get_ones(frame_mask) * 4;
    assert(!(frame_size & 3));
    debug("frame size: %d, save size: %d, mask: %#X\n", frame_size, save_size,
          frame_mask);
    assert(frame_size >= save_size);

    Emit("addiu", Gpr::sp, Gpr::sp, -frame_size);
    long size = frame_size - 4;
    for (auto i = 31; i >= 0 && size >= frame_size - save_size; --i) {
        auto bit = (frame_mask >> i) & 1;
        if (bit) {
            EmitStore((Gpr)i, Gpr::sp, size);
            size -= 4;
        }
    }
    // Emit("move", Gpr::fp, Gpr::sp);
}

void CodeGenerator::GenEpilogue(Frame &frame)
{
    auto frame_size = frame.size;
    auto frame_mask = frame.mask;
    if (!frame_size) {
        EmitComment("Empty frame, no Epilogue!");
        Emit("jr", Gpr::ra);
        return;
    }
    EmitComment(".epilogue");
    // Emit("move", Gpr::sp, Gpr::fp);

    auto save_size = get_ones(frame_mask) * 4;
    assert(!(frame_size & 3));
    assert(frame_size >= save_size);

    long size = frame_size - save_size;
    for (auto i = 0; i < 32 && size < frame_size; ++i) {
        auto bit = (frame_mask >> i) & 1;
        if (bit) {
            EmitLoad((Gpr)i, Gpr::sp, size);
            size += 4;
        }
    }
    Emit("addiu", Gpr::sp, Gpr::sp, frame_size);
    Emit("jr", Gpr::ra);
}

void CodeGenerator::GenFunc(FuncInfo *func_info)
{
    m_curfunc = func_info;
    auto name = m_curfunc->Name();
    Emit("\n################### " + name + " ###################", 0);
    EmitLabel(m_curfunc->m_entry_label);
    GenPrologue(m_curfunc->m_frame);
    // GenCopyParams();

    for (auto quad : m_curfunc->m_quads) {
        EmitComment(quad->Str().c_str());
        debug(quad->Str().c_str());
        EmitQuad(quad);
    }

    EmitLabel(m_curfunc->m_exit_label);
    GenEpilogue(m_curfunc->m_frame);
    Emit("\n# ^^^^^^^^^^^^^^^^^^ " + name + " ^^^^^^^^^^^^^^^^^^", 0);
}

void CodeGenerator::Gen()
{
    // TestGen();
    Emit("########## MIPS Assembly Code generated by c0c ##########", 0);
    // EmitDirective(D_TEXT);
    // Emit("j", "$func_main_entry");

    EmitDirective(D_DATA);
    // Emit("__builtin_line_feed__: .asciiz \"\\n\"");  // @refactor
    for (auto entry : m_qg->m_data_entries) {
        Emit(entry->Repr());
    }

    EmitDirective(D_TEXT);
    // for (auto func : m_qg->m_funcs) {
    //     GenFunc(func);
    // }
    m_curfunc = *(m_qg->m_funcs.end() - 1);  // main
    for (auto quad : m_curfunc->m_quads) {
        if (quad->m_op == QuadOp::QO_RETURN) {
            EmitSyscall(10);
        }
        else {
            EmitComment(quad->Str().c_str());
            debug(quad->Str().c_str());
            EmitQuad(quad);
        }
    }
    EmitSyscall(10);

    for (auto it = m_qg->m_funcs.end() - 2; it >= m_qg->m_funcs.begin(); --it) {
        GenFunc(*it);
    }
}

void CodeGenerator::TestGen()
{
    // StringLiteral teststr("wodiaonimada");
    // IntegerLiteral testint(157, QualType(nullptr), SourceLocation());
    // Identifier testident("glb_const_int_1", ArithmType::New(T_INT));
    Frame frame = {8, 0x40000000, 0};
    GenPrologue(frame);
    GenEpilogue(frame);
}
