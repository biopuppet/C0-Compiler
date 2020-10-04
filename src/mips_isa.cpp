#include "mips_isa.h"

RegNameList regs{"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
                 "$t0",   "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
                 "$s0",   "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                 "$t8",   "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
// struct Reg RegPool[8] = {
//     {Gpr::t0, 0, nullptr}, {Gpr::t1, 0, nullptr}, {Gpr::t2, 0, nullptr},
//     {Gpr::t3, 0, nullptr}, {Gpr::t4, 0, nullptr}, {Gpr::t5, 0, nullptr},
//     {Gpr::t6, 0, nullptr}, {Gpr::t7, 0, nullptr}};

// DataSegEntry *DataSegEntry::New(QuadAddr *qa)
// {
//     switch (qa->m_type) {
//     case QuadAddr::AT_IDENT:
//         auto ident = reinterpret_cast<Identifier *>(qa->m_data);
//         auto decl = ident->m_decl;
//         auto ec = EntryClass::EC_WORD;
//         auto val =
//             ident->IsConstQualified() ?
//                 ident->IsChar() ? ident->CharValue() : ident->IntValue() :
//                 0;
//         auto num = decl-> ?
//         break;

//     default:
//         break;
//     }
//     auto ident = qa->m_data;
//     auto ret = new (dataSegEntryPool.Alloc()) DataSegEntry(ident, val);
//     ret->m_pool = &dataSegEntryPool;
//     return ret;
// }
