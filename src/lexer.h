#ifndef C0C_LEXER_H
#define C0C_LEXER_H

#include "token.h"

class Lexer {
  friend class Parser;

public:
  Lexer(const std::string *srcbuf, const SourceLocation &loc)
    : Lexer(srcbuf, loc.filename, loc.line, loc.column)
  {
  }

  explicit Lexer(const std::string *srcbuf,
                 const char *filename = nullptr,
                 unsigned line = 1,
                 unsigned column = 1)
    : m_token(EOFTK), m_srcbuf(srcbuf)
  {
    m_pbuf = &(*srcbuf)[0];  // origin
    m_loc = {filename, m_pbuf, line, column};
  }
  Lexer() = delete;
  ~Lexer() {}

  void Analyse();
  Token *GetToken();

private:
  int GetChar();
  void UngetChar();
  void SkipSpace();
  Token *SkipIdentifier();
  Token *SkipNumber(int ch);
  Token *SkipString(int ch);
  Token *SkipChar(int ch);
  Token *MakeToken(const TokenType type);

  // Peek next char(8 bytes)
  int Peek() const
  {
    return (uint8_t)(*m_pbuf);
  }
  static bool IsExtendedAlnum(const int x)
  {
    return (isalnum(x) || x == '_' || x == '+' || x == '-' || x == '*' ||
            x == '/');
  }
  static bool IsSpecialAscii(const int x)
  {
    return ((x > 35 && x < 126) || x == 32 || x == 33);
  }

  // current lexing location
  SourceLocation m_loc;

  // current token read
  Token m_token;

  // source file buffer pointer(const)
  const std::string *m_srcbuf;

  // source file buffer pointer(follow lexer)
  const char *m_pbuf;
};

#endif  // !C0C_LEXER_H
