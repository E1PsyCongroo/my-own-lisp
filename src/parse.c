#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "local-include/lenv.h"
#include "local-include/lval.h"
#include <clisp.h>
#include <mpc.h>

mpc_parser_t *Number = NULL;
mpc_parser_t *Symbol = NULL;
mpc_parser_t *String = NULL;
mpc_parser_t *Comment = NULL;
mpc_parser_t *Sexpr = NULL;
mpc_parser_t *Qexpr = NULL;
mpc_parser_t *Expr = NULL;
mpc_parser_t *Lispy = NULL;

void parser_init(void) {
  Number = mpc_new("number");
  Symbol = mpc_new("symbol");
  String = mpc_new("string");
  Comment = mpc_new("comment");
  Sexpr = mpc_new("sexpr");
  Qexpr = mpc_new("qexpr");
  Expr = mpc_new("expr");
  Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                     \
              number  : /-?[0-9]+/ ;                              \
              symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&|]+/ ;       \
              string  : /\"(\\\\.|[^\"])*\"/ ;                    \
              comment : /;[^\\r\\n]*/ ;                           \
              sexpr   : '(' <expr>* ')' ;                         \
              qexpr   : '{' <expr>* '}' ;                         \
              expr    : <number> | <symbol> | <string> |          \
                        <sexpr> | <qexpr> ;                       \
              lispy   : /^/ (<sexpr> | <comment>)* /$/ ;          \
    ",
            Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);
}

int parse_line(const char *line, mpc_result_t *r) {
  return mpc_parse("<stdin>", line, Lispy, r);
}

void parse_print(mpc_result_t *r) { mpc_ast_print(r->output); }

void parse_delete(mpc_result_t *r) { mpc_ast_delete(r->output); }

void parse_error(mpc_result_t *r) {
  mpc_err_print(r->error);
  mpc_err_delete(r->error);
}

void parse_args(int argc, char **argv, lenv *e) {
  if (argc >= 2) {
    for (int i = 1; i < argc; i++) {
      lval *args = lval_add(lval_sexpr(), lval_str(argv[i]));
      lval *x = builtin_load(e, args);
      if (x->type == LVAL_ERR) {
        lval_println(x);
      }
      lval_del(x);
    }
  }
}

void parser_quit(void) {
  mpc_cleanup(8, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);
}
