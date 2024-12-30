#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct cmd cmd_t;

typedef struct {

  FILE *out;

  FILE *err;

} redir_t;

typedef void (*cmd_callback_t)(cmd_t *cmd, int argc, char **argv,

                               redir_t *redir);

struct cmd {

  const char *str;

  unsigned len;

  cmd_callback_t callback;
};

int parse_input(char **argv, char *inp);

cmd_t *get_builtin(const char *inp);

int get_from_path(char *buf, const char *command);

void exit_callback(cmd_t *, int, char **, redir_t *);

void echo_callback(cmd_t *, int, char **, redir_t *);

void type_callback(cmd_t *, int, char **, redir_t *);

void pwd_callback(cmd_t *, int, char **, redir_t *);

void cd_callback(cmd_t *, int, char **, redir_t *);

cmd_t builtins[] = {

    {"exit", 4, exit_callback}, {"echo", 4, echo_callback},

    {"type", 4, type_callback}, {"pwd", 3, pwd_callback},

    {"cd", 2, cd_callback},

};

unsigned n_builtins = sizeof(builtins) / sizeof(*builtins);

redir_t setup_redir(int argc, char **argv) {

  redir_t r = {stdout, stderr};

  for (int i = 0; i < argc; i++) {

    char *arg = argv[i];

    if (strcmp(arg, ">") == 0 || strcmp(arg, "1>") == 0) {

      argv[i] = NULL;

      r.out = fopen(argv[i + 1], "wb");

    } else if (strcmp(arg, "2>") == 0) {

      argv[i] = NULL;

      r.err = fopen(argv[i + 1], "wb");

    } else if (strcmp(arg, ">>") == 0 || strcmp(arg, "1>>") == 0) {

      argv[i] = NULL;

      r.out = fopen(argv[i + 1], "ab");

    } else if (strcmp(arg, "2>>") == 0) {

      argv[i] = NULL;

      r.err = fopen(argv[i + 1], "ab");
    }
  }

  return r;
}

void cleanup_redir(redir_t *redir) {

  if (redir->out != stdout) {

    fclose(redir->out);

    redir->out = stdout;
  }

  if (redir->err != stderr) {

    fclose(redir->err);

    redir->err = stderr;
  }
}

int main() {

  while (1) {

    char input[100];

    printf("$ ");

    fflush(stdout);

    fgets(input, 100, stdin);

    char *argv[100] = {0};

    int argc = parse_input(argv, input);

    if (!argc)

      continue;

    redir_t redir = setup_redir(argc, argv);

    cmd_t *cmd = get_builtin(argv[0]);

    if (cmd) {

      cmd->callback(cmd, argc, argv, &redir);

      cleanup_redir(&redir);

      continue;
    }

    char buf[1024];

    if (get_from_path(buf, argv[0])) {

      argv[0] = buf;

      pid_t pid = fork();

      if (pid == -1) {

        cleanup_redir(&redir);

        exit(1);

      } else if (pid == 0) {

        if (redir.out != stdout)

          dup2(fileno(redir.out), STDOUT_FILENO);

        if (redir.err != stderr)

          dup2(fileno(redir.err), STDERR_FILENO);

        cleanup_redir(&redir);

        execv(argv[0], argv);

        exit(1);

      } else {

        waitpid(pid, NULL, 0);

        continue;
      }
    }

    fprintf(stderr, "%s: command not found\n", input);
  }

  return 0;
}

static inline int is_space(char c) {

  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

int parse_input(char **argv, char *inp) {

  int argc = 0;

  inp[strlen(inp) - 1] = 0;

  for (; *inp && is_space(*inp); inp++)

    ;

  if (!*inp)

    return 0;

  argv[argc++] = inp;

  int escaped = 0;

  int inquotes = 0;

  unsigned len = strlen(inp);

  for (char *i = inp; *i; len--) {

    if (*i == '\\' && !inquotes) {

      memmove(i, i + 1, len);

      i++;

    } else if (*i == '\\' && inquotes) {

      char n = *(i + 1);

      if (n == '"' || n == '\n' || n == '$' || n == '\\')

        memmove(i, i + 1, len);

      i++;

    } else if (*i == '\'' && !inquotes) {

      memmove(i, i + 1, len);

      for (; *i && *i != '\''; i++, len--)

        ;

      memmove(i, i + 1, len);

    } else if (*i == '"') {

      inquotes = !inquotes;

      memmove(i, i + 1, len);

    } else if (!inquotes && is_space(*i)) {

      *i = 0;

      for (i = i + 1; *i && is_space(*i); i++, len--)

        ;

      if (*i)

        argv[argc++] = i;

    } else {

      i++;
    }
  }

  return argc;
}

cmd_t *get_builtin(const char *inp) {

  unsigned ilen = strlen(inp);

  for (int i = 0; i < n_builtins; i++) {

    const char *str = builtins[i].str;

    unsigned len = builtins[i].len;

    if (len == ilen && memcmp(str, inp, len) == 0)

      return &builtins[i];
  }

  return NULL;
}

int get_from_path(char *buf, const char *command) {

  const char *path = getenv("PATH");

  for (const char *p = path; *p; p++) {

    int i = 0;

    while (*p && *p != ':')

      buf[i++] = *p++;

    buf[i++] = '/';

    for (const char *c = command; *c; c++)

      buf[i++] = *c;

    buf[i++] = 0;

    if (access(buf, X_OK) == F_OK)

      return 1;
  }

  return 0;
}

void exit_callback(cmd_t *cmd, int argc, char **argv, redir_t *r) {

  exit(argc > 1 ? atoi(argv[1]) : 0);
}

void echo_callback(cmd_t *cmd, int argc, char **argv, redir_t *r) {

  for (argv = argv + 1; *(argv + 1); argv++)

    fprintf(r->out, "%s ", *argv);

  fprintf(r->out, "%s\n", *argv);
}

void type_callback(cmd_t *cmd, int argc, char **argv, redir_t *r) {

  const char *command = argv[1];

  cmd = get_builtin(command);

  if (cmd) {

    fprintf(r->out, "%s is a shell builtin\n", cmd->str);

    return;
  }

  char buf[4096];

  if (get_from_path(buf, command)) {

    fprintf(r->out, "%s is %s\n", command, buf);

    return;
  }

  fprintf(r->err, "%s: not found\n", command);
}

void pwd_callback(cmd_t *cmd, int argc, char **argv, redir_t *r) {

  (void)argc;

  (void)argv;

  char buf[1024] = {0};

  getcwd(buf, sizeof(buf) - 1);

  fprintf(r->out, "%s\n", buf);
}

void cd_callback(cmd_t *cmd, int argc, char **argv, redir_t *r) {

  const char *dir = argv[1];

  char buf[1024] = {0};

  if (*dir == '~') {

    char *home = getenv("HOME");

    char *end = stpcpy(buf, home);

    strcpy(end, dir + 1);

    dir = buf;
  }

  if (chdir(dir) != 0) {

    fprintf(r->err, "cd: %s: No such file or directory\n", dir);

    return;
  }
}
