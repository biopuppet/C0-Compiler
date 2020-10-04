#include "lexer.h"
#include "error.h"

#include <cassert>
#include <ctype.h>
#include <iostream>

#ifdef PRINT_OUTPUT
extern std::ofstream outstream;
#endif  // PRINT_OUTPUT

/**
 * @brief Get a uint_8 char from `m_pbuf` and update `m_loc`.
 * (Core function in lexer)
 */
int Lexer::GetChar()
{
    int ch = Peek();

    ++m_pbuf;
    if (ch == '\n') {
        ++m_loc.line;
        m_loc.column = 1;
        m_loc.line_begin = m_pbuf;
    }
    else if (ch == '\r') {  // skip \r
        return GetChar();
    }
    else {
        ++m_loc.column;
    }
    return ch;
}

/**
 * @brief Put back a uint_8 char into `m_pbuf` and update `m_loc`.
 */
void Lexer::UngetChar()
{
    if (0) {
        Error("Lexer: reached begin of file.");
        return;
    }
    int ch = *--m_pbuf;
    // if (ch == '\n' && m_pbuf[-1] == '\\')
    // {
    //     --m_loc.line;
    //     --m_pbuf;
    //     return UngetChar();
    // }
    ch == '\n' ? --m_loc.line : --m_loc.column;
}

void Lexer::SkipSpace()
{
    while (isspace(Peek())) {
        GetChar();
    }
}

Token *Lexer::SkipIdentifier()
{
    // assum that ch is alnum or '_' (caller guarantees)
    while (1) {
        int ch = GetChar();
        if (ch == '\n' || ch == '(' || ch == ')' || ch == '{' || ch == ' ' ||
            ch == '=' || ch == ';' || ch == '[' || ch == ']' || ch == '+' ||
            ch == '-' || ch == '*' || ch == '/' || ch == '>' || ch == '<' ||
            ch == '!' || ch == ',') {
            UngetChar();
            break;
        }
        else if (isalnum(ch) || ch == '_') {
            m_token.m_value.push_back(ch);
        }
        else {
            Error(&m_token, 'a');
        }
    }

    if (Token::IsReservedKeyword(m_token.m_value)) {
        m_token.m_type = Token::Type(m_token.m_value);
    }
    else  // just regular identifier
    {
        m_token.m_type = IDENFR;
    }
    return Token::New(m_token);
}

Token *Lexer::SkipNumber(int ch)
{
    // assum that ch is digit (caller guarantees)
    assert(isdigit(ch));
    do {
        m_token.m_value.push_back(ch);
        ch = GetChar();
    } while (isdigit(ch));
    UngetChar();
    m_token.m_type = INTCON;
    return Token::New(m_token);
}

Token *Lexer::SkipString(int ch)
{
    // assum that ch is '\"' (caller guarantees)
    assert(ch == '\"');
    ch = GetChar();
    while (IsSpecialAscii(ch)) {
        if (ch == '\\') {
            m_token.m_value.push_back(ch);
        }
        m_token.m_value.push_back(ch);
        ch = GetChar();
    }
    if (ch == '\"') {
        m_token.m_type = STRCON;
    }
    else if (ch == '\0') {
        // TODO error handling
        m_token.m_type = EOFTK;
    }
    /* error a */
    else {
        // TODO error handling
        m_token.m_type = NATK;
        int count = 1;
        while (ch != '\n') {
            ch = GetChar();
            ++count;
            if (ch == '\"') {
                Error(&m_token, 'a');
                m_token.m_type = STRCON;
                return Token::New(m_token);
            }
        }
        while (count--) {
            UngetChar();
        }
        Error(&m_token, 'a');
        m_token.m_type = STRCON;
    }
    return Token::New(m_token);
}

