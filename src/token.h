#ifndef C0C_TOKEN_H
#define C0C_TOKEN_H

#include "debug.h"

#include <deque>
#include <fstream>
#include <unordered_map>

extern const char *tokenlist[];

typedef struct SourceLocation {
  const char *filename;
  const char *line_begin;
  unsigned line;
  unsigned column;

  const char *loc() const
  {
    return line_begin + column - 1;
  }
} SourceLocation;

/*
 * Token types enum for controlling tokens in global variable `tokenlist`.
 * Note: requires `TokenType` start from 0 to be the index of `tokenlist`
 */
enum TokenType {
  NATK = 0,  // not a token
  EOFTK,     // end of file
  IDENFR,    // [_a-z0-9]
  INTCON,    // [0-9]
  CHARCON,   // 'x'
  STRCON,    // "abc"
  CONSTTK,   // const
  INTTK,     // int
  CHARTK,    // char
  VOIDTK,    // void
  MAINTK,    // main
  IFTK,      // if
  ELSETK,    // else
  DOTK,      // do
  WHILETK,   // while
  FORTK,     // for
  SCANFTK,   // scanf
  PRINTFTK,  // printf
  RETURNTK,  // return

  PLUS,     // +
  MINU,     // -
  MULT,     // *
  DIV,      // /
  LSS,      // <
  LEQ,      // <=
  GRE,      // >
  GEQ,      // >=
  EQL,      // ==
  NEQ,      // !=
  ASSIGN,   // =
  SEMICN,   // ;
  COMMA,    // ,
  LPARENT,  // (
  RPARENT,  // )
  LBRACK,   // [
  RBRACK,   // ]
  LBRACE,   // {
  RBRACE,   // }
  CAST,
#ifdef EXTRA_SWITCH
  ,
  SWITCHTK,   // switch
  CASETK,     // case
  DEFAULTTK,  // default
  COLON       // :
#endif        // EXTRA_SWITCH
#ifdef EXTRA_MIDEXAM
  ,
  SWITCHTK,   // switch
  CASETK,     // case
  DEFAULTTK,  // default
  COLON       // :
#endif        // EXTRA_MIDEXAM
};

struct Token {
public:
  // hash table implementation
  using TokenMap = std::unordered_map<std::string, TokenType>;

public:
  static Token *New(const TokenType type);
  static Token *New(const Token &other);
  static Token *New(const TokenType type,
                    const SourceLocation &loc,
                    const std::string &value);

  static TokenType Type(const std::string &_str)
  {
    if (IsReservedKeyword(_str)) {
      return m_kwtab.at(_str);
    }
    return NATK;  // TODO get this right
  }

  static bool IsCmpOp(const TokenType type)
  {
    return (type == TokenType::LSS || type == TokenType::LEQ ||
            type == TokenType::GRE || type == TokenType::GEQ ||
            type == TokenType::NEQ || type == TokenType::EQL);
  }

  static bool IsPlusMinusOp(const TokenType type)
  {
    return (type == TokenType::PLUS || type == TokenType::MINU);
  }

  static bool IsMultDivOp(const TokenType type)
  {
    return (type == TokenType::MULT || type == TokenType::DIV);
  }

  static bool IsReservedKeyword(const char *_str)
  {
    return m_kwtab.find(_str) != m_kwtab.end();
  }

  static bool IsReservedKeyword(const std::string &_str)
  {
    return m_kwtab.find(_str) != m_kwtab.end();
  }

  bool IsReservedKeyword() const
  {
    return m_kwtab.find(m_value) != m_kwtab.end();
  }
  TokenType Type() const
  {
    return m_type;
  }

  void Delete();

  Token(const TokenType _type,
        const SourceLocation &_srcloc,
        const std::string &_value)
    : m_loc(_srcloc), m_value(_value), m_type(_type)
  {
  }

  Token(const TokenType _type, const std::string &_value)
    : m_value(_value), m_type(_type)
  {
  }

  Token(const TokenType type = NATK) : m_type(type) {}

  ~Token() {}

  bool operator!=(const Token &tok)
  {
    return this->m_type != tok.m_type;
  }

  bool operator==(const Token &tok)
  {
    return this->m_type == tok.m_type;
  }

  friend std::ofstream &operator<<(std::ofstream &_outstream,
                                   const Token &_token);
  friend std::ostream &operator<<(std::ostream &_outstream,
                                  const Token &_token);

  SourceLocation m_loc;
  std::string m_value;
  TokenType m_type;

  static const TokenMap m_kwtab;
};

class Lexer;
class TokenStream {
  using TokenDeque = std::deque<Token *>;
  using reference = Token *&;
  /* ref:https://blog.csdn.net/Jocodeoe/article/details/8275082 */
  using const_reference = Token *const &;

  typedef size_t size_type;

public:
  TokenStream(Lexer *lexer) : m_lexer(lexer), m_td(new TokenDeque()) {}

  ~TokenStream() {}

  Token *At(size_t offset);

  TokenType GetType(size_t offset)
  {
    return At(offset)->m_type;
  }

  const std::string &GetName(size_t offset)
  {
    return At(offset)->m_value;
  }

  Token *GetFront()
  {
    return At(0);
  }

  TokenType GetFrontType()
  {
    return At(0)->m_type;
  }

  const std::string &GetFrontName()
  {
    return At(0)->m_value;
  }

  Token *PrintFront();
  Token *FlushFront();

  Token *prev() const noexcept
  {
    return m_prev;
  }

  TokenDeque::iterator begin()
  {
    return m_td->begin();
  }

  TokenDeque::iterator end()
  {
    return m_td->end();
  }

  void clear() noexcept
  {
    m_td->clear();
  }

  bool empty() const noexcept
  {
    return m_td->empty();
  }

  Token *operator[](size_type n)
  {
    return At(n);
  }

  Token *operator=(TokenStream &ts) = delete;

private:
  void pop_front()
  {
    m_prev = At(0);
    m_td->pop_front();
  }

  // previous element pointer
  Token *m_prev{nullptr};

  // token provider
  Lexer *m_lexer;

  // encapsulated token queue pointer
  TokenDeque *m_td;
};

#endif  // !C0C_TOKEN_H
