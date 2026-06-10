// rshell.h - robot console commands (output over UART).
#ifndef RSHELL_H
#define RSHELL_H

// Try to handle a shell line as a robot command.
// Returns 1 if handled, 0 if the command is unknown to the robot stack.
int robot_shell_command(const char* line);

#endif // RSHELL_H
