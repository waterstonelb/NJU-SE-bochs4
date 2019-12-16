
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"


PUBLIC	PROCESS			proc_table[NR_TASKS];

PUBLIC	char			task_stack[STACK_SIZE_TOTAL];

PUBLIC	TASK	task_table[NR_TASKS] = {{ReaderA, STACK_SIZE_READERA, "ReaderA"},
					{ReaderB, STACK_SIZE_READERB, "ReaderB"},
					{ReaderC, STACK_SIZE_READERC, "ReaderC"},
                    {WriterD,STACK_SIZE_WRITERD,"WriterD"},
                    {WriterE,STACK_SIZE_WRITERE,"WriterE"},
                    {NormalF,STACK_SIZE_NORMALF,"NormalF"}};

PUBLIC	irq_handler		irq_table[NR_IRQ];

PUBLIC	system_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks,sys_dispstr,sys_color_dispstr,sys_sleep,sys_P,sys_V};

