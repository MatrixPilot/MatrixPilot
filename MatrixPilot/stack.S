; File: stack.S
 // desired stack size in bytes (including the stack guardband)
 // need at least 470 + guard for MatrixPilot_UDB5_WJP project MatrixPilot-auav3
 #define STACK_SIZE 4096
 // allow 8KB of data below stack
 #define STACK_BOT   0X2000


 .section non_eds_stack, stack, address(STACK_BOT)
 .space STACK_SIZE
