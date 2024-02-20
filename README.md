# Minishell

"Minishell" is a command-line interpreter crafted in C, tailored for UNIX/Linux systems. It supports process management functionalities provided by POSIX, facilitating simple or chained commands, input/output redirection, and basic shell scripting. Key features include command parsing, execution, and signal interruption handling. The implementation heavily relies on POSIX system calls such as fork, wait, and exit for process management, alongside pipe, dup, and close for process communication.
