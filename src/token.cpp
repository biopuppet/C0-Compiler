#include "token.h"
#include "lexer.h"
#include "memory_pool.h"

#include <string>

#ifdef DEBUG_LEXER
#include <iomanip>
#endif

#ifdef PRINT_OUTPUT
extern std::ofstream outstream;
#endif  // PRINT_OUTPUT

static MemoryPoolImp<Token> tokenpool;
/*
 * Fastest and simplest way of recording all token names.
 * BUT requires index consistency with `enum TokenType`.
 */
const char *tokenlist[] = {"NATK",
                           "EOFTK",
                           "IDENFR",
                           "INTCON",
                           "CHARCON",
                           "STRCON",
                           "CONSTTK",
                           "INTTK",
                           "CHARTK",
                           "VOIDTK",
                           "MAINTK",
                           "IFTK",
                           "ELSETK",
                           "DOTK",
                           "WHILETK",
                           "FORTK",
                           "SCANFTK",
                           "PRINTFTK",
                           "RETURNTK",
                           "PLUS",
                           "MINU",
                           "MULT",
                           "DIV",
                           "LSS",
                           "LEQ",
                           "GRE",
                           "GEQ",
                           "EQL",
                           "NEQ",
                           "ASSIGN",
                           "SEMICN",
                           "COMMA",
                           "LPARENT",
                           "RPARENT",
                           "LBRACK",
                           "RBRACK",
                           "LBRACE",
                           "RBRACE"
#ifdef EXTRA_SWITCH
                           ,
                           "SWITCHTK",
                           "CASETK",
                           "DEFAULTTK",
                           "COLON"
#endif  // EXTRA_SWITCH
#ifdef EXTRA_MIDEXAM
                           ,
                           "SWITCHTK",
                           "CASETK",
                           "DEFAULTTK",
                           "COLON"
#endif  // EXTRA_MIDEXAM
};

const Token::TokenMap Token::m_kwtab = {{"const", CONSTTK},
                                        {"int", INTTK},
                                        {"char", CHARTK},
                                        {"void", VOIDTK},
                                        {"main", MAINTK},
                                        {"if", IFTK},
                                        {"else", ELSETK},
                                        {"do", DOTK},
                                        {"while", WHILETK},
                                        {"for", FORTK},
                                        {"scanf", SCANFTK},
                                        {"printf", PRINTFTK},
                                        {"return", RETURNTK}
#ifdef EXTRA_SWITCH
                                        ,
                                        {"switch", SWITCHTK},
                                        {"case", CASETK},
                                        {"default", DEFAULTTK}
#endif  // EXTRA_SWITCH
#ifdef EXTRA_MIDEXAM
                                        ,
                                        {"switch", SWITCHTK},
                                        {"case", CASETK},
                                        {"default", DEFAULTTK}
#endif  // EXTRA_MIDEXAM
};

std::ofstream &operator<<(std::ofstream &_outstream, const Token &_token)
{
#if defined(DEBUG_LEXER)
  _outstream.setf(std::ios::left);
  _outstream << std::setw(10) << tokenlist[_token.m_type] << " "
             << std::setw(10) << _token.m_value << std::setw(0)
             << _token.m_loc.line << ":" << _token.m_loc.column << '\t';
  //<< std::setw(10) << _token.m_loc.line_begin << std::endl;
  auto p = _token.m_loc.line_begin;
  while (p && *p != '\n' && *p != '\0')
    _outstream << *p++;
  _outstream << std::endl;
#elif defined(PRINT_OUTPUT)
  _outstream << tokenlist[_token.m_type] << " " << _token.m_value << std::endl;
#endif
  return _outstream;
}

std::ostream &operator<<(std::ostream &_outstream, const Token &_token)
{
  _outstream << tokenlist[_token.m_type] << " " << _token.m_value << std::endl;
  return _outstream;
}

Token *Token::New(const TokenType type)
{
  return new (tokenpool.Alloc()) Token(type);
}

Token *Token::New(const Token &other)
{
  return new (tokenpool.Alloc()) Token(other);
}

Token *Token::New(const TokenType type,
                  const SourceLocation &loc,
                  const std::string &str)
{
  // TODO: optimize with c++11
  return new (tokenpool.Alloc()) Token(type, loc, str);
}

void Token::Delete()
{
  tokenpool.Free(this);
}

/**
 * locate `offset`-th element in the stream, extend At()
 */
Token *TokenStream::At(size_t offset)
{
  auto it = begin() + offset;

  while (it == end()) {
    m_td->push_back(m_lexer->GetToken());
  }
  return *it;
}

/**
 * Print front token in the queue and *pop it out*.
 */
Token *TokenStream::PrintFront()
{
  if (empty()) {
    debug("Paser::PrintFront(): Empty queue! nothing to be printed.");
    return nullptr;
  }
#ifdef PRINT_OUTPUT
  outstream << *At(0);
#endif  // PRINT_OUTPUT
  return FlushFront();
}

/**
 * @brief Remove first token in TokenStream.
 * @return element just dropped
 */
Token *TokenStream::FlushFront()
{
  if (empty()) {
    debug("Paser::FlushFront(): Empty queue! nothing to be Poped.");
    return nullptr;
  }
  auto temp = GetFront();
  pop_front();
  return temp;
}
