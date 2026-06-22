const char *Help = R"HELP(
PIJ - Run process tree in a job and wait for completion.

Usage:
  pij my-program.exe arguments...

Use it when you want to run my-program.exe in a batch file, but
my-program.exe starts another process(es) and returns before they
complete, which is not what you want: batch file should continue only
when all additional processes also exit. PIJ runs initial process in a
job and waits for this job to become empty, before exiting.

PIJ exit codes:
  0   the initial process exited with code 0 (success);
  1   a PIJ-internal error (process creation or the waiting phase failed);
  2   the initial process exited with a non-zero code.

Code 2 covers any non-zero exit of the initial process - the child's
exact code is not propagated, since it could itself be 1 and would then
be indistinguishable from a PIJ-internal error.

All arguments are passed to CreateProcess'es lpCommandLine, hence some
restrictions on what you can start: it should be a real EXE file
(please start cmd.exe if you want to run another batch file), standard
directories and PATH are used, registry AppPaths aren't.

Written by Anton Kovalenko <anton@sw4me.com>. Public domain.
)HELP";

