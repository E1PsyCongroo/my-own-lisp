#ifndef __LENV_H__
#define __LENV_H__

#include "common.h"
#include <mpc.h>

char *lenv_get_fun_name(lenv *e, lval *k);
lval *lenv_get(lenv *e, lval *k);
void lenv_put(lenv *e, lval *k, lval *v);
void lenv_add_builtin(lenv *e, char *name, lbuiltin func);

#endif
