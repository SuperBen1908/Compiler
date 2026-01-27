
#include <stdlib.h>
#include <string.h>
#include "../inc/lexer.h"

#define CAP         32
#define CAP_EXP_THR (1<<20)


lexer_err_e lexer_parse(const char  *buf,
                        size_t       buf_size,
                        token_tab_t *token_tab)
{
  lexer_err_e err = LEXER_OK; 

  typedef enum char_type_e {
    INVALID_CHAR = 0,
    LETTER,
    NUMBER,
    OPERATOR,
    PARENTHESES,
    QUOTES,
    SPECIAL,
    SPACE,
  } char_type_e;
  char_type_e char_type_lut[256] = {0};
  char escape_characters[256] = {0};

  for (char l = 'A'; l < 'Z'; ++l)
    char_type_lut[l] = LETTER;
  for (char l = 'a'; l < 'z'; ++l)
    char_type_lut[l] = LETTER;
  char_type_lut['_'] = LETTER;

  for (char i = 0; i < 10; ++i)
    char_type_lut[i]  = NUMBER;

  char_type_lut['+']  = OPERATOR;    char_type_lut['-']  = OPERATOR;
  char_type_lut['*']  = OPERATOR;    char_type_lut['/']  = OPERATOR;
  char_type_lut['%']  = OPERATOR;    char_type_lut['=']  = OPERATOR;
  char_type_lut['?']  = OPERATOR;    char_type_lut[':']  = OPERATOR;
  char_type_lut['<']  = OPERATOR;    char_type_lut['>']  = OPERATOR;
  char_type_lut['^']  = OPERATOR;    char_type_lut['|']  = OPERATOR;
  char_type_lut['&']  = OPERATOR;    char_type_lut['.']  = OPERATOR;
  char_type_lut[';']  = OPERATOR;

  char_type_lut['(']  = PARENTHESES; char_type_lut[')']  = PARENTHESES;
  char_type_lut['{']  = PARENTHESES; char_type_lut['}']  = PARENTHESES;
  char_type_lut['[']  = PARENTHESES; char_type_lut[']']  = PARENTHESES;

  char_type_lut['\"'] = QUOTES;      char_type_lut['\''] = QUOTES;
                                     
  char_type_lut['.']  = SPECIAL;
  char_type_lut['#']  = SPECIAL;
  char_type_lut['\\'] = SPECIAL;
                                     
  char_type_lut[' ']  = SPACE;       char_type_lut['\t'] = SPACE;
  char_type_lut['\n'] = SPACE;       char_type_lut['\r'] = SPACE;


  escape_characters['t']  = '\t'; escape_characters['v']  = '\v';
  escape_characters['r']  = '\r'; escape_characters['n']  = '\n';
  escape_characters['a']  = '\a'; escape_characters['b']  = '\b';
  escape_characters['e']  = '\e'; escape_characters['f']  = '\f';
  escape_characters['\"'] = '\"'; escape_characters['\''] = '\'';
  escape_characters['\\'] = '\\';

  token_tab->cap = CAP;
  token_tab->n_lines = 0;
  token_tab->token_lines = calloc(sizeof(*token_tab->token_lines), token_tab->cap);
  if (!token_tab->token_lines)
    goto defer;

  const char *p = buf;
  while (p - buf < buf_size) {
    while (p - buf < buf_size && char_type_lut[*p] == SPACE) {
      if (*p == '\n') {

        /* Add token to current line */
        if (token_tab->n_lines >= token_tab->cap) {
          if (token_tab->cap < CAP_EXP_THR)
            token_tab->cap <<= 1;
          else
            token_tab->cap += CAP_EXP_THR;

          void *ret = realloc(token_tab->token_lines, token_tab->cap * sizeof(*token_tab->token_lines));
          if (ret) {
          }
        }
        
        token_tab->token_lines++;
      }
      ++p;
    }

    const char *tok_start = p;
    token_type_e tok_type = 0;


    switch (char_type_lut[*p]) {
      case LETTER:
      {
        /* First parse it as identifier, to be possibly changed later */
        while (p - buf < buf_size && char_type_lut[*p] == LETTER
                                  && char_type_lut[*p] == NUMBER)
          ++p;

        tok_type = TOKEN_IDENTIFIER;
      }
      break;
      case NUMBER:
      {
        if (*p != '0') {
          tok_type = CONSTANT_DEC;
          /* Decimal */
          while (p - buf < buf_size && char_type_lut[*p] == NUMBER)
            ++p;
        }

        if (*p == '0' && tok_type != CONSTANT_DEC) {
          /* starts with 0, advance ot next character and parse it */
          ++p;
          if (p - buf < buf_size)
            break;

          /* 0b%d -> binary, 0%d -> oct, 0x%d -> hex */
          switch (*p) {
            case 'b':
            case 'B':
            {
              tok_type = CONSTANT_BIN;

              ++p;
              if (p - buf < buf_size)
                break;

              /* Accepts only 01's after 0b | 0B */
              while (p - buf < buf_size &&
                    (*p == '0' || *p == '1'))
                ++p;
            }
            break;
            case 'x':
              tok_type = CONSTANT_HEX_LITTLE;
            case 'X':
            {
              if (tok_type)
                tok_type = CONSTANT_HEX_BIG;

              ++p;
              if (p - buf < buf_size)
                break;

              /* Accepts only 0-9 | A-F | a-f after 0x | 0X */
              if (tok_type == CONSTANT_HEX_BIG) {
                while (p - buf < buf_size     &&
                      (*p >= 'a' && *p <= 'f' ||
                       char_type_lut[*p] == NUMBER))
                  ++p;
              }
              else {
                while (p - buf < buf_size     &&
                      (*p >= 'A' && *p <= 'F' ||
                       char_type_lut[*p] == NUMBER))
                  ++p;
              }
            }
            break;
            default:
            {
              if (char_type_lut[*p] != NUMBER || *p == 0 || *p > '7')
                break;

              /* Accepts only 0-7 after 0%d */
              tok_type = CONSTANT_OCT;

              while (p - buf < buf_size &&
                    (*p >= '0' && *p <= '7'))
                ++p;
            }
            break;
          }

          if (p - buf < buf_size)
            break;
        }

        if (*p == '.') {
          tok_type = CONSTANT_FLOAT;

          /* Float */
          while (p - buf < buf_size &&
                (char_type_lut[*p] == NUMBER))
            ++p;
        }

      }
      break;
      case OPERATOR:
      {
        int end = 0;
        /* First operator */
        switch (*p) {
          case '+':
            tok_type = OPERATOR_PLUS;
          break;
          case '-':
            tok_type = OPERATOR_MINUS;
          break;
          case '*':
            tok_type = OPERATOR_MUL;
          break;
          case '/':
            tok_type = OPERATOR_DIV;
          break;
          case '%':
            tok_type = OPERATOR_MOD;
          break;

          case '<':
            tok_type = OPERATOR_LESS;
          break;
          case '>':
            tok_type = OPERATOR_GREATER;
          break;

          case '!':
            tok_type = OPERATOR_LOGICAL_NOT;
          break;
          case '~':
          {
            tok_type = OPERATOR_BITWISE_NOT;
            end = 1;
          }
          break;
          case '&':
            tok_type = OPERATOR_BITWISE_AND;
          break;
          case '|':
            tok_type = OPERATOR_BITWISE_OR;
          break;
          case '^':
            tok_type = OPERATOR_BITWISE_XOR;
          break;

          case '=':
            tok_type = OPERATOR_ASSIGN;
          break;

          case '?':
          {
            tok_type = OPERATOR_QUESTION_MARK;
            end = 1;
          }
          break;

          case ':':
          {
            tok_type = SPECIAL_COLON;
            end = 1;
          }
          break;
          case ',':
          {
            tok_type = SPECIAL_COMMA;
            end = 1;
          }
          break;
          case ';':
          {
            tok_type = SPECIAL_SEMICOLON;
            end = 1;
          }
          break;
        }


        if (end)
          break;

        /* Second operator */
        ++p;
        if (p - buf < buf_size ||
            char_type_lut[*p] != OPERATOR)
          break;

        /* double operation */
        if (p[-1] == *p) {
          switch (tok_type) {
            case OPERATOR_PLUS:        /* ++ */
            case OPERATOR_MINUS:       /* -- */
            case OPERATOR_ASSIGN:      /* == */
            case OPERATOR_BITWISE_AND: /* && */
            case OPERATOR_BITWISE_OR:  /* || */
              end = 1;
            /* Don't end because <<= and >>= */
            case OPERATOR_LESS:
            case OPERATOR_GREATER:
            {

              if ((tok_type & OPERATOR_ARITHMETIC) == OPERATOR_ARITHMETIC) {
                tok_type ^= OPERATOR_ARITHMETIC;
                tok_type ^= OPERATOR_UNARY;
                break;
              }

              if (tok_type == OPERATOR_ASSIGN) {
                tok_type = OPERATOR_EQ;
                break;
              }

              if ((tok_type & OPERATOR_RELATIONAL) == OPERATOR_RELATIONAL) {
                tok_type ^= OPERATOR_RELATIONAL;
                tok_type ^= OPERATOR_BITWISE;
                tok_type += (OPERATOR_BITWISE_LSHIFT ^ OPERATOR_BITWISE) -
                            (OPERATOR_LESS ^ OPERATOR_RELATIONAL);
                break;
              }

              if ((tok_type & OPERATOR_BITWISE) == OPERATOR_BITWISE) {
                tok_type ^= OPERATOR_BITWISE;
                tok_type ^= OPERATOR_LOGICAL;
                break;
              }
            }
            break;
          }
        }

        if (end)
          break;

        /* operator and assignment */
        if (*p == '=') {
          switch (tok_type) {
            case OPERATOR_PLUS:
            case OPERATOR_MINUS:
            case OPERATOR_MUL:
            case OPERATOR_DIV:
            case OPERATOR_MOD:
            {
              tok_type ^= OPERATOR_ARITHMETIC;
              tok_type ^= OPERATOR_ASSIGN_ARITHMETIC;
              tok_type += (OPERATOR_ASSIGN_PLUS ^ OPERATOR_ASSIGN_ARITHMETIC) -
                          (OPERATOR_PLUS ^ OPERATOR_ARITHMETIC);
            }
            break;

            case OPERATOR_LESS:
            case OPERATOR_GREATER:
            {
              tok_type += OPERATOR_LEQ -
                          OPERATOR_LESS;
            }
            break;

            case OPERATOR_LOGICAL_NOT:
            {
              tok_type = OPERATOR_NOT_EQ;
            }
            break;

            case OPERATOR_BITWISE_OR:
            case OPERATOR_BITWISE_AND:
            case OPERATOR_BITWISE_XOR:
            {
              tok_type ^= OPERATOR_BITWISE;
              tok_type ^= OPERATOR_ASSIGN_BITWISE;
              tok_type += (OPERATOR_ASSIGN_OR ^ OPERATOR_ASSIGN_BITWISE) -
                          (OPERATOR_BITWISE_OR ^ OPERATOR_BITWISE);
            }
            break;
          }
        }

        /* Arrow */
        if (tok_type == OPERATOR_MINUS && *p == '>') {
          tok_type = SPECIAL_ARROW;
          break;
        }

        /* Third operator */
        ++p;
        if (p - buf < buf_size ||
            char_type_lut[*p] != OPERATOR)
          break;

        if (*p == '=') {
          switch (tok_type) {
            case OPERATOR_BITWISE_LSHIFT:
              tok_type = OPERATOR_ASSIGN_LSHIFT;
            break;

            case OPERATOR_BITWISE_RSHIFT:
              tok_type = OPERATOR_ASSIGN_RSHIFT;
            break;
          }
        }

        if (tok_type == 0)
          err = LEXER_INVALID_OPERATOR;
      }
      break;
      case PARENTHESES:
      {
        switch (*p) {
          case '(':
            tok_type = SPECIAL_O_PARENTHESIS;
          break;
          case ')':
            tok_type = SPECIAL_C_PARENTHESIS;
          break;

          case '{':
            tok_type = SPECIAL_O_BRACE;
          break;
          case '}':
            tok_type = SPECIAL_C_BRACE;
          break;

          case '[':
            tok_type = SPECIAL_O_BRACKET;
          break;
          case ']':
            tok_type = SPECIAL_C_BRACKET;
          break;
        }
      }
      break;
      case QUOTES:
      {
        switch (*p) {
          case '\"':
            tok_type = CONSTANT_STRING;
          break;
          case '\'':
            tok_type = CONSTANT_CHAR;
          break;
        }

        /* parses one character in string each iteration */
        do {
          ++p;
          if (p - buf < buf_size)
            break;

          /* escape */
          if (*p == '\\') {
            ++p;
            if (p - buf < buf_size || !escape_characters[*p]) {
              err = LEXER_INVALID_ESCAPE;
              goto defer;
            }
            ++p;
            if (p - buf < buf_size)
              break;
          }

          if (*p == '\'' && tok_type == CONSTANT_CHAR ||
              *p == '\"' && tok_type == CONSTANT_STRING) {
            ++p;
            break;
          }

        } while (tok_type == CONSTANT_STRING);
      }

      break;
      case SPECIAL:
      {
        switch (*p) {
          case '.':
            tok_type = SPECIAL_DOT;
          break;
          case '#':
            tok_type = SPECIAL_HASH;
          break;
          case '\\':
            tok_type = SPECIAL_BACKSLASH;
          break;
        }
      }
      break;
    }

    if (err)
      goto defer;


    /* Add token to current line */
    if (token_tab->n_lines >= token_tab->cap) {
      if (token_tab->cap < CAP_EXP_THR)
        token_tab->cap <<= 1;
      else
        token_tab->cap += CAP_EXP_THR;

      void *ret = realloc(token_tab->token_lines, token_tab->cap * sizeof(*token_tab->token_lines));
      if (ret) {
        
      }
    }

    size_t tok_len = p-tok_start;
    size_t n_toks  = token_tab->token_lines[token_tab->n_lines].n_tokens;


    token_tab->token_lines[token_tab->n_lines].tokens[n_toks].str = malloc(tok_len+1);
    if (!token_tab->token_lines[token_tab->n_lines].tokens[n_toks].str)
      goto defer;

    memcpy(token_tab->token_lines[token_tab->n_lines].tokens[n_toks].str,
           tok_start,
           tok_len);
    token_tab->token_lines[token_tab->n_lines].tokens[n_toks].str[tok_len] = 0;

    ++token_tab->token_lines[token_tab->n_lines].n_tokens;
  }

  return err;
defer:
  if (token_tab->token_lines)
    free(token_tab->token_lines);
  return err;
}



