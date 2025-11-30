# Linux_device_driver
This directory contains codes for various linux device driver. 

1. Creating a File in /proc Using a Linux Device Driver (proc_ldd.c)

This module demonstrates how to create a simple file under the /proc directory using the Linux kernel’s proc_fs interface.

A. Steps to Create a File in /proc

Include required kernel headers (proc_fs.h, module.h, init.h).

Define a struct proc_dir_entry * to hold the proc entry.

Define a struct proc_ops structure (for read/write handlers).

Call proc_create() in the module’s init function to create /proc/my_module.

Store the returned entry pointer for cleanup.

Call proc_remove() in the module’s exit function to delete the entry.

Compile the module using the kernel build system.

Load the module using insmod → the file appears in /proc.

Unload using rmmod → the file is removed.

B. Code Structure Overview

Module Metadata
Provides licensing, author, and description information.

Proc Entry Pointer
Used to store the handle returned by proc_create().

proc_ops Table
Holds read/write callback functions (currently empty; added later).

Module Init Function
Calls proc_create("my_module", …) to create /proc/my_module.

Module Exit Function
Calls proc_remove() to cleanly remove the proc entry on unload.


D. How to Compile the Module (Makefile)
obj-m += proc_ldd.o → tells the kernel to build proc_ldd.c as a module.

Running make compiles and generates proc_ldd.ko.

E. Load the Module
sudo insmod proc_ldd.ko
dmesg | tail
ls /proc/my_module

F. Unload the Module
sudo rmmod proc_ldd
dmesg | tail
ls /proc/my_module   # should be removed

2. Read/Write Behavior of the /proc File

A. Read (my_read)

The read() function only returns data until the end of the stored message.

It correctly uses and updates the file offset, which is required for tools like cat to stop at end-of-file (EOF).

Behavior:

First read → returns the message and advances the offset.

Next read → offset ≥ length → returns 0 → EOF.

This prevents repeated data output and ensures compatibility with standard user-space tools.

B. Write (my_write)

The write() function accepts data from user space and stores it inside the kernel buffer.

It copies only as many bytes as fit in the kernel buffer, preventing overflow.

Importantly:

It does not modify the file offset.
Procfs write operations should always behave like writing to a simple memory region—each write replaces the buffer content directly.

After a successful write, the buffer is NULLs-terminated so the message can be read back safely
