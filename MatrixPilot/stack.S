; File: stack.S
 // desired stack size in bytes (including the stack guardband)
 // need at least 470 + guard for MatrixPilot_UDB5_WJP project MatrixPilot-auav3
 #define STACK_SIZE 0x2000
 // allocate stack from 0x6000 to 0x7FFF
 #define STACK_BOT   0X6000


 .section non_eds_stack, stack, address(STACK_BOT)
 .space STACK_SIZE
