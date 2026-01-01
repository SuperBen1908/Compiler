
#include <stdlib.h>
#include <string.h>
#include "../inc/lexer.h"

#define CAP 10

lexer_err_e lexer_parse(const char  *buf,
                        size_t       buf_size,
                        token_tab_t *token_tab)
{
  lexer_err_e err = 0; 

  typedef enum char_type_e {
    LETTER = 1,
    NUMBER,
    OPERATOR,
    PARENTHESES,
    QUOTES,
    PP,
    BS,
    SPACE,
  } char_type_e;
  char_type_e char_type_lut[256] = {0};

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
                                     
  char_type_lut['#']  = PP;          char_type_lut['\\'] = BS;
                                     
  char_type_lut[' ']  = SPACE;       char_type_lut['\t'] = SPACE;
  char_type_lut['\n'] = SPACE;       char_type_lut['\r'] = SPACE;

  token_tab->cap = CAP;
  token_tab->n_lines = 0;
  token_tab->token_lines = malloc(token_tab->cap * sizeof(*token_tab->token_lines));
  if (!token_tab->token_lines)
    goto defer;

  const char *p = buf;
  while (p - buf < buf_size) {
    while (p - buf < buf_size && char_type_lut[*p] == SPACE)
      ++p;

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
        tok_type = CONSTANT_DEC;

        if (*p != '0') {
          /* Decimal */
          while (p - buf < buf_size && char_type_lut[*p] == NUMBER)
            ++p;
        }
        else {
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
              if (*p == '.') {
                tok_type = CONSTANT_FLOAT;

                /* Float */
                while (p - buf < buf_size &&
                      (char_type_lut[*p] == NUMBER))
                  ++p;
              }
              else {
                if (char_type_lut[*p] != NUMBER || *p == 0 || *p > '7')
                  break;

                /* Accepts only 0-7 after 0%d */
                tok_type = CONSTANT_OCT;

                while (p - buf < buf_size &&
                      (*p >= '0' && *p <= '0'))
                  ++p;
              }
            }
            break;
          }

          if (p - buf < buf_size)
            break;
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
            tok_type = OPERATOR_BITWISE_NOT;
            end = 1;
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

          case ':':
            tok_type = OPERATOR_TRINARY;
            end = 1;
          break;
          case '?':
            tok_type = OPERATOR_QUESTION_MARK;
            end = 1;
          break;
        }

        if (end)
          break;

        char prev_p = *p;
        ++p;
        if (p - buf < buf_size)
          break;

        /* Second operator */
        /* double operation */
        switch (tok_type) {
          case OPERATOR_PLUS:
          case OPERATOR_MINUS:
          case OPERATOR_ASSIGN:
          case OPERATOR_LESS:
          case OPERATOR_GREATER:
          case OPERATOR_BITWISE_AND:
          case OPERATOR_BITWISE_OR:
          {
            if (prev_p == *p) {
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
                tok_type += 3; /* to bitshift */
                break;
              }

              if ((tok_type & OPERATOR_BITWISE) == OPERATOR_BITWISE) {

                tok_type ^= OPERATOR_BITWISE;
                tok_type ^= OPERATOR_LOGICAL;
                break;
              }
            }
          }
          break;
        }

        /* operator assignment */
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
              tok_type += 1;
            }
            break;

            case OPERATOR_LESS:
            case OPERATOR_GREATER:
            {
              tok_type += 2;
            }
            break;

            case OPERATOR_LOGICAL_NOT:
            {
              tok_type = OPERATOR_NOT_EQ;
            }
            break;

            case OPERATOR_BITWISE_AND:
            case OPERATOR_BITWISE_OR:
            case OPERATOR_BITWISE_XOR:
            {
              tok_type ^= OPERATOR_BITWISE;
              tok_type ^= OPERATOR_ASSIGN_BITWISE;
              tok_type -= 1;
            }
            break;
          }
        }

        switch (tok_type) {
          case OPERATOR_PLUS:
          break;
          case OPERATOR_MINUS:
          break;
          case OPERATOR_MUL:
          break;
          case OPERATOR_DIV:
          break;
          case OPERATOR_MOD:
          break;

          case OPERATOR_LOGICAL_NOT:
          break;
          case OPERATOR_BITWISE_NOT:
          break;
          case OPERATOR_BITWISE_AND:
          break;
          case OPERATOR_BITWISE_OR:
          break;
          case OPERATOR_BITWISE_XOR:
          break;

          case OPERATOR_ASSIGN:
          break;
          case OPERATOR_LESS:
          break;
          case OPERATOR_GREATER:
          break;

          default:
          break;
        }
      }
      break;
      case PARENTHESES:
      break;
      case QUOTES:
      break;
      case PP:
      break;
      case BS:
      break;
      default:
      break;
    }

    size_t tok_len = p-tok_start;
    size_t n_toks  = token_tab->token_lines[token_tab->n_lines].n_tokens;

    token_tab->token_lines[token_tab->n_lines].tokens[n_toks].str = malloc(tok_len+1);
    if (!token_tab->token_lines[token_tab->n_lines].tokens[n_toks].str)
      goto defer;

    memcpy(token_tab->token_lines[token_tab->n_lines].tokens[n_toks].str,
           tok_start,
           tok_len);
    ++token_tab->token_lines[token_tab->n_lines].n_tokens;
  }

  return err;
defer:
  if (token_tab->token_lines)
    free(token_tab->token_lines);
  return err;
}



