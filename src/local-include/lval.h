#ifndef __LVAL_H__
#define __LVAL_H__

#include "common.h"
#include <mpc.h>

#define LASSERT(args, cond, fmt, ...)                                          \
  do {                                                                         \
    if (!(cond)) {                                                             \
      lval *err = lval_err(fmt, ##__VA_ARGS__);                                \
      lval_del(args);                                                          \
      return err;                                                              \
    }                                                                          \
  } while (0)

lval *lval_num(long x);
lval *lval_err(char *fmt, ...);
lval *lval_sym(char *s);
lval *lval_sexpr(void);
lval *lval_qexpr(void);
lval *lval_fun(lbuiltin func);
lval *lval_read_num(mpc_ast_t *t);
lval *lval_add(lval *v, lval *x);
lval *lval_add_front(lval *v, lval *x);
lval *lval_copy(lval *v);

lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);
lval *lval_eval_sexpr(lenv *e, lval *v);

lval *builtin_head(lenv *e, lval *a);
lval *builtin_tail(lenv *e, lval *a);
lval *builtin_list(lenv *e, lval *a);
lval *builtin_eval(lenv *e, lval *a);
lval *lval_join(lval *x, lval *y);
lval *builtin_join(lenv *e, lval *a);
lval *builtin_cons(lenv *e, lval *a);
lval *builtin_len(lenv *e, lval *a);
lval *builtin_init(lenv *e, lval *a);
lval *builtin_op(lenv *e, lval *a, char *op);
lval *builtin_add(lenv *e, lval *a);
lval *builtin_sub(lenv *e, lval *a);
lval *builtin_mul(lenv *e, lval *a);
lval *builtin_div(lenv *e, lval *a);
lval *builtin_def(lenv *e, lval *a);
lval *builtin_exit(lenv *e, lval *a);

char *ltype_name(int t);
void lval_expr_print(lenv *e, lval *v, char open, char close);

#endif
