#ifndef C0C_ERROR_H
#define C0C_ERROR_H

struct SourceLocation;
struct Token;
class Expr;

void Error(const SourceLocation &loc, const char *format, ...);
void Error(const char *format, ...);
void Error(const Token *tok, const char *format, ...);
void Error(const Expr *expr, const char *format, ...);

/**
 * Fucked-up-by-course compromise
 */
void Error(const Token *tok, const char _errno);

#endif  // !C0C_ERROR_H
