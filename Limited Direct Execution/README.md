*context_switch.c* attempts to find the time it takes to switch contexts.

This is done by opening two pipes between two processes (parent and child in this case), and alternatively writing and reading from the pipes. The parent writes to the first pipe, and attempts to read from the second, meanwhile the OS switches to the child process, which reads from the second pipe and writes to the first.

Note: We practice additional control by allowing the code to run only on a single processor.

We can time this, and establish a rough upper bound for the context-switching cost for a program of this size and complexity.
