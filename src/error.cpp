#include "error.h"

#include "ast.h"
#include "iobase.h"
#include "token.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

#ifdef PRINT_ERROR
extern std::ofstream errstream;
#endif  // PRINT_ERROR

int error_flag;

void Error(const char *format, ...)
{
  fprintf(stderr, "error: ");

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  fprintf(stderr, "\n");
}

static void VError(const SourceLocation &loc, const char *format, va_list args)
{
  fprintf(stderr, "%d:%d: error: ", loc.line, loc.column);
  vfprintf(stderr, format, args);
  fprintf(stderr, "\n    ");

  bool saw_no_space = false;
  int nspaces = 0;
  for (auto p = loc.line_begin; *p != '\n' && *p != 0; p++) {
    if (!saw_no_space && (*p == ' ' || *p == '\t')) {
      ++nspaces;
    }
    else {
      saw_no_space = true;
      fputc(*p, stderr);
    }
  }

  fprintf(stderr, "\n    ");
  for (unsigned i = 1; i + nspaces < loc.column; ++i)
    fputc(' ', stderr);
  fprintf(stderr, "^\n");
}

void Error(const SourceLocation &loc, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  VError(loc, format, args);
  va_end(args);
}

void Error(const Expr *expr, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  VError(expr->Tok()->m_loc, format, args);
  va_end(args);
}

void Error(const Token *tok, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  VError(tok->m_loc, format, args);
  va_end(args);
}

void Error(const Token *tok, const char _errno)
{
  if (tok->m_loc.line < 1 || _errno < 'a' || _errno > 'o') {
    Error("Illegal line number %d or error number %c.", tok->m_loc.line,
          _errno);
  }

#ifdef PRINT_ERROR
  errstream << tok->m_loc.line << " " << _errno << std::endl;
#endif  // PRINT_ERROR
  error_flag = 1;
  switch (_errno) {
  case 'a':  // 非法符号或不符合词法
    Error(tok, "unrecognized token \'%s\'", tok->m_value.c_str());
    break;
  case 'b':  // 名字重定义
    Error(tok, "redefinition of identifier \'%s\'", tok->m_value.c_str());
    break;
  case 'c':  // 未定义的名字
    Error(tok, "used undeclared identifier \'%s\'", tok->m_value.c_str());
    break;
  case 'd':  // 函数参数个数不匹配
    Error(tok, "too many(or less) parameters in call");
    break;
  case 'e':  // 函数参数类型不匹配
    Error(tok, "imcompatible parameter type");
    break;
  case 'f':  // 条件判断中出现不合法的类型
    Error(tok, "illegal type in condition expression");
    break;
  case 'g':  // 无返回值的函数存在不匹配的return语句
    Error(tok, "void function cannot return a value");
    break;
  case 'h':  // 有返回值的函数缺少return语句或存在不匹配的return语句
    Error(tok, "non-void function should return a proper value");
    break;
  case 'i':  // 数组的下标只能是整型表达式
    Error(tok, "subscript of array must be \'int\' type");
    break;
  case 'j':  // 不能改变常量的值
    Error(tok, "cannot assign to constant variable");
    break;
  case 'k':  // 应为分号
    Error(tok, "expected \';\' after %s.", tokenlist[tok->m_type]);
    break;
  case 'l':  // 应为右小括号’)’
    Error(tok, "expected \')\', we got %s.", tok->m_value.c_str());
    break;
  case 'm':  // 应为右中括号’]’
    Error(tok, "expected \']\', we got %s.", tok->m_value.c_str());
    break;
  case 'n':  // do-while语句中缺少 while
    Error(tok, "expected \'while\' in do/while loop");
    break;
  case 'o':  // 常量定义中=后面只能是整型或字符型常量
    Error(tok, "const variable definition must be initialized with \'int\' "
               "or \'char\' type");
    break;
  default:
    Error("unsupported error type!");
    break;
  }
}
