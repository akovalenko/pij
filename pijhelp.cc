const char *Help = R"HELP(
PIJ - Run process tree in a job and wait for completion.

Usage:
  pij my-program.exe arguments...

Use it when you want to run my-program.exe in a batch file, but
my-program.exe starts another process(es) and returns before they
complete, which is not what you want: batch file should continue only
when all additional processes also exit. PIJ runs initial process in a
job and waits for this job to become empty, before exiting.

PIJ terminates with non-zero exit code if there's some problem during
initial process creation or waiting phase. Zero exit code is returned
when the last process in the job exits.

All arguments are passed to CreateProcess'es lpCommandLine, hence some
restrictions on what you can start: it should be a real EXE file
(please start cmd.exe if you want to run another batch file), standard
directories and PATH are used, registry AppPaths aren't.

Written by Anton Kovalenko <anton@sw4me.com>. Public domain.
)HELP";

