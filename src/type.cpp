#include "type.h"

#include "ast.h"
#include "scope.h"
#include "token.h"

#include <algorithm>
#include <cassert>
#include <iostream>

static MemoryPoolImp<VoidType> voidTypePool;
static MemoryPoolImp<ArrayType> arrayTypePool;
static MemoryPoolImp<FuncType> funcTypePool;
static MemoryPoolImp<PointerType> pointerTypePool;
static MemoryPoolImp<ArithmType> arithmTypePool;

VoidType *VoidType::New()
{
  static auto ret = new (voidTypePool.Alloc()) VoidType(&voidTypePool);
  return ret;
}

ArithmType *ArithmType::New(int typeSpec)
{
#define NEW_TYPE(tag)                                                          \
  new (arithmTypePool.Alloc()) ArithmType(&arithmTypePool, tag);

  static auto boolType = NEW_TYPE(T_BOOL);
  static auto charType = NEW_TYPE(T_CHAR);
  static auto intType = NEW_TYPE(T_INT);
  static auto voidType = NEW_TYPE(T_VOID);

  auto tag = ArithmType::Spec2Tag(typeSpec);
  switch (tag) {
  case T_BOOL:
    return boolType;
  case T_CHAR:
    return charType;
  case T_INT:
    return intType;
  case T_VOID:
    return voidType;
  default:
    Error("not supported yet");
  }
  return nullptr;  // Make compiler happy

#undef NEW_TYPE
}

ArrayType *ArrayType::New(int len, QualType eleType)
{
  return new (arrayTypePool.Alloc()) ArrayType(&arrayTypePool, len, eleType);
}

ArrayType *ArrayType::New(Expr *len_expr, QualType eleType)
{
  return new (arrayTypePool.Alloc())
    ArrayType(&arrayTypePool, len_expr, eleType);
}

FuncType *FuncType::New(QualType derived, int funcSpec, FunctionDecl *fd)
{
  return new (funcTypePool.Alloc())
    FuncType(&funcTypePool, derived, funcSpec, fd);
}

FuncType *FuncType::New(QualType derived,
                        int funcSpec,
                        const ParamList &params,
                        FunctionDecl *fd)
{
  return new (funcTypePool.Alloc())
    FuncType(&funcTypePool, derived, funcSpec, params, fd);
}

PointerType *PointerType::New(QualType derived)
{
  return new (pointerTypePool.Alloc()) PointerType(&pointerTypePool, derived);
}

int ArithmType::Width() const
{
  // switch (m_tag) {
  // case T_BOOL:
  // case T_CHAR:
  //     return 1;
  // case T_INT:
  //     return intWidth_;
  // default:
  //     assert(false);
  // }

  return intWidth_;  // Make compiler happy
}

int ArithmType::Rank() const
{
  switch (m_tag) {
  case T_BOOL:
    return 0;
  case T_CHAR:
    return 1;
  case T_INT:
    return 3;
  default:
    // assert(m_tag & T_COMPLEX);
    Error("complex not supported yet");
  }
  return 0;
}

ArithmType *ArithmType::MaxType(ArithmType *lhs, ArithmType *rhs)
{
  if (lhs->IsInteger())
    lhs = ArithmType::IntegerPromote(lhs);
  if (rhs->IsInteger())
    rhs = ArithmType::IntegerPromote(rhs);
  auto ret = lhs->Rank() > rhs->Rank() ? lhs : rhs;
  if (lhs->Width() == rhs->Width())
    return ArithmType::New(ret->Tag());
  return ret;
}

/*
 * Converting from type specifier to type tag
 */
int ArithmType::Spec2Tag(int spec)
{
  return spec;
}

std::string ArithmType::Str() const
{
  std::string width = ":" + std::to_string(Width());

  switch (m_tag) {
  case T_BOOL:
    return "bool";

  case T_CHAR:
    return "char";

  case T_INT:
    return "int";
  case T_VOID:
    return "void";
  default:
    assert(false);
  }

  return "error";  // Make compiler happy
}

bool FuncType::Compatible(const Type &other) const
{
  auto otherFunc = other.ToFunc();
  // The other type is not an function type
  if (!otherFunc)
    return false;
  // TODO(wgtdkp): do we need to check the type of return value when deciding
  // compatibility of two function types ??
  if (!m_derived->Compatible(*otherFunc->m_derived))
    return false;
  if (m_params.size() != otherFunc->m_params.size())
    return false;

  auto thisIter = m_params.begin();
  auto otherIter = otherFunc->m_params.begin();
  while (thisIter != m_params.end()) {
    if (!(*thisIter)->Type()->Compatible(*(*otherIter)->Type()))
      return false;
    ++thisIter;
    ++otherIter;
  }

  return true;
}

std::string FuncType::Str() const
{
  auto str = m_derived->Str() + "(";
  auto iter = m_params.begin();
  for (; iter != m_params.end(); ++iter) {
    str += (*iter)->Type()->Str() + ", ";
  }

  if (m_params.size())
    str.resize(str.size() - 2);

  return str + ")";
}

// Fuck compiler course
std::string FuncType::QuadStr(const std::string &name) const
{
  auto str = m_derived->Str() + " " + name + "()\n";
  debug("%s param num:%d\n", name.c_str(), m_params.size());
  auto iter = m_params.begin();
  for (; iter != m_params.end(); ++iter) {
    str += "    para ";
    str += (*iter)->Type()->Str() + " ";
    str += (*iter)->Name() + "\n";
  }
  // if (m_params.size())
  //     str.resize(str.size() - 2);

  return str;
}