Token *Lexer::SkipChar(int ch)
{
    // assum that ch is '\'' (caller guarantees)
    auto ch1 = GetChar();
    auto ch2 = '\0';

    if (ch1 == '\0') {
        return MakeToken(EOFTK);
    }
    ch2 = GetChar();
    if (IsExtendedAlnum(ch1) && ch2 == '\'') {
        // only record ch between ' and '
        m_token.m_value.push_back(ch1);
        m_token.m_type = CHARCON;
        return Token::New(m_token);
    }
    // else if (ch1 == '\\' && isalpha(ch2) && GetChar() == '\'')
    // {
    //     // escape char
    //     m_token.m_value.push_back(ch1);
    //     m_token.m_value.push_back(ch2);
    //     m_token.m_type = CHARCON;
    //     return m_token;
    // }

    /* unrecognized char */
    else if (ch2 == '\'') {
        // TODO error handling
        Error(&m_token, 'a');

        m_token.m_value.push_back(ch1);
        m_token.m_type = CHARCON;
        return Token::New(m_token);
    }
    else {
        int count = 1;
        while (ch2 != '\n') {
            ch2 = GetChar();
            ++count;
            if (ch2 == '\'') {
                m_token.m_value.push_back(ch1);
                m_token.m_type = CHARCON;
                return Token::New(m_token);
            }
        }
        while (count--) {
            UngetChar();
        }
        m_token.m_type = CHARCON;
        m_token.m_value.push_back(ch1);
        Error(&m_token, 'a');

        return Token::New(m_token);
    }
}

/**
 * @brief Get a token from pbuf.
 * @implementation
 *   Mark start location after filtered spaces such that
 *   MakeToken can calculate the start & end position.
 */
Token *Lexer::GetToken()
{
    SkipSpace();

    // Mark start location
    m_token.m_loc = Lexer::m_loc;

    // Clear std::string m_value for next token
    m_token.m_value.resize(0);

    auto ch = GetChar();

    if (isalpha(ch) || ch == '_') {
        UngetChar();
        return SkipIdentifier();
    }
    if (isdigit(ch)) {
        return SkipNumber(ch);
    }
    // Get first `real` char
    switch (ch) {
    case '\'':
        return SkipChar(ch);
    case '\"':
        return SkipString(ch);
    case '<': {
        if (Peek() == '=') {
            GetChar();
            return MakeToken(LEQ);
        }
        else {
            return MakeToken(LSS);
        }
    }

    case '>': {
        if (Peek() == '=') {
            GetChar();
            return MakeToken(GEQ);
        }
        else {
            return MakeToken(GRE);
        }
    }

    case '=': {
        if (Peek() == '=') {
            GetChar();
            return MakeToken(EQL);
        }
        else  // TODO check
        {
            return MakeToken(ASSIGN);
        }
    }

    case '!': {
        if (Peek() == '=') {
            GetChar();
            return MakeToken(NEQ);
        }
        else  // TODO check or extend !
        {
            return MakeToken(NATK);
        }
    }

#ifdef EXTRA_SWITCH
    case ':':
        return MakeToken(COLON);
#endif  // EXTRA_SWITCH

#ifdef EXTRA_MIDEXAM
    case 'x':
        return MakeToken();
#endif  // EXTRA_MIDEXAM

    case '+':
        return MakeToken(PLUS);
    case '-':
        return MakeToken(MINU);
    case '*':
        return MakeToken(MULT);
    case '/':
        return MakeToken(DIV);
    case ';':
        return MakeToken(SEMICN);
    case ',':
        return MakeToken(COMMA);
    case '(':
        return MakeToken(LPARENT);
    case ')':
        return MakeToken(RPARENT);
    case '[':
        return MakeToken(LBRACK);
    case ']':
        return MakeToken(RBRACK);
    case '{':
        return MakeToken(LBRACE);
    case '}':
        return MakeToken(RBRACE);
    case '\0':
        return MakeToken(EOFTK);
    default:
        Error(&m_token, 'a');
        return GetToken();
    }
}

/**
 * @brief Make a token with Lexer::m_token(type).
 * @param type Token type that should be of m_token.
 */
Token *Lexer::MakeToken(const TokenType type)
{
    m_token.m_type = type;
    auto &value = m_token.m_value;
    value.resize(0);
    const char *p = m_token.m_loc.loc();
    for (; p < m_pbuf; ++p) {
        if (p[0] == '\n') {
            value.pop_back();
        }
        else {
            value.push_back(p[0]);
        }
    }
    return Token::New(m_token);
}

/*
 * Course demand
 */
void Lexer::Analyse()
{
#ifdef PRINT_OUTPUT
    for (auto token = GetToken(); token->m_type != EOFTK; token = GetToken()) {
        if (token->m_type != NATK) {
            outstream << *token;
            token->Delete();
        }
    }
#endif  // PRINT_OUTPUT
}
