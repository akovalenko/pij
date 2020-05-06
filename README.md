## PIJ - Run process tree in a job and wait for completion.

## Usage
`pij my-program.exe arguments...`

## Purpose

Ensure the whole subtree of processes has completed before the batch
file continues.

Let's assume that `my-program.exe` does something useful unattended,
making it usable from batch files. However, its initial process just
starts another process to do the real thing, and then exits, not
waiting for that another process to finish. `START /W` won't help in
this case, as it will return as soon as the initial process exits.

PIJ runs the whole subtree of processes-running-processes in a single
job and waits for this job to become empty before exiting.

## Result code (ERRORLEVEL)
PIJ terminates with non-zero exit code if there's some problem during
initial process creation or waiting phase. Zero exit code is returned
when the last process in the job exits.

## Restrictions
All arguments are passed to CreateProcess'es lpCommandLine, hence some
restrictions on what you can start: it should be a real EXE file
(please start cmd.exe if you want to run another batch file), standard
directories and PATH are used, registry AppPaths aren't.

## Author
Written by Anton Kovalenko <anton@sw4me.com>. My code is public domain.
Uses some code by Raymond Chen.
