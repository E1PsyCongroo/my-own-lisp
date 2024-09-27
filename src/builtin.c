#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "local-include/common.h"
#include "local-include/lenv.h"
#include "local-include/lval.h"
#include <clisp.h>
#include <mpc.h>

lval *builtin_head(lenv *e, lval *a) {
  LASSERT_NUM("head", a, 1);
  LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("head", a, 0);

  lval *v = lval_take(a, 0);

  while (v->count > 1) {
    lval_del(lval_pop(v, 1));
  }
  return v;
}

lval *builtin_tail(lenv *e, lval *a) {
  LASSERT_NUM("tail", a, 1);
  LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("tail", a, 0);

  lval *v = lval_take(a, 0);

  lval_del(lval_pop(v, 0));
  return v;
}

lval *builtin_list(lenv *e, lval *a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval *builtin_eval(lenv *e, lval *a) {
  LASSERT_NUM("eval", a, 1);
  LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);

  lval *x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval *builtin_join(lenv *e, lval *a) {
  for (int i = 0; i < a->count; i++) {
    LASSERT_TYPE("join", a, i, LVAL_QEXPR);
  }

  lval *x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }

  lval_del(a);
  return x;
}

lval *builtin_cons(lenv *e, lval *a) {
  LASSERT_NUM("cons", a, 2);
  LASSERT_TYPE("cons", a, 1, LVAL_QEXPR);

  lval *x = lval_pop(a, 0);
  lval *v = lval_pop(a, 0);
  lval_add_front(v, x);
  lval_del(a);
  return v;
}

lval *builtin_len(lenv *e, lval *a) {
  LASSERT_NUM("len", a, 1);
  LASSERT_TYPE("len", a, 0, LVAL_QEXPR);

  lval *x = lval_take(a, 0);
  lval *y = lval_num(x->count);
  lval_del(x);
  return y;
}

lval *builtin_init(lenv *e, lval *a) {
  LASSERT_NUM("init", a, 1);
  LASSERT_TYPE("init", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("init", a, 0);

  lval *x = lval_take(a, 0);
  lval_del(lval_pop(x, x->count - 1));
  return x;
}

lval *builtin_var(lenv *e, lval *a, char *func) {
  LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

  lval *syms = a->cell[0];
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
            "Function '%s' cannot define non-symbol. "
            "Got %s, Expected %s.",
            func, ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
  }

  LASSERT(a, (syms->count == a->count - 1),
          "Function '%s' passed too many arguments for symbols. "
          "Got %i, Expected %i.",
          func, syms->count, a->count - 1);

  for (int i = 0; i < syms->count; i++) {
    if (strcmp(func, "def") == 0) {
      lenv_def(e, syms->cell[i], a->cell[i + 1]);
    }
    if (strcmp(func, "fun") == 0) {
      lenv_def(e, syms->cell[i], a->cell[i + 1]);
    }
    if (strcmp(func, "=") == 0) {
      lenv_put(e, syms->cell[i], a->cell[i + 1]);
    }
  }

  lval_del(a);
  return lval_sexpr();
}

lval *builtin_def(lenv *e, lval *a) { return builtin_var(e, a, "def"); }

lval *builtin_put(lenv *e, lval *a) { return builtin_var(e, a, "="); }

lval *builtin_lambda(lenv *e, lval *a) {
  LASSERT_NUM("\\", a, 2);
  LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
  LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

  for (int i = 0; i < a->cell[0]->count; i++) {
    LASSERT_TYPE("\\", a->cell[0], i, LVAL_SYM);
  }

  lval *formals = lval_pop(a, 0);
  lval *body = lval_pop(a, 0);
  lval_del(a);

  return lval_lambda(formals, body);
}

lval *builtin_fun(lenv *e, lval *a) {
  LASSERT_NUM("\\", a, 2);
  LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
  LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

  for (int i = 0; i < a->cell[0]->count; i++) {
    LASSERT_TYPE("\\", a->cell[0], i, LVAL_SYM);
  }

  lval *formals = lval_pop(a, 0);
  LASSERT(formals, formals->count > 1,
          "Function '%s' passed incorrect number of symbols. "
          "Got %i, Expected %s.",
          "\\", formals->count, "> 1");

  lval *func_name = lval_add(lval_qexpr(), lval_pop(formals, 0));
  lval *body = lval_pop(a, 0);
  lval *lambda = lval_lambda(formals, body);
  lval *args = lval_add(lval_add(lval_sexpr(), func_name), lambda);
  lval_del(a);

  return builtin_var(e, args, "fun");
}

lval *builtin_exit(lenv *e, lval *a) {
  if (a->count == 0) {
    exit(0);
  }

  LASSERT_NUM("exit", a, 1);
  LASSERT_TYPE("exit", a, 0, LVAL_NUM);
  exit(a->num);
  lval_del(a);
  return lval_err("exit");
}

