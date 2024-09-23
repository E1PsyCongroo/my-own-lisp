#ifndef __CLISP_H__
#define __CLISP_H__

#include <mpc.h>

typedef struct lval lval;

lval *lval_read(mpc_ast_t *t);
void lval_del(lval *v);

lval *lval_eval(lval *v);

void lval_print(lval *v);
void lval_println(lval *v);

#endif
