#ifndef C0C_TYPE_H
#define C0C_TYPE_H

#include "memory_pool.h"
#include "debug.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <list>
#include <string>
#include <vector>

// #include "ast.h"

class Scope;
struct Token;
class Expr;
class ParmVarDecl;
class FunctionDecl;

class Type;
class QualType;
class VoidType;
class Identifier;
class StringLiteral;
class CharLiteral;
class IntegerLiteral;

class ArithmType;
class DerivedType;
class ArrayType;
class FuncType;
class PointerType;
class EnumType;

enum ExprType {
  // Type specifier
  T_INVALID = 0x40,
  T_CHAR = 0x80,
  T_INT = 0x100,
  T_VOID = 0x200,
  T_BOOL = 0x400,

  F_NORETURN = 0x1000
};

struct Qualifier {
  enum { CONST = 0x01, MASK = CONST };
};

class Type;
class QualType {
public:
  QualType(Type *ptr, int quals = 0x00) : m_ptr(reinterpret_cast<intptr_t>(ptr))
  {
    assert((quals & ~Qualifier::MASK) == 0);
    m_ptr |= quals;
  }
  QualType(int quals = 0x00)
  {
    assert((quals & ~Qualifier::MASK) == 0);
    m_ptr |= quals;
  }

  operator bool() const
  {
    return !IsNull();
  }
  bool IsNull() const
  {
    return GetPtr() == nullptr;
  }
  const Type *GetPtr() const
  {
    return reinterpret_cast<const Type *>(m_ptr & ~Qualifier::MASK);
  }
  Type *GetPtr()
  {
    return reinterpret_cast<Type *>(m_ptr & ~Qualifier::MASK);
  }
  Type &operator*()
  {
    return *GetPtr();
  }
  const Type &operator*() const
  {
    return *GetPtr();
  }
  Type *operator->()
  {
    return GetPtr();
  }
  const Type *operator->() const
  {
    return GetPtr();
  }

  // Indicate whether the specified types are identical(exclude qualifiers).
  friend bool operator==(QualType lhs, QualType rhs)
  {
    return lhs.operator->() == rhs.operator->();
  }
  friend bool operator!=(QualType lhs, QualType rhs)
  {
    return !(lhs == rhs);
  }

  int Qual() const
  {
    return m_ptr & 0x1;
  }
  bool IsConstQualified() const
  {
    return m_ptr & Qualifier::CONST;
  }

private:
  intptr_t m_ptr;
};

class Type {
public:
  static const int intWidth_ = 4;
  static const int machineWidth_ = 8;

  bool operator!=(const Type &other) const = delete;
  bool operator==(const Type &other) const = delete;

  virtual bool Compatible(const Type &other) const
  {
    return m_complete == other.m_complete;
  }

  virtual ~Type() {}

  // For Debugging
  virtual std::string Str() const = 0;
  virtual int Width() const = 0;
  virtual int Align() const
  {
    return Width();
  }
  static int MakeAlign(int offset, int align)
  {
    if ((offset % align) == 0)
      return offset;
    if (offset >= 0)
      return offset + align - (offset % align);
    else
      return offset - align - (offset % align);
  }

  // static QualType MayCast(QualType type, bool inProtoScope = false);
  bool Complete() const
  {
    return m_complete;
  }
  void SetComplete(bool complete) const
  {
    m_complete = complete;
  }
  virtual bool IsScalar() const
  {
    return false;
  }

  virtual bool IsInteger() const
  {
    return false;
  }
  virtual bool IsChar() const
  {
    return false;
  }
  virtual bool IsBool() const
  {
    return false;
  }
  virtual bool IsVoidPointer() const
  {
    return false;
  }
  virtual bool IsVoid() const
  {
    return false;
  }