lval *builtin_op(lenv *e, lval *a, char *op) {
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      lval_del(a);
      return lval_err("Cannot operate on non-number!");
    }
  }

  lval *x = lval_pop(a, 0);
  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;
  }

  while (a->count > 0) {
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

lval *builtin_ord(lenv *e, lval *a, char *op) {
  LASSERT_NUM(op, a, 2);
  LASSERT_TYPE(op, a, 0, LVAL_NUM);
  LASSERT_TYPE(op, a, 1, LVAL_NUM);

  int r;
  if (strcmp(op, ">") == 0) {
    r = (a->cell[0]->num > a->cell[1]->num);
  }
  if (strcmp(op, "<") == 0) {
    r = (a->cell[0]->num < a->cell[1]->num);
  }
  if (strcmp(op, ">=") == 0) {
    r = (a->cell[0]->num >= a->cell[1]->num);
  }
  if (strcmp(op, "<=") == 0) {
    r = (a->cell[0]->num <= a->cell[1]->num);
  }
  lval_del(a);
  return lval_num(r);
}

lval *builtin_gt(lenv *e, lval *a) { return builtin_ord(e, a, ">"); }
lval *builtin_lt(lenv *e, lval *a) { return builtin_ord(e, a, "<"); }
lval *builtin_ge(lenv *e, lval *a) { return builtin_ord(e, a, ">="); }
lval *builtin_le(lenv *e, lval *a) { return builtin_ord(e, a, "<="); }

lval *builtin_cmp(lenv *e, lval *a, char *op) {
  LASSERT_NUM(op, a, 2);
  int r;
  if (strcmp(op, "==") == 0) {
    r = lval_eq(a->cell[0], a->cell[1]);
  }
  if (strcmp(op, "!=") == 0) {
    r = !lval_eq(a->cell[0], a->cell[1]);
  }
  lval_del(a);
  return lval_num(r);
}
lval *builtin_eq(lenv *e, lval *a) { return builtin_cmp(e, a, "=="); }
lval *builtin_ne(lenv *e, lval *a) { return builtin_cmp(e, a, "!="); }

lval *builtin_logic(lenv *e, lval *a, char *op) {
  LASSERT_NUM(op, a, 2);
  LASSERT_TYPE(op, a, 0, LVAL_NUM);
  LASSERT_TYPE(op, a, 1, LVAL_NUM);
  int r;
  if (strcmp(op, "&&") == 0) {
    r = (a->cell[0]->num && a->cell[1]->num);
  }
  if (strcmp(op, "||") == 0) {
    r = (a->cell[0]->num || a->cell[1]->num);
  }
  lval_del(a);
  return lval_num(r);
}
lval *builtin_and(lenv *e, lval *a) { return builtin_logic(e, a, "&&"); }
lval *builtin_or(lenv *e, lval *a) { return builtin_logic(e, a, "||"); }
lval *builtin_not(lenv *e, lval *a) {
  LASSERT_NUM("not", a, 1);
  LASSERT_TYPE("not", a, 0, LVAL_NUM);
  lval *x = lval_num(!a->cell[0]->num);
  lval_del(a);
  return x;
}

lval *builtin_if(lenv *e, lval *a) {
  LASSERT_NUM("if", a, 3);
  LASSERT_TYPE("if", a, 0, LVAL_NUM);
  LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
  LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

  lval *x;
  a->cell[1]->type = LVAL_SEXPR;
  a->cell[2]->type = LVAL_SEXPR;

  if (a->cell[0]->num) {
    x = lval_eval(e, lval_pop(a, 1));
  } else {
    x = lval_eval(e, lval_pop(a, 2));
  }

  lval_del(a);
  return x;
}

lval *builtin_load(lenv *e, lval *a) {
  LASSERT_NUM("load", a, 1);
  LASSERT_TYPE("load", a, 0, LVAL_STR);

  mpc_result_t r;
  if (mpc_parse_contents(a->cell[0]->str, Lispy, &r)) {

    lval *expr = lval_read(r.output);
    mpc_ast_delete(r.output);

    while (expr->count) {
      lval *x = lval_eval(e, lval_pop(expr, 0));
      /* If Evaluation leads to error print it */
      if (x->type == LVAL_ERR) {
        lval_println(x);
      }
      lval_del(x);
    }

    lval_del(expr);
    lval_del(a);

    return lval_sexpr();

  } else {
    char *err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    lval *err = lval_err("Could not load Library %s", err_msg);
    free(err_msg);
    lval_del(a);

    return err;
  }
}

lval *builtin_print(lenv *e, lval *a) {
  for (int i = 0; i < a->count; i++) {
    lval_print(a->cell[i]);
    putchar(' ');
  }

  putchar('\n');
  lval_del(a);

  return lval_sexpr();
}

lval *builtin_error(lenv *e, lval *a) {
  LASSERT_NUM("error", a, 1);
  LASSERT_TYPE("error", a, 0, LVAL_STR);

  lval *err = lval_err(a->cell[0]->str);

  lval_del(a);
  return err;
}
