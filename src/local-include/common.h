/*
 * common.h - 本地环境头文件
 * 此头文件应仅在特定实现中包含，不应对调用者公开。
 * 包含了实现所需的常量定义和类型声明。
 */
#ifndef __COMMON_H__
#define __COMMON_H__

/*
 * 前向声明
 * 如果你不清楚前向声明的作用，STFW & RTFM
 */
typedef struct lval lval;
typedef struct lenv lenv;

/*
 * 定义 lisp 值的可能类型的枚举常量。
 * LVAL_ERR: 错误类型。
 * LVAL_NUM: 数值类型。
 * LVAL_SYM: 符号类型。
 * LVAL_FUN: 函数类型。
 * LVAL_SEXPR: S表达式类型。
 * LVAL_QEXPR: Q表达式类型。
 */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

/*
 * 声明 lbuiltin 类型，指向 lisp 内置函数的函数指针。
 * 如果你不熟悉 typedef 或函数指针，STFW & RTFM
 */
typedef lval *(*lbuiltin)(lenv *, lval *);

/*
 * 声明 lval 结构体，表示 lisp 值。
 * 如果你不熟悉（匿名）结构体和联合体的用法，STFW &RTFM
 * 该结构体根据不同类型的值使用不同的存储方式。
 * - type == LVAL_ERR: 使用 err 存储错误信息。
 * - type == LVAL_NUM: 使用 num 存储数值。
 * - type == LVAL_SYM: 使用 sym 存储符号。
 * - type == LVAL_SEXPR 或 LVAL_QEXPR: 使用 cell 数组存储表达式。
 * - type == LVAL_FUN:
 *   - 如果 builtin 不为 NULL，表示为内置函数。
 *   - 如果 builtin 为 NULL，表示为用户定义的 lambda 函数，
 *     其中 env 为函数环境，formals 为参数列表，body 为函数体。
 */
typedef struct lval {
  int type;
  union {
    long num;
    char *err;
    char *sym;
    struct {
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

/*
 * 定义 lenv 结构体，表示 lisp 环境。
 * par 是父环境，用于实现环境的嵌套。
 * syms 和 vals 分别存储环境中定义的符号及其对应的 lval 值。
 */
struct lenv {
  lenv *par;
  int count;
  char **syms;
  lval **vals;
};

#endif
