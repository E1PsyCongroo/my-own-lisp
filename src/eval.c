#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "local-include/lenv.h"
#include "local-include/lval.h"
#include <clisp.h>
#include <mpc.h>

lval *lval_pop(lval *v, int i) {
  lval *x = v->cell[i];

  memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval *) * (v->count - i - 1));

  v->count--;
  v->cell = realloc(v->cell, sizeof(lval *) * v->count);
  return x;
}

lval *lval_take(lval *v, int i) {
  lval *x = lval_pop(v, i);
  lval_del(v);
  return x;
}

lval *lval_call(lenv *e, lval *f, lval *a) {
  /* If Builtin then simply apply that */
  if (f->builtin) {
    return f->builtin(e, a);
  }

  /* Record Argument Counts */
  int given = a->count;
  int total = f->formals->count;
  /* While arguments still remain to be processed */
  while (a->count) {
    /* If we've ran out of formal arguments to bind */
    if (f->formals->count == 0) {
      lval_del(a);
      return lval_err("Function passed too many arguments. "
                      "Got %i, Expected %i.",
                      given, total);
    }
    /* Pop the first symbol from the formals */
    lval *sym = lval_pop(f->formals, 0);
    /* Special Case to deal with '&' */
    if (strcmp(sym->sym, "&") == 0) {
      /* Ensure '&' is followed by another symbol */
      if (f->formals->count != 1) {
        lval_del(a);
        return lval_err("Function format invalid. "
                        "Symbol '&' not followed by single symbol.");
      }
      /* Next formal should be bound to remaining arguments */
      lval *nsym = lval_pop(f->formals, 0);
      lenv_put(f->env, nsym, builtin_list(e, a));
      lval_del(sym);
      lval_del(nsym);
      break;
    }
    /* Pop the next argument from the list */
    lval *val = lval_pop(a, 0);
    /* Bind a copy into the function's environment */
    lenv_put(f->env, sym, val);
    /* Delete symbol and value */
    lval_del(sym);
    lval_del(val);
  }
  /* Argument list is now bound so can be cleaned up */
  lval_del(a);
  /* If '&' remains in formal list bind to empty list */
  if (f->formals->count > 0 && strcmp(f->formals->cell[0]->sym, "&") == 0) {
    /* Check to ensure that & is not passed invalidly. */
    if (f->formals->count != 2) {
      return lval_err("Function format invalid. "
                      "Symbol '&' not followed by single symbol.");
    }
    /* Pop and delete '&' symbol */
    lval_del(lval_pop(f->formals, 0));
    /* Pop next symbol and create empty list */
    lval *sym = lval_pop(f->formals, 0);
    lval *val = lval_qexpr();
    /* Bind to environment and delete */
    lenv_put(f->env, sym, val);
    lval_del(sym);
    lval_del(val);
  }
  /* If all formals have been bound evaluate */
  if (f->formals->count == 0) {
    /* Set environment parent to evaluation environment */
    f->env->par = e;
    /* Evaluate and return */
    return builtin_eval(f->env, lval_add(lval_sexpr(), lval_copy(f->body)));
  } else {
    /* Otherwise return partially evaluated function */
    return lval_copy(f);
  }
}

lval *lval_eval_sexpr(lenv *e, lval *v) {
  /* Evaluate Children */
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(e, v->cell[i]);
  }
  /* Error Checking */
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) {
      return lval_take(v, i);
    }
  }
  /* Empty Expression */
  if (v->count == 0) {
    return v;
  }
  /* Single Expression */
  if (v->count == 1) {
    lval *f = lval_pop(v, 0);
    if (f->type == LVAL_FUN) {
      lval *result = lval_call(e, f, v);
      lval_del(f);
      return result;
    } else {
      lval_del(v);
      return f;
    }
  }
  /* Ensure First Element is Symbol */
  lval *f = lval_pop(v, 0);
  if (f->type != LVAL_FUN) {
    lval *err = lval_err("S-Expression starts with incorrect type. "
                         "Got %s, Expected %s.",
                         ltype_name(f->type), ltype_name(LVAL_FUN));
    lval_del(f);
    lval_del(v);
    return err;
  }
  /* Call builtin with operator */
  lval *result = lval_call(e, f, v);
  lval_del(f);
  return result;
}

lval *lval_eval(lenv *e, lval *v) {
  if (v->type == LVAL_SYM) {
    lval *x = lenv_get(e, v);
    lval_del(v);
    return x;
  }
  if (v->type == LVAL_SEXPR) {
    return lval_eval_sexpr(e, v);
  }
  return v;
}
