#ifndef __LVAL_H__
#define __LVAL_H__

#include <mpc.h>

/* Create Enumeration of Possible lval Types */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Declare New lval Struct */
typedef struct lval {
  int type;
  union {
    long num;
    /* Error and Symbol types have some string data */
    char *err;
    char *sym;
    struct {
      /* Count and Pointer to a list of "lval*" */
      int count;
      struct lval **cell;
    };
  };
} lval;

#define LASSERT(args, cond, err)                                               \
  do                                                                           \
    if (!(cond)) {                                                             \
      lval_del(args);                                                          \
      return lval_err(err);                                                    \
    }                                                                          \
  while (0)

lval *lval_num(long x);
lval *lval_err(char *m);
lval *lval_sym(char *s);
lval *lval_sexpr(void);
lval *lval_qexpr(void);
lval *lval_read_num(mpc_ast_t *t);
lval *lval_add(lval *v, lval *x);

lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);
lval *lval_eval_sexpr(lval *v);

lval *builtin_head(lval *a);
lval *builtin_tail(lval *a);
lval *builtin_list(lval *a);
lval *builtin_eval(lval *a);
lval *lval_join(lval *x, lval *y);
lval *builtin_join(lval *a);
lval *builtin_op(lval *a, char *op);
lval *builtin(lval *a, char *func);

void lval_expr_print(lval *v, char open, char close);

#endif
