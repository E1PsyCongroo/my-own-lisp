#ifndef __CLISP_H__
#define __CLISP_H__

#include <mpc.h>

typedef struct lenv lenv;
typedef struct lval lval;

lenv *lenv_new(void);
void lenv_add_builtins(lenv *e);
void lenv_del(lenv *e);

lval *lval_read(mpc_ast_t *t);
void lval_del(lval *v);

lval *lval_eval(lenv *e, lval *v);

void lenv_print(lenv *e);
void lval_print(lenv *e, lval *v);
void lval_println(lenv *e, lval *v);

#endif