  virtual ArithmType *ToArithm()
  {
    return nullptr;
  }
  virtual const ArithmType *ToArithm() const
  {
    return nullptr;
  }
  virtual ArrayType *ToArray()
  {
    return nullptr;
  }
  virtual const ArrayType *ToArray() const
  {
    return nullptr;
  }
  virtual PointerType *ToPointer()
  {
    return nullptr;
  }
  virtual const PointerType *ToPointer() const
  {
    return nullptr;
  }
  virtual VoidType *ToVoid()
  {
    return nullptr;
  }
  virtual const VoidType *ToVoid() const
  {
    return nullptr;
  }
  virtual FuncType *ToFunc()
  {
    return nullptr;
  }
  virtual const FuncType *ToFunc() const
  {
    return nullptr;
  }
  virtual DerivedType *ToDerived()
  {
    return nullptr;
  }
  virtual const DerivedType *ToDerived() const
  {
    return nullptr;
  }

protected:
  Type(MemoryPool *pool, bool complete) : m_complete(complete), m_pool(pool) {}

  mutable bool m_complete;
  MemoryPool *m_pool;
};

class DerivedType : public Type {
public:
  QualType Derived() const
  {
    return m_derived;
  }
  void SetDerived(QualType derived)
  {
    m_derived = derived;
  }
  virtual DerivedType *ToDerived()
  {
    return this;
  }
  virtual const DerivedType *ToDerived() const
  {
    return this;
  }
  // virtual bool IsVoid() {
  //     return m_derived->IsVoid();
  // }

protected:
  DerivedType(MemoryPool *pool, QualType derived)
    : Type(pool, true), m_derived(derived)
  {
  }

  QualType m_derived;
};

class PointerType : public DerivedType {
public:
  static PointerType *New(QualType derived);
  virtual ~PointerType() {}
  virtual PointerType *ToPointer()
  {
    return this;
  }
  virtual const PointerType *ToPointer() const
  {
    return this;
  }
  virtual int Width() const
  {
    return 8;
  }
  virtual bool IsScalar() const
  {
    return true;
  }
  virtual bool IsVoidPointer() const
  {
    return m_derived->ToVoid();
  }
  virtual std::string Str() const
  {
    return m_derived->Str() + "*:" + std::to_string(Width());
  }

protected:
  PointerType(MemoryPool *pool, QualType derived) : DerivedType(pool, derived)
  {
  }
};

class VoidType : public Type {
public:
  static VoidType *New();
  virtual ~VoidType() {}
  virtual VoidType *ToVoid()
  {
    return this;
  }
  virtual const VoidType *ToVoid() const
  {
    return this;
  }
  virtual bool Compatible(const Type &other) const
  {
    return other.ToVoid();
  }
  virtual int Width() const
  {
    // Non-standard GNU extension
    return 1;
  }
  virtual std::string Str() const
  {
    return "void:1";
  }
  // virtual bool IsVoid()
  // {
  //     return true;
  // }

protected:
  explicit VoidType(MemoryPool *pool) : Type(pool, false) {}
};

class ArithmType : public Type {
public:
  static ArithmType *New(int typeSpec);

  virtual ~ArithmType() {}
  virtual ArithmType *ToArithm()
  {
    return this;
  }
  virtual const ArithmType *ToArithm() const
  {
    return this;
  }
  virtual bool Compatible(const Type &other) const
  {
    // C11 6.2.7 [1]: Two types have compatible type if their types are the
    // same But I would to loose this constraints: integer and pointer are
    // compatible if (IsInteger() && other.ToPointer())
    //   return other.Compatible(*this);
    return this == &other;
  }

  virtual int Width() const;
  virtual std::string Str() const;
  virtual bool IsScalar() const
  {
    return true;
  }
  virtual bool IsInteger() const
  {
    return m_tag & ExprType::T_INT;
  }
  virtual bool IsChar() const
  {
    return m_tag & ExprType::T_CHAR;
  }

  virtual bool IsVoid() const
  {
    return m_tag & ExprType::T_VOID;
  }

  int Tag() const
  {
    return m_tag;
  }

  int Rank() const;
  static ArithmType *IntegerPromote(ArithmType *type)
  {
    assert(type->IsInteger());
    if (type->Rank() < ArithmType::New(T_INT)->Rank())
      return ArithmType::New(T_INT);
    return type;
  }
  static ArithmType *MaxType(ArithmType *lhsType, ArithmType *rhsType);

protected:
  explicit ArithmType(MemoryPool *pool, int spec)
    : Type(pool, true), m_tag(Spec2Tag(spec))
  {
  }

private:
  static int Spec2Tag(int spec);

