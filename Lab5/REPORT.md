# Operating Systems: Laboratory Work No. 5

**The File System with Your Own Hands** (Track A: warm-up on Linux)

**Elaborated by:** Veacheslav Morari, FAF-243
**Verified by:** Patricia Reţman

---

## Contents

- [Introduction](#introduction)
- [1. The tree and the file types](#1-the-tree-and-the-file-types)
- [2. Inodes and links](#2-inodes-and-links)
- [3. Permissions](#3-permissions)
- [4. Disks and the kernel as files](#4-disks-and-the-kernel-as-files)
- [Conclusion](#conclusion)

---

## Introduction

The purpose of this laboratory work is to check on a real Linux system what was shown in the lecture about the file system: the directory tree and the types of files, inodes, hard and symbolic links, permissions with the special bits, and the disks and the kernel interface `/proc`. No programs are written; only standard commands are used, and the results are explained.

The work was done on Ubuntu 26.04.1 LTS in a virtual machine, as the normal user `veacheslav` and not as `root`, because `root` ignores most permission checks and the experiments would give wrong results. All files were created in the folder `~/os-lab5`.

## 1. The tree and the file types

The working folder was created and the root directory was listed. The first letter of the line printed by `ls -l` gives the type of an object: `-` is a regular file, `d` a directory, `l` a symbolic link, `c` a character device and `b` a block device.

```
$ whoami
veacheslav
$ mkdir -p ~/os-lab5
$ cd ~/os-lab5
$ pwd
/home/veacheslav/os-lab5
$ ls /
bin   cdrom  etc   lib         mnt   proc  run   snap  sys  usr
boot  dev    home  lib64 media opt   root  sbin  srv   tmp  var
```

The command `ls -ld /etc /etc/passwd /bin /dev/null /tmp` showed the type of five different paths. The `/dev` directory contains hundreds of entries, so `ls -l /dev | head` was used to see the beginning. The command `file` was used to confirm the type of a few paths, and `du -sh` printed the sizes of `/var/log` and `/usr`. Finally, `lsblk` listed the disks and partitions.

### 1.1. Observations

(1) The path `/etc` is a directory (`d`), `/etc/passwd` is a regular file (`-`), `/bin` is a symbolic link (`l`), `/dev/null` is a character device (`c`) and `/tmp` is a directory (`d`) whose last permission letter is `t`, the sticky bit.

(2) On this system `/bin` is not a directory but a symbolic link to `usr/bin`. The command `file` confirms it. It also shows that `/usr/bin/ls` is itself a link, to `../lib/cargo/bin/coreutils/ls`, and that `/etc/passwd` is plain ASCII text.

(3) The device `/dev/null` is a character device (`c`, major 1, minor 3), and `/dev/loop0` is a block device (`b`, major 7, minor 0). Block devices are the ones that are read and written in blocks, such as disks and the loop devices used here for Snap packages. The listing of `/dev` also contains directories and links, for example `cdrom -> sr0`.

(4) The folder `/usr` is bigger: it takes 5.1 GB, while `/var/log` takes 459 MB, so `/usr` is about 4.6 GB larger. This is expected, because `/usr` holds the programs and libraries of the system and `/var/log` holds only logs.

## 2. Inodes and links

### 2.1. An inode and a name

A file `notes.txt` with the text `hello` was created and examined with `stat` and `ls -li`. The size is 6 bytes (five letters and the newline), the inode number is 1701749 and the file has one link.

```
$ echo hello > notes.txt
$ stat notes.txt
  File: notes.txt
  Size: 6             Blocks: 8          IO Block: 4096   regular file
Device: 8,2    Inode: 1701749    Links: 1
Access: (0664/-rw-rw-r--)  Uid: ( 1000/veacheslav)   Gid: ( 1000/veacheslav)
```

### 2.2. A hard link and removing the original

The command `ln notes.txt hard.txt` created a second name for the same inode: both names show inode 1701749 and the link count became 2. After `rm notes.txt` the file was still readable under the name `hard.txt`, and the link count went back to 1.

### 2.3. A symbolic link and removing its target

The symbolic link `soft.txt` got its own inode, 1701750, and its type is `l`. Reading through it worked while `hard.txt` existed. After `rm hard.txt` the link remained in the directory, but `cat soft.txt` failed with "No such file or directory".

### 2.4. Link counts of directories

A new directory `d` had 2 links. After the creation of `d/x` and `d/y` the count became 4.

### 2.5. Size is not space

A file with 2 bytes occupies 4.0K according to `du`, and a sparse file of 1 GiB created with `truncate` occupies 0. The big file was removed afterwards.

### 2.6. Observations

(1) The inode number is the first column of `ls -li` and the field `Inode` in the output of `stat`. The number of names of the file is the field `Links` in `stat`, or the number after the permission string in `ls -l`.

(2) After `rm notes.txt` the file could still be read because the name is not part of the file. A name is only an entry in a directory that points to an inode, and the inode with the data had two such entries. The command `rm` removes one entry and lowers the link count; the data are freed only when the count reaches zero and no process keeps the file open.

(3) The command `cat soft.txt` failed because a symbolic link stores only the path of its target, not the content. When `hard.txt` was removed, the path pointed to nothing, so the link became dangling, although the link itself still exists. The size of `soft.txt` is 8 bytes because it holds the text of the path, `hard.txt`, which has exactly 8 characters.

(4) The link count of a new directory is 2 because the directory is referenced from its own name there and from its own entry `.` (its own name there). Each subdirectory contains an entry `..` that points back to the parent, so two subdirectories add two links, and the total is 4. Plain files do not add anything to the count of the parent.

(5) A file system allocates space in blocks, and the smallest block here is 4 KiB, so a file of 2 bytes takes one whole block, and `du` shows 4.0K. The file `big.img` is sparse: `truncate` only set its length to 1 GiB and wrote no data, so no blocks were allocated and `du` shows 0, while `ls` shows the logical size of 1.0G.

## 3. Permissions

### 3.1. Files and umask

The file `s.txt` was given mode 600, then 000, and then the read right of the owner was added with `u+r`. In the state 000 even the owner got "Permission denied". The umask of the user is 0002, and the new file and directory got modes 664 and 775.

### 3.2. Directories

In the directory `box` with the file `f` two modes were tested. With read only (`u=r`) the command `ls box` printed the name `f`, but `cat box/f` was denied. With execute only (`u=x`) `ls box` was denied, but `cat box/f` printed `hi`. At the end the mode was restored with `chmod 755 box`.

### 3.3. Special bits and other people's files

The program `/usr/bin/passwd` has the letter `s` in the owner's execute position, and `/tmp` has the letter `t` at the end. The file `/etc/shadow` has mode 640, belongs to `root` and to the group `shadow`, so the ordinary user cannot read it.

### 3.4. Observations

(1) The mode 600 is `rw-------`: a private file, for example a secret key or a password file. The mode 644 is `rw-r--r--`: an ordinary file that everyone may read, such as a document or a web page. The mode 755 is `rwxr-xr-x`: a program or a public directory. The mode 700 is `rwx------`: a private directory of the owner, for example `~/.ssh`.

(2) Ownership does not help because the kernel checks the permission bits of the class that matches the user. For the owner these are the first three bits, and with mode 000 they are all empty, so reading is refused. The owner is only allowed to change the mode with `chmod`, which is what restored access in the experiment.

(3) The `umask` is 0002. New files start from 666 and new directories from 777, and the bits of the mask are removed. The mask 002 removes the write bit for others, so `new.txt` got 664 (`rw-rw-r--`) and `newdir` got 775 (`rwxrwxr-x`), as the listing shows.

(4) On a directory, the right `r` allows reading the list of names, which is why `ls box` worked in the first test and failed in the second. The right `x` allows passing through the directory and reaching an entry whose name is already known, which is why `cat box/f` failed without `x` and worked with it. Without `x` the names in the directory are visible but useless; without `r` the file is reachable only if you know its name.

(5) The letter `s` in `/usr/bin/passwd` is the setuid bit: when any user runs the program, it works with the rights of the owner of the file, here `root`. It is needed because `passwd` must write to protected files such as `/etc/shadow`, which an ordinary user cannot even read, as the failed `cat /etc/shadow` showed. The letter `t` in `/tmp` is the sticky bit: everybody can create files in this shared directory, but a user can delete or rename only their own files there.

## 4. Disks and the kernel as files

The disks were listed with `lsblk` (Figure 1.5), and the root file system was examined with `df -h` and `mount`. The kernel information was read from the virtual files `/proc/cpuinfo`, `/proc/meminfo` and `/proc/self/status`, and the open files of a process from `/proc/self/fd`.

```
$ df -h /
Filesystem      Size   Used  Avail Use% Mounted on
/dev/sda2       50G    8.3G  39G   18%  /
$ mount | grep " / "
/dev/sda2 on / type ext4 (rw,relatime)
```

### 4.1. Observations

(1) The disk is `sda` with a size of 50.4 GiB. It has two partitions: `sda1` of 1 MiB, which is not mounted (it is typically a small boot area), and `sda2` of 50.4 GiB, which is mounted at `/`. The file system on `/dev/sda2` is `ext4`, mounted read-write (`rw,relatime`). The loop devices `loop0`–`loop18` are not real disks: they are Snap packages mounted from files, and `sr0` is the virtual CD with the VirtualBox Guest Additions.

(2) The root file system is 18% full: 8.3 GB are used and 39 GB are available out of 50 GB.

(3) The `grep -c processor /proc/cpuinfo` command printed 7, so the virtual machine has 7 CPU cores. The field `MemTotal` is 7 601 204 kB, which is about 7.25 GiB (7.6 GB) of RAM; at that moment 6 148 248 kB were available.

(4) The file `/proc/self/status` showed the name `head`. The path `self` always means the process that opens the file, and the process that read it was `head`, which started in the same command line and read the file itself. The same happens in the last experiment: the link `3 -> /proc/6563/fd` belongs to `ls`, which had the directory open while it listed it, and descriptors 0, 1 and 2 are the terminal `/dev/pts/0`. None of these files is stored on the disk: `/proc` is generated by the kernel when it is read.

## Conclusion

In this work the main parts of the Linux file system were checked by experiment. The tree contains regular files, directories, links and device files, and the first letter of `ls -l` tells them apart. A name is only an entry in a directory that points to an inode, so several hard links can share one file, removing one name does not remove the data, and a symbolic link, which keeps only a path, breaks when its target disappears. The link counts of directories follow from the entries `.` and `..`, and the difference between the logical size and the used space is explained by block allocation and sparse files. The permission experiments showed that the owner is still bound by the mode bits, that `r` and `x` mean different things on a directory, that `umask` decides the modes of new objects, and that the setuid and sticky bits solve the problems of changing a password and of sharing `/tmp`. The last part showed that the disk is split into partitions with an `ext4` file system mounted at `/`, and that the kernel exposes its own data, such as the number of CPUs, the memory size and the state of a process, as ordinary files in `/proc`.
