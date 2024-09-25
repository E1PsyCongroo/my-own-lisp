/*
 * lenv.h - 本地环境头文件
 * 此头文件应仅在特定实现中包含，不应对调用者公开。
 * 包含实现所需的函数声明。
 */
#ifndef __LENV_H__
#define __LENV_H__

#include "common.h"
#include <mpc.h>

/*
 * 根据 lval `k` 从全局环境中获取内置函数的名称。
 * 此函数用于实现 lval_print 相关的功能
 * 返回值：
 *  - 如果 `k` 不是内置函数，则返回 "not builtin function"（非内置函数）。
 *  - 如果 `k` 是内置函数类型但在环境中无法找到，则返回 "unknown"（未知），
 *    可能指示实现错误。
 */
char *lenv_get_builtin_name(lenv *e, lval *k);
/*
 * 从环境 `e` 或其父环境中获取符号 `k` 的值。
 * 如果找不到符号 `k`，返回 LVAL_ERR。
 * "调用方"负责使用 `lval_del` 释放返回的值。
 */
lval *lenv_get(lenv *e, lval *k);
/*
 * 将值 `v` 绑定到环境 `e` 中的符号 `k`。
 * 注意：`lenv_put` 不对 `k` 和 `v` 拥有所有权，不负责释放它们。
 */
void lenv_put(lenv *e, lval *k, lval *v);
/*
 * 将值 `v` 绑定到从环境 `e` 追溯到的全局环境中的符号 `k`。
 * 注意：`lenv_def` 不对 `k` 和 `v` 拥有所有权，不负责释放它们。
 */
void lenv_def(lenv *e, lval *k, lval *v);
/*
 * 返回环境 `e` 的一个副本。
 * 调用方负责使用 `lenv_del` 释放返回的环境。
 * 注意：`lenv_copy` 不会释放原始环境 `e`。
 */
lenv *lenv_copy(lenv *e);
/*
 * 向环境 `e` 添加一个内置函数，提供函数名 `name` 和函数指针 `func`。
 */
void lenv_add_builtin(lenv *e, char *name, lbuiltin func);

#endif
