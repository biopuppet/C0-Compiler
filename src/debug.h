#ifndef C0C_DEBUG_H
#define C0C_DEBUG_H

#define DEBUG_PARSER
#define DEBUG_LEXER
#define DEBUG_ERROR

// #define PRINT_PARSER
// #define PRINT_LEXER
// #define PRINT_ERROR

#define NDEBUG
// #define EXTRA_SWITCH
// #define EXTRA_MIDEXAM

#if !defined(DEBUG_PARSER) && !defined(DEBUG_LEXER) && !defined(DEBUG_ERROR)
/* submit mode */
#if defined(PRINT_PARSER) || defined(PRINT_LEXER) || defined(PRINT_PLACEHOLDER)
#define PRINT_OUTPUT
#endif  // defined PRINT_PARSER or PRINT_LEXER or ...

#if defined(PRINT_ERROR)
#ifdef PRINT_OUTPUT
#undef PRINT_OUTPUT  // course demands
#endif               // defined PRINT_OUTPUT
#endif               // defined PRINT_ERROR

#else /* debug mode */
#define PRINT_OUTPUT
#define PRINT_ERROR

#endif  // !DEBUG_*

// #define FUCK_COURSE_DEMANDS

#ifndef FUCK_COURSE_DEMANDS
#define COURSE_DEMANDS
#endif  // !FUCK_COURSE_DEMANDS

void _debug(const char *file, int line, const char *format, ...);

#undef debug
#ifdef NDEBUG
#define debug(...) ((void)0)
#else
#define debug(...) _debug(__FILE__, __LINE__, __VA_ARGS__)
#endif  // NDEBUG

/* Optimization */
#define CONST_PROPAGATION
// #define INLINE_EPILOG
// #define GLOBAL_REG_ALLOCATION
// #define REG_ARGS

#endif  // !C0C_DEBUG_H
