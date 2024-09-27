#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clisp.h>
#include <editline/history.h>
#include <editline/readline.h>
#include <mpc.h>

int main(int argc, char **argv) {
  puts("Lispy Version 0.0.0.0.6");
  puts("Press Ctrl+d to Exit\n");
  signal(SIGINT, SIG_IGN);

  parser_init();
  lenv *e = lenv_new();
  lenv_add_builtins(e);
  parse_args(argc, argv, e);

  /* REPL */
  char *input = NULL;
  while ((input = readline("lispy> "))) {
    add_history(input);

    mpc_result_t r;
    if (parse_line(input, &r)) {
      // parse_print(&r);
      lval *x = lval_eval(e, lval_read(r.output));
      // lenv_print(e);
      lval_println(x);
      lval_del(x);
      parse_delete(&r);
    } else {
      parse_error(&r);
    }

    free(input);
  }
  lenv_del(e);
  putchar('\n');

  parser_quit();
  return 0;
}
