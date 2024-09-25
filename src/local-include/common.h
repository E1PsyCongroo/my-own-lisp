#ifndef __COMMON_H__
#define __COMMON_H__

/* Forward Declarations */
typedef struct lval lval;
typedef struct lenv lenv;

/* Create Enumeration of Possible lval Types */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Declare New lbuiltin Type */
typedef lval *(*lbuiltin)(lenv *, lval *);

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
    struct {
      lbuiltin builtin;
      lenv *env;
      lval *formals;
      lval *body;
    };
  };
} lval;

/* Declare New lenv Struct */
struct lenv {
  lenv *par;
  int count;
  char **syms;
  lval **vals;
};

#endif
