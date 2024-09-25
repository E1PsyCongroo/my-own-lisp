#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clisp.h>
#include <editline/history.h>
#include <editline/readline.h>
#include <mpc.h>

int main(int argc, char **argv) {

  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Symbol = mpc_new("symbol");
  mpc_parser_t *Sexpr = mpc_new("sexpr");
  mpc_parser_t *Qexpr = mpc_new("qexpr");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                     \
              number : /-?[0-9]+/ ;                               \
              symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
              sexpr  : '(' <expr>* ')' ;                          \
              qexpr  : '{' <expr>* '}' ;                          \
              expr   : <number> | <symbol> | <sexpr> | <qexpr> ;  \
              lispy  : /^/ <sexpr>* /$/ ;                         \
    ",
            Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

  signal(SIGINT, SIG_IGN);

  puts("Lispy Version 0.0.0.0.6");
  puts("Press Ctrl+d to Exit\n");

  lenv *e = lenv_new();
  lenv_add_builtins(e);
  char *input = NULL;
  while ((input = readline("lispy> "))) {
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      mpc_ast_print(r.output);
      lval *x = lval_eval(e, lval_read(r.output));
      lenv_print(e);
      lval_println(e, x);
      lval_del(x);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }
  lenv_del(e);
  putchar('\n');
  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
  return 0;
}
