#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <db_185.h>
#include <fcntl.h>
#include <limits.h>

void usage(char* pname[])
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, " %s create [-f <file.db>]\n", *pname);
  fprintf(stderr, " %s begin <location> [-c <comment>] [<ts>] [-f <file.db>]\n", *pname);
  fprintf(stderr, " %s end <id> <location> [-c <comment>] [<ts>] [-f <file.db>]\n", *pname);
  fprintf(stderr, " %s report [<ts> [<ts>]] [-f <file.db>]\n", *pname);
}

void tl_begin(int cmd_argc, char* cmd_argv[], DB* tl_db, char* pname[])
{
}

void tl_end(int cmd_argc, char* cmd_argv[], DB* tl_db, char* pname[])
{
}

void tl_report(int cmd_argc, char* cmd_argv[], DB* tl_db, char* pname[])
{
}

int main (int argc, char* argv[])
{
  char** pname = &(argv[0]);
  if (argc < 2)
  {
    fprintf(stderr, "%s: No command provided.\n\n", *pname);
    usage(pname);
    exit(EXIT_FAILURE);
  }

  char* f;
  int cmd_argc = argc;
  if (argc > 3 && strcmp(argv[argc - 2], "-f") == 0)
  {
    f = argv[argc - 1];
    cmd_argc -= 2;
  } else
  {
    f = "tl_default.db";
    fprintf(stderr, "Using tl database %s.\n", f);
  }

  char** cmd = &(argv[1]);
  cmd_argc -= 1;
  char** cmd_argv = &(argv[2]);
  if (strcmp(*cmd, "create") == 0)
  {
    if (cmd_argc > 1)
    {
      fprintf(stderr, "%s: create: %d additional argument(s) passed. First: `%s'.\n", *pname, cmd_argc, *cmd_argv);
      usage(pname);
      exit(EXIT_FAILURE);
    }
    DB* tl_db = dbopen(f, O_CREAT | O_RDWR, 0, DB_HASH, NULL);
    if (tl_db == NULL)
    {
      fprintf(stderr, "%s: Failed to create tl database `%s'.\n", *pname, *f);
      exit(EXIT_FAILURE);
    }
  } else {
    DB* tl_db = dbopen(f, O_RDWR, 0, DB_HASH, NULL);
    if (tl_db == NULL)
    {
      fprintf(stderr, "%s: Failed to open tl database `%s'.\n", *pname, *f);
      exit(EXIT_FAILURE);
    }

    if (strcmp(*cmd, "begin") == 0)
    {
      tl_begin(cmd_argc, cmd_argv, tl_db, pname);
    } else if (strcmp(*cmd, "end") == 0)
    {
      tl_end(cmd_argc, cmd_argv, tl_db, pname);
    } else if (strcmp(*cmd, "report") == 0)
    {
      tl_report(cmd_argc, cmd_argv, tl_db, pname);
    } else
    {
      fprintf(stderr, "%s: Unknown command %s.\n\n", *pname, *cmd);
      usage(pname);
      exit(EXIT_FAILURE);
    }
  }


  exit(EXIT_SUCCESS);
}
