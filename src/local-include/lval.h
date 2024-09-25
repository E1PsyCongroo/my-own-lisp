/*
 * lenv.h - 本地环境头文件
 * 此头文件应仅在特定实现中包含，不应对调用者公开。
 * lval.h 包含实现 lisp 值操作所需的函数声明和宏定义
 */
#ifndef __LVAL_H__
#define __LVAL_H__

#include "common.h"
#include <mpc.h>

/*
 * LASSERT 宏 - 用于断言检查并在失败时释放资源并返回错误。
 * 参数：
 * args - 传入的 lval 参数列表，将在断言失败时被释放。
 * cond - 断言检查的条件。
 * fmt - 如果断言失败，用于格式化错误消息的格式字符串。
 * ... - 用于 fmt 字符串的可变参数列表。
 * 用法示例：LASSERT(args, args->count == 2, "Expected two arguments.");
 * 如果 cond 为假，则释放 args 并返回一个格式化的错误 lval。
 *
 * LASSERT_NUM: 检查参数数量
 * LASSERT_TYPE: 检查参数类型
 * LASSERT_NOT_EMPTY: 检查 S表达式或 Q表达式是否为空
 */
#define LASSERT(args, cond, fmt, ...)                                          \
  do {                                                                         \
    if (!(cond)) {                                                             \
      lval *err = lval_err(fmt, ##__VA_ARGS__);                                \
      lval_del(args);                                                          \
      return err;                                                              \
    }                                                                          \
  } while (0)

#define LASSERT_NUM(func, args, num)                                           \
  LASSERT(args, args->count == num,                                            \
          "Function '%s' passed incorrect number of arguments. "               \
          "Got %i, Expected %i.",                                              \
          func, args->count, num)

#define LASSERT_TYPE(func, args, index, expect)                                \
  LASSERT(args, args->cell[index]->type == expect,                             \
          "Function '%s' passed incorrect type for argument %i. "              \
          "Got %s, Expected %s.",                                              \
          func, index, ltype_name(args->cell[index]->type),                    \
          ltype_name(expect))

#define LASSERT_NOT_EMPTY(func, args, index)                                   \
  LASSERT(args, args->cell[index]->count != 0,                                 \
          "Function '%s' passed {} for argument %i.", func, index);

/*
 * 创建一个新的数值类型的 lval。
 * 参数 `x`: 需要封装的数值。
 * 返回: 指向新创建的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_num(long x);
/*
 * 创建一个新的错误类型的 lval。
 * 参数 `fmt`: 错误信息格式字符串。
 * 参数 `...`: 可变参数，用于 fmt 字符串的格式化。
 * 返回: 指向新创建的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_err(char *fmt, ...);
/*
 * 创建一个新的符号类型的 lval。
 * 参数 `s`: 符号字符串。
 * 返回: 指向新创建的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_sym(char *s);
/*
 * 创建一个新的空 S表达式类型的 lval。
 * 返回: 指向新创建的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_sexpr(void);
/*
 * 创建一个新的空 Q表达式类型的 lval。
 * 返回: 指向新创建的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_qexpr(void);
/*
 * 创建一个新的函数类型的 lval。
 * 参数 `func`: 函数指针，指向内置的 lisp 函数。
 * 返回: 指向新创建的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_fun(lbuiltin func);
/*
 * 创建一个新的 lambda 函数类型的 lval。
 * 参数 `formals`: 形式参数列表，LVAL_QEXPR 类型。
 * 参数 `body`: 函数体，LVAL_QEXPR 类型。
 * 返回: 指向新创建的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_lambda(lval *formals, lval *body);
/*
 * 将两个 LVAL_SEXPR | LVAL_QEXPR lval 连接成一个。
 * 参数 `x`, `y`: 需要连接的两个 lval。
 * 返回: 指向连接后的 lval 的指针。
 * 原始 lval 'y' 在连接后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 * 原始 lval `x` 的所有权和管理责任仍由调用者持有。
 * 注意: 返回值与 `x` 指向同一对象。
 */
lval *lval_join(lval *x, lval *y);
/*
 * 返回 lval 的一个副本。
 * 参数 `v`: 需要复制的 lval。
 * 返回: 指向新复制的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 * 原始 lval `v` 的所有权和管理责任仍由调用者持有。
 */
lval *lval_copy(lval *v);
/*
 * 比较两个 lval 是否相等。
 * 参数 `x`, `y`: 需要比较的两个 lval。
 * 返回: 如果相等返回 1，否则返回 0。
 * 原始 lval `x`, `y` 的所有权和管理责任仍由调用者持有。
 */
int lval_eq(lval *x, lval *y);
/*
 * 从抽象语法树节点中读取一个数值，并封装成 LVAL_NUM 类型的 lval。
 * 参数 `t`: 指向 mpc_ast_t 结构的指针，代表抽象语法树节点。
 * 返回:
 *  - 指向新创建的 lval 的指针。
 *  - 如果节点表示的不是有效的数值，则返回错误类型的 lval。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_read_num(mpc_ast_t *t);
/*
 * 将一个 lval 添加到另一个 LVAL_SEPXR | LVAL_QEXPR 类型的 lval 的末尾。
 * 参数 `v`: 指向要添加元素的 lval 的指针。
 * 参数 `x`: 指向要被添加的 lval 的指针。
 * 返回: 指向更新后的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 * 原始 lval `v` 的所有权和管理责任仍由调用者持有。
 * 注意: 返回值与 `v` 指向同一对象。
 */
lval *lval_add(lval *v, lval *x);
/*
 * 将一个 lval 插入到另一个 lval 的开头。
 * 参数 v: 指向要添加元素的 lval 的指针。
 * 参数 x: 指向要被插入的 lval 的指针。
 * 返回: 指向更新后的 lval 的指针。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 * 原始 lval `v` 的所有权和管理责任仍由调用者持有。
 * 注意: 返回值与 `v` 指向同一对象。
 */
lval *lval_add_front(lval *v, lval *x);

/*
 * 获取列表的第一个元素。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: Q表达式，包含一个或多个元素。
 * 返回: 只包含第一个元素的新 Q表达式。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_head(lenv *e, lval *a);
/*
 * 获取列表除第一个元素外的剩余部分。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: Q表达式，包含一个或多个元素。
 * 返回: 不包含第一个元素的新 Q表达式。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_tail(lenv *e, lval *a);
/*
 * 将一个或多个 lval 转换为一个 Q表达式。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 一个或多个 lval。
 * 返回: 包含所有输入 lval 的 Q表达式。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_list(lenv *e, lval *a);
/*
 * 对 Q表达式进行求值。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含一个 Q表达式的 lval。
 * 返回: 求值结果。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_eval(lenv *e, lval *a);
/*
 * 将多个 Q表达式合并成一个。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含多个 Q表达式的 lval。
 * 返回: 合并后的单个 Q表达式。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_join(lenv *e, lval *a);
/*
 * 将一个元素添加到列表的前端。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含一个元素和一个 Q表达式的 lval。
 * 返回: 更新后的 Q表达式。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_cons(lenv *e, lval *a);
/*
 * 获取列表的长度。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含一个 Q表达式的 lval。
 * 返回: 表示列表长度的数值 lval。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_len(lenv *e, lval *a);
/*
 * 获取列表除最后一个元素外的所有元素。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含一个或多个元素的 Q表达式。
 * 返回: 不包含最后一个元素的 Q表达式。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_init(lenv *e, lval *a);
/*
 * 定义变量。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含一个符号列表和相应的值的 lval。
 * 参数 func: 指示变量应该被定义在全局还是局部作用域的字符串（"def", "put"）。
 * 返回: 空的 S表达式，表示定义成功。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_var(lenv *e, lval *a, char *func);
/*
 * 定义全局变量。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含符号和值的 lval。
 * 返回: 空的 S表达式。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_def(lenv *e, lval *a);
/*
 * 定义局部变量。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含符号和值的 lval。
 * 返回: 空的 S表达式。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_put(lenv *e, lval *a);
/*
 * 创建一个用户定义的函数。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含形式参数列表和函数体的 lval。
 * 返回: 用户定义的函数。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_lambda(lenv *e, lval *a);
/*
 * 创建一个命名的用户定义的函数并将其添加到全局环境中。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含函数名称、形式参数列表和函数体的 lval。
 * 返回: 空的 S表达式，表示定义成功。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_fun(lenv *e, lval *a);
/*
 * 终止解释器的执行。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 可选的退出状态代码（如果提供）。
 * 返回: 通常不返回。
 */
lval *builtin_exit(lenv *e, lval *a);

/*
 * 执行基本算术和逻辑运算。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含一个或多个数值 lval 的列表。
 * 参数 `op`: 要执行的运算符（"+", "-", "*", "/"）。
 * 返回: 运算结果。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_op(lenv *e, lval *a, char *op);
lval *builtin_add(lenv *e, lval *a);
lval *builtin_sub(lenv *e, lval *a);
lval *builtin_mul(lenv *e, lval *a);
lval *builtin_div(lenv *e, lval *a);

/*
 * 执行数值比较操作。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含两个数值 lval 的列表。
 * 参数 `op`: 比较操作符（">", "<", ">=", "<="）。
 * 返回: 比较结果。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_ord(lenv *e, lval *a, char *op);
lval *builtin_gt(lenv *e, lval *a);
lval *builtin_lt(lenv *e, lval *a);
lval *builtin_ge(lenv *e, lval *a);
lval *builtin_le(lenv *e, lval *a);
/*
 * 执行逻辑比较操作。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含两个 lval 的列表。
 * 参数 `op`: 比较操作符（"==", "!="）。
 * 返回: 比较结果。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_cmp(lenv *e, lval *a, char *op);
lval *builtin_eq(lenv *e, lval *a);
lval *builtin_ne(lenv *e, lval *a);
/*
 * 执行逻辑运算。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含两个数值 lval 的列表。
 * 参数 `op`: 逻辑操作符（"&&", "||", "!"）。
 * 返回: 逻辑运算结果。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_logic(lenv *e, lval *a, char *op);
lval *builtin_and(lenv *e, lval *a);
lval *builtin_or(lenv *e, lval *a);
lval *builtin_not(lenv *e, lval *a);
/*
 * 条件执行操作。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `a`: 包含条件表达式和两个 Q表达式的列表。
 * 返回: 根据条件表达式求值结果选择执行的 Q表达式的结果。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *builtin_if(lenv *e, lval *a);

/*
 * 从 lval 中移除并返回指定位置的元素，不删除其余元素。
 * 参数 `v`: 包含元素的 lval。
 * 参数 `i`: 要移除元素的索引。
 * 返回: 被移除的元素。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 * 注意: 调用此函数后，原始的 lval 'v' 会失去一个元素，其余元素向前移动。
 */
lval *lval_pop(lval *v, int i);
/*
 * 从 lval 中移除并返回指定位置的元素，删除剩余元素。
 * 参数 `v`: 包含元素的 lval。
 * 参数 `i`: 要提取的元素的索引。
 * 返回: 被提取的元素。
 * 原始 lval 'v' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_take(lval *v, int i);
/*
 * 根据函数 lval 和给定的参数列表调用函数。
 * 参数 `e`: 当前 lisp 环境。
 * 参数 `f`: 函数 lval。
 * 参数 `a`: 包含实际参数的 lval。
 * 返回: 函数调用的结果。
 * 原始 lval 'a' 在求值后被释放，调用者不应再使用它。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_call(lenv *e, lval *f, lval *a);
/*
 * 对 S表达式 lval 进行求值。
 * 参数 `e`: 当前 Lisp 环境。
 * 参数 `v`: 包含 S表达式的 lval。
 * 返回: 求值后的结果。
 * S表达式中的所有元素，如果任何元素求值为错误，将立即返回错误。
 * 原始 lval 'v' 在求值后被释放，调用者不应再使用它。
 * 原始 lval `f` 的所有权和管理责任仍由调用者持有。
 * "调用者"负责使用 `lval_del` 释放返回的 lval。
 */
lval *lval_eval_sexpr(lenv *e, lval *v);

/*
 * 根据 lval 类型获取相应的类型名称字符串。
 * 参数 `t`: lval 的类型枚举值。
 * 返回: 对应类型的名称字符串。此字符串为静态分配，不应被释放或修改。
 */
char *ltype_name(int t);
/*
 * 打印 lval 表达式，根据指定的开闭字符封装表达式。
 * 参数 `e`: 指向当前 lisp 环境的指针，用于可能的环境依赖的打印操作。
 * 参数 `v`: 指向要打印的 lval 表达式的指针。
 * 参数 `open`: 表达式开始的字符，如 '(' 或 '{'。
 * 参数 `close`: 表达式结束的字符，如 ')' 或 '}'。
 * 说明: 该函数递归调用 lval_print 来打印 v
 * 内部的每个元素，元素之间用空格分隔，最后不带额外空格。
 */
void lval_expr_print(lenv *e, lval *v, char open, char close);

#endif