  int m_tag;
};

class ArrayType : public DerivedType {
public:
  static ArrayType *New(int len, QualType eleType);
  static ArrayType *New(Expr *expr, QualType eleType);
  virtual ~ArrayType()
  { /*delete m_derived;*/
  }

  virtual ArrayType *ToArray()
  {
    return this;
  }
  virtual const ArrayType *ToArray() const
  {
    return this;
  }
  virtual int Width() const
  {
    return m_derived->Width() * m_len;
  }
  virtual int Align() const
  {
    return m_derived->Align();
  }
  virtual std::string Str() const
  {
    return m_derived->Str() + "[]:" + std::to_string(Width());
  }

  int GetElementOffset(int idx) const
  {
    return m_derived->Width() * idx;
  }
  int Len() const
  {
    return m_len;
  }
  void SetLen(int len)
  {
    m_len = len;
  }
  virtual bool IsInteger() const
  {
    return m_derived->ToArithm()->IsInteger();
  }
  virtual bool IsChar() const
  {
    return m_derived->ToArithm()->IsChar();
  }

protected:
  ArrayType(MemoryPool *pool, Expr *lenExpr, QualType derived)
    : DerivedType(pool, derived), lenExpr_(lenExpr)
  {
    m_len = 4;  // lenExpr->Val();
    SetComplete(m_len >= 0);
  }

  ArrayType(MemoryPool *pool, int len, QualType derived)
    : DerivedType(pool, derived), lenExpr_(nullptr), m_len(len)
  {
    SetComplete(m_len >= 0);
  }
  Expr *lenExpr_;
  int m_len;
};

class FuncType : public DerivedType {
public:
  using ParamList = std::vector<ParmVarDecl *>;

public:
  static FuncType *New(QualType derived,
                       int funcSpec,
                       const ParamList &params,
                       FunctionDecl *fd = nullptr);
  static FuncType *
  New(QualType derived, int funcSpec, FunctionDecl *fd = nullptr);
  virtual ~FuncType() {}
  virtual FuncType *ToFunc()
  {
    return this;
  }
  virtual const FuncType *ToFunc() const
  {
    return this;
  }
  virtual bool Compatible(const Type &other) const;
  virtual int Width() const
  {
    return 1;
  }
  virtual std::string Str() const;
  std::string QuadStr(const std::string &name) const;
  const ParamList &Params() const
  {
    return m_params;
  }
  void SetParams(const ParamList &params)
  {
    m_params = params;
  }

  virtual bool IsVoid() const
  {
    return (m_func_spec & F_NORETURN) || m_derived->IsVoid() ||
           m_derived->ToVoid();
  }
  virtual bool IsChar() const
  {
    return m_derived->IsChar();
  }
  virtual bool IsInteger() const
  {
    return m_derived->IsInteger();
  }
  bool IsNonVoid() const
  {
    return !IsVoid();
  }

  void SetFuncDecl(FunctionDecl *fd)
  {
    m_func_decl = fd;
  }
  FunctionDecl *GetFuncDecl() const
  {
    return m_func_decl;
  }

protected:
  FuncType(MemoryPool *pool,
           QualType res_type,
           int funcspec,
           const ParamList &params,
           FunctionDecl *fd = nullptr)
    : DerivedType(pool, res_type), m_params(params), m_func_spec(funcspec),
      m_func_decl(fd)
  {
    SetComplete(false);
  }
  FuncType(MemoryPool *pool,
           QualType res_type,
           int funcspec,
           FunctionDecl *fd = nullptr)
    : DerivedType(pool, res_type), m_func_spec(funcspec), m_func_decl(fd)
  {
    SetComplete(false);
  }

private:
  ParamList m_params;
  int m_func_spec;
  FunctionDecl *m_func_decl;
};

#endif  // !C0C_TYPE_H
