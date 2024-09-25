#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "local-include/lenv.h"
#include "local-include/lval.h"
#include <clisp.h>
#include <mpc.h>

char *ltype_name(int t) {
  switch (t) {
  case LVAL_FUN:
    return "Function";
  case LVAL_NUM:
    return "Number";
  case LVAL_ERR:
    return "Error";
  case LVAL_SYM:
    return "Symbol";
  case LVAL_SEXPR:
    return "S-Expression";
  case LVAL_QEXPR:
    return "Q-Expression";
  default:
    return "Unknown";
  }
}

void lval_expr_print(lenv *e, lval *v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {
    /* Print Value contained within */
    lval_print(e, v->cell[i]);
    /* Don't print trailing space if last element */
    if (i != (v->count - 1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_print(lenv *e, lval *v) {
  switch (v->type) {
  case LVAL_NUM:
    printf("%li", v->num);
    break;
  case LVAL_ERR:
    printf("Error: %s", v->err);
    break;
  case LVAL_SYM:
    printf("%s", v->sym);
    break;
  case LVAL_SEXPR:
    lval_expr_print(e, v, '(', ')');
    break;
  case LVAL_QEXPR:
    lval_expr_print(e, v, '{', '}');
    break;
  case LVAL_FUN:
    if (v->builtin) {
      printf("<builtin-%s>", lenv_get_builtin_name(e, v));
    } else {
      printf("(\\ ");
      lval_print(e, v->formals);
      putchar(' ');
      lval_print(e, v->body);
      putchar(')');
    }
    break;
  }
}

void lval_println(lenv *e, lval *v) {
  lval_print(e, v);
  putchar('\n');
}

void lenv_print(lenv *e) {
  while (e) {
    for (int i = 0; i < e->count; i++) {
      printf("%s: ", e->syms[i]);
      lval_println(e, e->vals[i]);
    }
    e = e->par;
  }
}
