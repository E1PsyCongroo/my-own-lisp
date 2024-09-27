#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "local-include/common.h"
#include "local-include/lenv.h"
#include "local-include/lval.h"
#include <clisp.h>
#include <mpc.h>

lenv *lenv_new(void) {
  lenv *e = malloc(sizeof(lenv));
  e->par = NULL;
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}

void lenv_del(lenv *e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}

lval *lenv_get(lenv *e, lval *k) {
  for (int i = 0; i < e->count; i++) {
    if (strcmp(e->syms[i], k->sym) == 0) {
      return lval_copy(e->vals[i]);
    }
  }

  if (e->par) {
    return lenv_get(e->par, k);
  } else {
    return lval_err("Unbound Symbol '%s'", k->sym);
  }
}

void lenv_put(lenv *e, lval *k, lval *v) {
  for (int i = 0; i < e->count; i++) {
    if (strcmp(e->syms[i], k->sym) == 0) {
      lval_del(e->vals[i]);
      e->vals[i] = lval_copy(v);
      return;
    }
  }

  e->count++;
  e->vals = realloc(e->vals, sizeof(lval *) * e->count);
  e->syms = realloc(e->syms, sizeof(char *) * e->count);

  e->vals[e->count - 1] = lval_copy(v);
  e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
  strcpy(e->syms[e->count - 1], k->sym);
}

void lenv_def(lenv *e, lval *k, lval *v) {
  while (e->par) {
    e = e->par;
  }
  lenv_put(e, k, v);
}

lenv *lenv_copy(lenv *e) {
  lenv *n = malloc(sizeof(lenv));
  n->par = e->par;
  n->count = e->count;
  n->syms = malloc(sizeof(char *) * n->count);
  n->vals = malloc(sizeof(lval *) * n->count);
  for (int i = 0; i < e->count; i++) {
    n->syms[i] = malloc(strlen(e->syms[i]) + 1);
    strcpy(n->syms[i], e->syms[i]);
    n->vals[i] = lval_copy(e->vals[i]);
  }
  return n;
}

void lenv_add_builtin(lenv *e, char *name, lbuiltin func) {
  lval *k = lval_sym(name);
  lval *v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k);
  lval_del(v);
}

void lenv_add_builtins(lenv *e) {
  const struct {
    char *name;
    lbuiltin func;
  } builtins[] = {
      /* List Functions */
      {"list", builtin_list},
      {"head", builtin_head},
      {"tail", builtin_tail},
      {"eval", builtin_eval},
      {"join", builtin_join},
      {"cons", builtin_cons},
      {"len", builtin_len},
      {"init", builtin_init},
      {"def", builtin_def},
      {"=", builtin_put},
      {"\\", builtin_lambda},
      {"fun", builtin_fun},
      {"exit", builtin_exit},
      {"load", builtin_load},
      {"print", builtin_print},
      {"error", builtin_error},
      /* Mathematical Functions */
      {"+", builtin_add},
      {"-", builtin_sub},
      {"*", builtin_mul},
      {"/", builtin_div},
      /* Comparison Functions */
      {"if", builtin_if},
      {"==", builtin_eq},
      {"!=", builtin_ne},
      {">", builtin_gt},
      {"<", builtin_lt},
      {">=", builtin_ge},
      {"<=", builtin_le},
      {"and", builtin_and},
      {"&&", builtin_and},
      {"or", builtin_or},
      {"||", builtin_or},
      {"not", builtin_not},
      {"!", builtin_not},
  };
  const int builtin_count = (sizeof builtins) / (sizeof builtins[0]);
  for (int i = 0; i < builtin_count; i++) {
    lenv_add_builtin(e, builtins[i].name, builtins[i].func);
  }
}
