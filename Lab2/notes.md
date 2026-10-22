# Lab 2 — Meet the OS You Will Build (xv6)

**Student:** Morari Veaceslav, FAF-243

**Environment note:** I did this lab in Ubuntu 26.04 under WSL2 instead of the course VM. Two things were different from the lab sheet. First, the RISC-V emulator is now a separate package, so I also had to install `qemu-system-riscv`. Second, in the current xv6 source the `sleep` system call has been renamed to `pause` (upstream commit f5dea58: "user sleep() -> pause()"), so the lab's `sleep(...)` call does not compile there. My `sleep.c` handles both versions (see Part 4).

## Part 1. Build and boot

`make qemu` compiled the kernel and all user programs, created the disk image `fs.img`, and booted xv6 in QEMU:

```
xv6 kernel is booting

hart 1 starting
hart 2 starting
init: starting sh
$
```

QEMU emulates a RISC-V machine with 3 CPU cores ("harts"), 128 MB of RAM and one disk. As in Linux, the first user process is `init`, and it starts the shell.

## Part 2. Using xv6

```
$ ls | grep c
cat            2 3 36728
echo           2 4 35592
wc             2 17 37728
sync           2 22 34944
console        3 23 0
$ wc README
48 336 2441 README
```

**(1) Three programs xv6 ships with:** `cat`, `grep` and `wc`. Other examples are `ls`, `echo`, `mkdir`, `rm`, `kill` and the shell `sh`. In the output of `ls`, the second column is the file type: 1 is a directory, 2 is a regular file and 3 is a device. `console` has type 3, so even in xv6 the terminal is shown as a file.

**(2) OS features needed for a pipe:** First, the OS must support **processes**: the shell uses `fork` and `exec` to create two processes, `ls` and `grep`, that run at the same time. Second, it must support **inter-process communication**: the kernel creates a pipe, which is a buffer inside the kernel with a write end and a read end. The output of `ls` goes into the write end, and `grep` reads its input from the read end. The two processes cannot touch each other's memory, so the kernel has to move the data between them. The shell connects them with file descriptors (`pipe`, `dup`, `close`).

**(3) xv6 shell vs. Linux shell:** The xv6 shell works like bash from Lab 1, with the same kind of commands, pipes and redirection, but it is much simpler: there is no command history, no tab completion, no variables, and files have no owners or permissions.

## Part 3. Reading the source

**(1) System calls used by `user/cat.c`:**
`read(fd, buf, n)` asks the kernel to read up to `n` bytes from an open file into the program's buffer; `write(1, buf, n)` asks the kernel to write `n` bytes to file descriptor 1, which is the standard output (the screen); `open(path, O_RDONLY)` asks the kernel to open a file for reading and return a file descriptor; `close(fd)` asks the kernel to close the file; `exit(status)` asks the kernel to end the process.

`fprintf(2, ...)` is not a system call itself. It is a library function that calls `write` on file descriptor 2 (standard error).

**(2) Where `sys_read` is implemented:** in `kernel/sysfile.c`, at line 69 (`sys_write` is at line 83).

```
$ grep -n "sys_read\|sys_write" kernel/sysfile.c
69:sys_read(void)
83:sys_write(void)
```

**(3) Difference between `kernel/` and `user/`:** The code in `kernel/` is the operating system itself. It runs in privileged (supervisor) mode, with direct access to the hardware and to all memory. The code in `user/` consists of ordinary programs that run without privileges. Whenever they need the hardware, a file or another process, they must ask the kernel through a system call; the full list of these requests is in `user/user.h`.

## Part 4. My first xv6 program

I created `user/sleep.c` and added `$U/_sleep\` to `UPROGS` in the Makefile, right after `_echo`, with a tab for indentation. Because this xv6 version names the system call `pause`, the program checks `kernel/syscall.h` when it is compiled: if `SYS_pause` exists it calls `pause()`, otherwise it calls `sleep()`. So the same file builds on both the new and the old xv6.

Test inside xv6:

```
$ ls
...
sleep          2 5 35280
...
$ sleep
usage: sleep <ticks>
$ sleep 10
$ sleep 100
$
```

With no argument the program prints its usage message and exits. `sleep 10` pauses briefly, and `sleep 100` pauses for about 10 seconds, after which the prompt comes back. A tick is one timer interrupt, which is about 0.1 s in xv6.
