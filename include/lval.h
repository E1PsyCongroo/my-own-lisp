#ifndef __LVAL_H
#define __LVAL_H

#include <mpc.h>
/* Create Enumeration of Possible lval Types */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR };

/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Declare New lval Struct */
typedef struct lval {
  int type;
  long num;
  /* Error and Symbol types have some string data */
  char *err;
  char *sym;
  /* Count and Pointer to a list of "lval*" */
  int count;
  struct lval **cell;
} lval;

/* Construct a pointer to a new Number lval */
lval *lval_num(long x);
/* Construct a pointer to a new Error lval */
lval *lval_err(char *m);
/* Construct a pointer to a new Symbol lval */
lval *lval_sym(char *s);
/* A pointer to a new empty Sexpr lval */
lval *lval_sexpr(void);
void lval_del(lval *v);
lval *lval_read_num(mpc_ast_t *t);
lval *lval_read(mpc_ast_t *t);
lval *lval_add(lval *v, lval *x);
void lval_expr_print(lval *v, char open, char close);
void lval_print(lval *v);
void lval_println(lval *v);
lval *lval_eval_sexpr(lval *v);
lval *lval_eval(lval *v);
lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);
lval *builtin_op(lval *a, char *op);

#endif
