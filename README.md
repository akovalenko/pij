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

| Code | Meaning |
|------|---------|
| 0 | the initial process exited with code 0 (success) |
| 1 | a PIJ-internal error (process creation or the waiting phase failed) |
| 2 | the initial process exited with a non-zero code |

Code `2` covers any non-zero exit of the initial process: the child's
exact code is not propagated, since it could itself be `1` and would then
be indistinguishable from a PIJ-internal error.

## Restrictions
All arguments are passed to CreateProcess'es lpCommandLine, hence some
restrictions on what you can start: it should be a real EXE file
(please start cmd.exe if you want to run another batch file), standard
directories and PATH are used, registry AppPaths aren't.

## Author
Written by Anton Kovalenko <anton@sw4me.com>. My code is public domain.
Uses some code by Raymond Chen.
