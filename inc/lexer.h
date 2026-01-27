
#ifndef __LEXER_H__
#define __LEXER_H__

#include <stddef.h>

/* Enums */
typedef enum token_type_e {
  /* Divided into sections              */
  /* Binary format:                     */
  /* <1 nibble>  <1 nibble>  <2 nibble> */
  /*    type   -   subtype  -  entry    */

  /* TOKEN_KEYWORD */
  // {
  TOKEN_KEYWORD = 0x1000,

  KEYWORD_BASETYPE = 0x1100,
  KEYWORD_CHAR,
  KEYWORD_SHORT,
  KEYWORD_INT,
  KEYWORD_LONG,
  KEYWORD_FLOAT,
  KEYWORD_DOUBLE,
  KEYWORD_VOID,

  KEYWORD_SIG = 0x1200,
  KEYWORD_SIGNED,
  KEYWORD_UNSIGNED,

  KEYWORD_TYPE = 0x1300,
  KEYWORD_STRUCT,
  KEYWORD_UNION,
  KEYWORD_ENUM,
  KEYWORD_TYPEDEF,

  KEYWORD_CONTROL_FLOW = 0x1400,
  KEYWORD_IF,
  KEYWORD_ELSE,
  KEYWORD_FOR,
  KEYWORD_WHILE,
  KEYWORD_DO,
  KEYWORD_SWITCH,
  KEYWORD_CONTINUE,
  KEYWORD_BREAK,
  KEYWORD_CASE,
  KEYWORD_DEFAULT,
  KEYWORD_GOTO,
  KEYWORD_RETURN,

  KEYWORD_STORAGE_CLASS = 0x1500,
  KEYWORD_AUTO,
  KEYWORD_REGISTER,
  KEYWORD_VOLATILE,
  KEYWORD_STATIC,
  KEYWORD_EXTERN,

  KEYWORD_CONSTNESS = 0x1600,
  KEYWORD_CONST,

  KEYWORD_FUNCTION = 0x1700,
  KEYWORD_SIZEOF,
  KEYWORD_TYPEOF,

  KEYWORD_PREPROCESSOR = 0x1800,
  KEYWORD_DEFINE,
  KEYWORD_UNDEF,
  KEYWORD_INCLUDE,
  KEYWORD_PRAGMA,
  KEYWORD_LINE,
  KEYWORD_FILE,
  KEYWORD_ERROR,
  KEYWORD_IFDEF,
  KEYWORD_IFNDEF,
  KEYWORD_PP_IF,
  KEYWORD_PP_ELIF,
  KEYWORD_PP_ELSE,
  KEYWORD_PP_ENDIF,
  // }


  /* TOKEN_SPECIAL */
  // {
  TOKEN_SPECIAL = 0x2000,

  SPECIAL_PARENTHESES = 0x2100,
  SPECIAL_O_PARENTHESIS,
  SPECIAL_C_PARENTHESIS,

  SPECIAL_BRACE = 0x2200,
  SPECIAL_O_BRACE,
  SPECIAL_C_BRACE,

  SPECIAL_BRACKET = 0x2300,
  SPECIAL_O_BRACKET,
  SPECIAL_C_BRACKET,

  SPECIAL_SEPERATOR = 0x2400,
  SPECIAL_COMMA,
  SPECIAL_COLON,
  SPECIAL_SEMICOLON,

  SPECIAL_MEMORY = 0x2500,
  SPECIAL_DOT,
  SPECIAL_ARROW,

  SPECIAL_PREPROCESSOR = 0x2600,
  SPECIAL_HASH,
  SPECIAL_BACKSLASH,
  // }


  /* TOKEN_OPERATOR */
  // {
  TOKEN_OPERATOR = 0x3000,

  OPERATOR_ARITHMETIC = 0x3100,
  OPERATOR_PLUS,
  OPERATOR_MINUS,
  OPERATOR_MUL,
  OPERATOR_DIV,
  OPERATOR_MOD,

  OPERATOR_RELATIONAL = 0x3200,
  OPERATOR_EQ,
  OPERATOR_LESS,
  OPERATOR_GREATER,
  OPERATOR_LEQ,
  OPERATOR_GEQ,
  OPERATOR_NOT_EQ,

  OPERATOR_LOGICAL = 0x3300,
  OPERATOR_LOGICAL_NOT,
  OPERATOR_LOGICAL_OR,
  OPERATOR_LOGICAL_AND,

  OPERATOR_BITWISE = 0x3400,
  OPERATOR_BITWISE_NOT,
  OPERATOR_BITWISE_OR,
  OPERATOR_BITWISE_AND,
  OPERATOR_BITWISE_XOR,
  OPERATOR_BITWISE_LSHIFT,
  OPERATOR_BITWISE_RSHIFT,

  OPERATOR_ASSIGN_ARITHMETIC = 0x3500,
  OPERATOR_ASSIGN,
  OPERATOR_ASSIGN_PLUS,
  OPERATOR_ASSIGN_MINUS,
  OPERATOR_ASSIGN_MUL,
  OPERATOR_ASSIGN_DIV,
  OPERATOR_ASSIGN_MOD,

  OPERATOR_ASSIGN_BITWISE = 0x3600,
  OPERATOR_ASSIGN_OR,
  OPERATOR_ASSIGN_AND,
  OPERATOR_ASSIGN_XOR,
  OPERATOR_ASSIGN_LSHIFT,
  OPERATOR_ASSIGN_RSHIFT,

  OPERATOR_UNARY = 0x3700,
  OPERATOR_INCREMENT,
  OPERATOR_DECREMENT,

  OPERATOR_TRINARY = 0x3800,
  OPERATOR_QUESTION_MARK,
  // }
  
  /* TOKEN_CONSTANT */
  // {
  TOKEN_CONSTANT = 0x4000,

  // CONSTANT_STRING,
  CONSTANT_STRING = 0x4100,

  // CONSTANT_CHAR,
  CONSTANT_CHAR = 0x4200,

  // CONSTANT_FLOAT,
  CONSTANT_FLOAT = 0x4300,

  // CONSTANT_INT,
  CONSTANT_INT = 0x4400,
  CONSTANT_BIN,
  CONSTANT_OCT,
  CONSTANT_DEC,
  CONSTANT_HEX_LITTLE,
  CONSTANT_HEX_BIG,
  // }

  /* TOKEN_IDENTIFIER */
  // {
  // TOKEN_IDENTIFIER
  TOKEN_IDENTIFIER = 0x5000,
  // }

} token_type_e;


