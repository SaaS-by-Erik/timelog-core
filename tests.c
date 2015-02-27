/*
 * Copyright (c) 2015 Erik Nordstroem <contact@erikano.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define NUMTESTS 28
#define TESTNA(xsv, dv) tests[++i_tests]; \
tests[i_tests].xsuccess = xsv; \
tests[i_tests].desc = dv; \
tests[i_tests].argv = (char *[]){tlb, NULL};
#define TEST(xsv, dv, ...) tests[++i_tests]; \
tests[i_tests].xsuccess = xsv; \
tests[i_tests].desc = dv; \
tests[i_tests].argv = (char *[]){tlb, __VA_ARGS__, NULL};

#include <sys/param.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/wait.h>

extern char **environ;

typedef struct _tltest
{
  bool xsuccess; /* Expecting success (true) or failure (false)? */
  char *desc;    /* Description. */
  char **argv;
} tltest;

int main()
{
  int errors = 0;    /* Errors in the test program itself. */
  int nt_passed = 0; /* Number of tests passed. */
  int nt_failed = 0; /* Number of tests failed. */
  char odir[MAXPATHLEN];
  char tlb[MAXPATHLEN];
  struct stat stlb;
  char template[] = "/tmp/tl-XXXXXXXX";
  char *tmpdir = mkdtemp(template);
  const char f_tldir[] = ".tl/";
  const char f_tldb[] = "tl.db";
  const char f_tps[] = "tps.db";
  int i;

  tltest tests[NUMTESTS];
  int i_tests = -1;

  TEST(true, "`tl init'", "init");
  TEST(false, "`tl init' in directory with existing timelog", "init");
  TESTNA(false, "`tl' without arguments");
  TEST(false, "`tl x' (invalid command)", "x");
  TEST(true, "`tl pending' with empty stack", "pending");
  TEST(false, "`tl pop-drop' with empty stack", "pop-drop");
  TEST(false, "`tl merge-add' with empty stack", "merge-add");
  TEST(true, "`tl timepoint'", "timepoint");
  TEST(true, "`tl pending' with one element on stack", "pending");
  TEST(true, "`tl pop-drop' with one element on stack", "pop-drop");
  TEST(true, "`tl timepoint -l <loc>' with valid length loc", "timepoint",
    "-l", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxy");
  TEST(false, "`tl timepoint -l <loc>' with invalid length loc", "timepoint",
    "-l", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxyz");
  TEST(true, "`tl timepoint -m <msg>' with valid length msg", "timepoint",
    "-m", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxy");
  TEST(false, "`tl timepoint -m <msg>' with invalid length msg", "timepoint",
    "-m", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxyz");
  TEST(true, "`tl timepoint -t <ts>' with valid ts #1", "timepoint", "-t",
    "22:22");
  /* In timezone Europe/Oslo, calendar time will contain 0x0a. */
  TEST(true, "`tl timepoint -t <ts>' with valid ts #2", "timepoint", "-t",
    "2015-01-30T23:50");
  TEST(true, "`tl timepoint -t <ts>' with valid ts #3", "timepoint", "-t",
    "2015-01-31T01:02", NULL);
  TEST(true, "`tl pop-drop' to see if tps db was corrupted by valid ts #2",
    "pop-drop");
  TEST(false, "`tl timepoint -t <ts>' with invalid ts #1", "timepoint", "-t",
    "T22:22");
  TEST(false, "`tl timepoint -t <ts>' with invalid ts #2", "timepoint", "-t",
    "22:22:00");
  TEST(false, "`tl timepoint -t <ts>' with invalid ts #3", "timepoint", "-t",
    "2015-01-10t22:22");
  TEST(false, "`tl timepoint -t <ts>' with invalid ts #4", "timepoint", "-t",
    "2014-02-31T22:22");
  TEST(true, "`tl merge-add' with multiple points on stack", "merge-add");
  TEST(true, "`tl report' with one entry in log", "report");
  TEST(false, "`tl unlog' 2nd log entry with only one entry in log", "unlog",
    "2");
  TEST(true, "`tl unlog' 1st log entry", "unlog", "1");
  TEST(false, "`tl unlog' 1st log entry again", "unlog", "1");
  TEST(true, "`tl report' with empty log", "report");

  if (getcwd(odir, sizeof(odir)) == NULL)
  {
    fprintf(stderr, "`getcwd' failed.\n");
    exit(EXIT_FAILURE);
  }

  if (strlcpy(tlb, odir, sizeof(tlb)) > sizeof(tlb) ||
      strlcat(tlb, "/bin/tl", sizeof(tlb)) > sizeof(tlb) ||
      stat(tlb, &stlb) != 0)
  {
    fprintf(stderr, "Creation of path to `tl' binary failed.\n");
    exit(EXIT_FAILURE);
  }

  /* Temporary directory in which tests will be run. */
  if (chdir(tmpdir) != 0)
  {
    fprintf(stderr, "`chdir' into temporary directory `%s' failed.\n", tmpdir);
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
  {
    pid_t pid = fork();
    if (pid < 0)
    {
      fprintf(stderr, "Test #%02d: %s. Not run.\n", i + 1, tests[i].desc);
      errors++;
    }
    else if (pid == 0)
    {
      fclose(stdin);
      fclose(stdout);
      fclose(stderr);
      return execve(tlb, tests[i].argv, environ);
    }
    else
    {
      int r;
      waitpid(pid, &r, 0);
      if ((r == 0 && tests[i].xsuccess) || (r != 0 && !(tests[i].xsuccess)))
      {
        nt_passed++;
      }
      else
      {
        fprintf(stderr, "Test #%02d: %s. Failed.\n", i + 1, tests[i].desc);
        nt_failed++;
      }
    }
  }

  if (chdir(f_tldir) == 0)
  {
    unlink(f_tldb);
    unlink(f_tps);
    chdir("..");
    rmdir(f_tldir);
  }
  if (rmdir(tmpdir) != 0)
  {
    fprintf(stderr, "Failed to remove temporary directory `%s'.\n", tmpdir);
    errors++;
  }

  printf("Ran %d test. %d passed. %d failed.\n", nt_passed + nt_failed,
         nt_passed, nt_failed);

  exit(errors + nt_failed);
}
