#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "local-include/common.h"
#include "local-include/lenv.h"
#include "local-include/lval.h"
#include <clisp.h>
#include <mpc.h>

lval *builtin_head(lenv *e, lval *a) {
  /* Check Error Conditions */
  LASSERT(a, a->count == 1, "Function 'head' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'head' passed incorrect type for argument 0. "
          "Got %s, Expected %s.",
          ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  LASSERT(a, a->cell[0]->count != 0, "Function 'head' passed {}!");

  /* Otherwise take first argument */
  lval *v = lval_take(a, 0);

  /* Delete all elements that are not head and return */
  while (v->count > 1) {
    lval_del(lval_pop(v, 1));
  }
  return v;
}

lval *builtin_tail(lenv *e, lval *a) {
  /* Check Error Conditions */
  LASSERT(a, a->count == 1, "Function 'tail' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'tail' passed incorrect type for argument 0. "
          "Got %s, Expected %s.",
          ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  LASSERT(a, a->cell[0]->count != 0, "Function 'tail' passed {}!");

  /* Take first argument */
  lval *v = lval_take(a, 0);

  /* Delete first element and return */
  lval_del(lval_pop(v, 0));
  return v;
}

lval *builtin_list(lenv *e, lval *a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval *builtin_eval(lenv *e, lval *a) {
  LASSERT(a, a->count == 1, "Function 'eval' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'eval' passed incorrect type for argument 0. "
          "Got %s, Expected %s.",
          ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));

  lval *x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval *lval_join(lval *x, lval *y) {
  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}

lval *builtin_join(lenv *e, lval *a) {
  for (int i = 0; i < a->count; i++) {
    LASSERT(a, a->cell[i]->type == LVAL_QEXPR,
            "Function 'join' passed incorrect type for argument %i. "
            "Got %s, Expected %s.",
            i, ltype_name(a->cell[i]->type), ltype_name(LVAL_QEXPR));
  }

  lval *x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }

  lval_del(a);
  return x;
}

lval *builtin_cons(lenv *e, lval *a) {
  LASSERT(a, a->count == 2, "Function 'cons' passed too many arguments!");
  LASSERT(a, a->cell[1]->type == LVAL_QEXPR,
          "Function 'cons' passed incorrect type for argument 1. "
          "Got %s, Expected %s.",
          ltype_name(a->cell[1]->type), ltype_name(LVAL_QEXPR));

  lval *x = lval_pop(a, 0);
  lval *v = lval_pop(a, 0);
  lval_add_front(v, x);
  lval_del(a);
  return v;
}

lval *builtin_len(lenv *e, lval *a) {
  LASSERT(a, a->count == 1, "Function 'len' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'len' passed incorrect type for argument 0. "
          "Got %s, Expected %s.",
          ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));

  lval *x = lval_take(a, 0);
  lval *y = lval_num(x->count);
  lval_del(x);
  return y;
}

lval *builtin_init(lenv *e, lval *a) {
  LASSERT(a, a->count == 1, "Function 'init' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'init' passed incorrect type for argument 0. "
          "Got %s, Expected %s.",
          ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  LASSERT(a, a->cell[0]->count != 0, "Function 'init' passed {}!");

  lval *x = lval_take(a, 0);
  lval_del(lval_pop(x, x->count - 1));
  return x;
}

lval *builtin_op(lenv *e, lval *a, char *op) {
  /* Ensure all arguments are numbers */
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      lval_del(a);
      return lval_err("Cannot operate on non-number!");
    }
  }

  /* Pop the first element */
  lval *x = lval_pop(a, 0);
  /* If no arguments and sub then perform unary negation */
  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;
  }

  /* While there are still elements remaining */
  while (a->count > 0) {
    /* Pop the next element */
    lval *y = lval_pop(a, 0);
    if (strcmp(op, "+") == 0) {
      x->num += y->num;
    }
    if (strcmp(op, "-") == 0) {
      x->num -= y->num;
    }
    if (strcmp(op, "*") == 0) {
      x->num *= y->num;
    }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x);
        lval_del(y);
        x = lval_err("Division By Zero!");
        break;
      }
      x->num /= y->num;
    }
    lval_del(y);
  }
  lval_del(a);
  return x;
}

lval *builtin_add(lenv *e, lval *a) { return builtin_op(e, a, "+"); }

lval *builtin_sub(lenv *e, lval *a) { return builtin_op(e, a, "-"); }

lval *builtin_mul(lenv *e, lval *a) { return builtin_op(e, a, "*"); }

lval *builtin_div(lenv *e, lval *a) { return builtin_op(e, a, "/"); }

lval *builtin_def(lenv *e, lval *a) {
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'def' passed incorrect type for argument 0. "
          "Got %s, Expected %s.",
          ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
  /* First argument is symbol list */
  lval *syms = a->cell[0];

  /* Ensure all elements of first list are symbols */
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, syms->cell[i]->type == LVAL_SYM,
            "Function 'def' passed incorrect type for argument 1. "
            "Got %s, Expected %s.",
            ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
  }

  /* Check correct number of symbols and values */
  LASSERT(a, syms->count == a->count - 1,
          "Function 'def' cannot define incorrect "
          "number of values to symbols");
  /* Assign copies of values to symbols */
  for (int i = 0; i < syms->count; i++) {
    lenv_put(e, syms->cell[i], a->cell[i + 1]);
  }

  lval_del(a);
  return lval_sexpr();
}

lval *builtin_exit(lenv *e, lval *a) {
  if (a->count == 0) {
    exit(0);
  }

  LASSERT(a, a->count = 1, "Function 'exit' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_NUM,
          "Function 'exit' passed incorrect type for argument 0. "
          "Got %s, Expected %s.",
          ltype_name(a->cell[0]->type), ltype_name(LVAL_NUM));
  exit(a->num);
  lval_del(a);
  return lval_err("exit");
}