//#define  __IMPLEMENT__
#ifdef __IMPLEMENT__
/* Consts */
  // KEYWORDS
const char *const tokens_keyword_basetype[] = {
    "char", "short", "int", "long", "float", "double", "void" };
const char *const tokens_keyword_sign[] = {
    "signed", "unsigned" };
const char *const tokens_keyword_type[] = {
    "struct", "union", "enum", "typedef" };
const char *const tokens_keyword_control[] = {
    "if", "else", "for", "while", "do", "switch",
    "continue", "break", "case", "default", "goto", "return" };
const char *const tokens_keyword_storage[] = {
    "auto", "register", "volatile", "static", "extern" };
const char *const tokens_keyword_constness[] = {
    "const" };
const char *const tokens_keyword_function[] = {
    "sizeof" };
const char *const tokens_keyword_preprocessor[] = {
    "define", "undef", "include", "pragma", "line", "file", "error",
    "ifdef", "ifndef", "if", "elif", "else", "endif" };

  // SPECIAL
const char *const tokens_special_parentheses[] = { "(", ")" };
const char *const tokens_special_brace[] = { "{", "}" };
const char *const tokens_special_bracket[] = { "[", "]" };
const char *const tokens_special_seperator[] = { ",", ":", ";" };
const char *const tokens_special_memory[] = { ".", "->" };
const char *const tokens_special_preprocessor[] = { "#", "\\" };

  // OPERATORS
const char *const tokens_operator_arithmetic[] = {
    "+", "-", "*", "/", "%" };
const char *const tokens_operator_relational[] = {
    "==", "<", ">", "<=", ">=", "!=" };
const char *const tokens_operator_logical[] = {
    "!", "||", "&&" };
const char *const tokens_operator_bitwise[] = {
    "~", "|", "&", "^", "<<" , ">>" };
const char *const tokens_operator_assign_arithmetic[] = {
    "=", "+=", "-=", "*=", "/=", "%=" };
const char *const tokens_operator_assign_bitwise[] = {
    "|=", "&=", "^=", "<<=" ,">>=" };
const char *const tokens_operator_unary[] = {
    "++", "--" };
const char *const tokens_operator_trinary[] = {
    "?" };

const char *const *const tokens_str[][16] = { {
    tokens_keyword_basetype,           tokens_keyword_sign,
    tokens_keyword_type,               tokens_keyword_control,
    tokens_keyword_storage,            tokens_keyword_constness,
    tokens_keyword_function,           tokens_keyword_preprocessor
    }, {
    tokens_special_parentheses,        tokens_special_brace,
    tokens_special_bracket,            tokens_special_seperator,
    tokens_special_memory,             tokens_special_preprocessor,
    }, {
    tokens_operator_arithmetic,        tokens_operator_relational,
    tokens_operator_logical,           tokens_operator_bitwise,
    tokens_operator_assign_arithmetic, tokens_operator_assign_bitwise,
    tokens_operator_unary,             tokens_operator_trinary,
    } };

#endif /* __IMPLEMENT__ */


typedef enum lexer_err_e {
  LEXER_OK = 0,
  LEXER_INVALID_OPERATOR,
  LEXER_INVALID_ESCAPE,
} lexer_err_e;



/* Types */
typedef struct token_t {
  char        *str;
  token_type_e type;
} token_t;

typedef struct token_line_t {
  size_t   cap;
  size_t   n_tokens;
  token_t *tokens;
} token_line_t;

typedef struct token_tab_t {
  size_t        cap;
  size_t        n_lines;
  token_line_t *token_lines;
} token_tab_t;



/* Functions */
lexer_err_e lexer_parse(const char  *buf,
                        size_t       buf_size,
                        token_tab_t *token_tab);


#endif // __LEXER_H__
