/*
 * clisp.h 提供 API 接口给调用者使用。
 */
#ifndef __CLISP_H__
#define __CLISP_H__

#include <mpc.h>

/*
 * 对 lenv 和 lval 结构进行前向声明。
 * 这里不展示 lenv 和 lval 的具体结构细节，以实现抽象化。
 */
typedef struct lenv lenv;
typedef struct lval lval;

void parser_init(void);
int parse_line(const char *line, mpc_result_t *r);
void parse_print(mpc_result_t *r);
void parse_delete(mpc_result_t *r);
void parse_error(mpc_result_t *r);
void parse_args(int argc, char **argv, lenv *e);
void parser_quit(void);

/*
 * 创建一个新的 lisp 环境。
 * "调用方"负责使用 lenv_del 函数释放返回的环境。
 */
lenv *lenv_new(void);
/*
 * 在环境中添加内置函数。
 * 主要用于初始化环境。
 */
void lenv_add_builtins(lenv *e);
/*
 * 释放 lisp 环境 `e`。
 * "调用方"在释放 lisp 环境后不应继续使用该环境。
 * 建议在释放环境后将相关指针置为 NULL。
 */
void lenv_del(lenv *e);

/*
 * 将抽象语法树解析成 lisp 值。
 * "调用方"负责使用 `lval_del` 函数释放返回的值。
 */
lval *lval_read(mpc_ast_t *t);
/*
 * 释放 lisp 值 `v`。
 * "调用方"在释放 lisp 值后不应继续使用该值。
 * 建议在释放值后将相关指针置为 NULL。
 */
void lval_del(lval *v);

/*
 * 使用 lisp 环境 `e` 对 lisp 值 `v` 进行求值。
 * 用于对 LVAL_SYM（符号）类型和 LVAL_SEXPR（S表达式）类型进行求值。
 * 原来的 lval `v' 在求值后被释放，调用者不应再使用它。
 * "调用方"负责使用 `lval_del` 函数释放返回的值。
 */
lval *lval_eval(lenv *e, lval *v);

/*
 * 打印 lisp 环境，用于调试。
 */
void lenv_print(lenv *e);
/*
 * 打印 lisp 值
 */
void lval_print(lval *v);
/*
 * 打印 lisp 值，并在最后添加换行。
 */
void lval_println(lval *v);

#endif
