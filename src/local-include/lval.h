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

#define LASSERT_NUM(func, args, num)                                           \
  LASSERT(args, args->count == num,                                            \
          "Function '%s' passed incorrect number of arguments. "               \
          "Got %i, Expected %i.",                                              \
          func, args->count, num)

#define LASSERT_TYPE(func, args, index, expect)                                \
  LASSERT(args, args->cell[index]->type == expect,                             \
          "Function '%s' passed incorrect type for argument %i. "              \
          "Got %s, Expected %s.",                                              \
          func, index, ltype_name(args->cell[index]->type),                    \
          ltype_name(expect))

#define LASSERT_NOT_EMPTY(func, args, index)                                   \
  LASSERT(args, args->cell[index]->count != 0,                                 \
          "Function '%s' passed {} for argument %i.", func, index);

lval *lval_num(long x);
lval *lval_err(char *fmt, ...);
lval *lval_sym(char *s);
lval *lval_sexpr(void);
lval *lval_qexpr(void);
lval *lval_fun(lbuiltin func);
lval *lval_lambda(lval *formals, lval *body);
lval *lval_join(lval *x, lval *y);
lval *lval_copy(lval *v);
int lval_eq(lval *x, lval *y);
lval *lval_read_num(mpc_ast_t *t);
lval *lval_add(lval *v, lval *x);
lval *lval_add_front(lval *v, lval *x);

lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);
lval *lval_call(lenv *e, lval *f, lval *a);
lval *lval_eval_sexpr(lenv *e, lval *v);

lval *builtin_head(lenv *e, lval *a);
lval *builtin_tail(lenv *e, lval *a);
lval *builtin_list(lenv *e, lval *a);
lval *builtin_eval(lenv *e, lval *a);
lval *builtin_join(lenv *e, lval *a);
lval *builtin_cons(lenv *e, lval *a);
lval *builtin_len(lenv *e, lval *a);
lval *builtin_init(lenv *e, lval *a);
lval *builtin_var(lenv *e, lval *a, char *func);
lval *builtin_def(lenv *e, lval *a);
lval *builtin_put(lenv *e, lval *a);
lval *builtin_lambda(lenv *e, lval *a);
lval *builtin_fun(lenv *e, lval *a);
lval *builtin_exit(lenv *e, lval *a);

lval *builtin_op(lenv *e, lval *a, char *op);
lval *builtin_add(lenv *e, lval *a);
lval *builtin_sub(lenv *e, lval *a);
lval *builtin_mul(lenv *e, lval *a);
lval *builtin_div(lenv *e, lval *a);

lval *builtin_ord(lenv *e, lval *a, char *op);
lval *builtin_gt(lenv *e, lval *a);
lval *builtin_lt(lenv *e, lval *a);
lval *builtin_ge(lenv *e, lval *a);
lval *builtin_le(lenv *e, lval *a);
lval *builtin_cmp(lenv *e, lval *a, char *op);
lval *builtin_eq(lenv *e, lval *a);
lval *builtin_ne(lenv *e, lval *a);
lval *builtin_if(lenv *e, lval *a);

char *ltype_name(int t);
void lval_expr_print(lenv *e, lval *v, char open, char close);

#endif
