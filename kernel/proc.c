
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"
void next();
void clear();
void wakeup(int);
void block();
/*======================================================================*
                              schedule
 *======================================================================*/
// PUBLIC void schedule()
// {
// 	PROCESS* p;
// 	int	 greatest_ticks = 0;

// 	while (!greatest_ticks) {
// 		for (p = proc_table; p < proc_table+NR_TASKS; p++) {
// 			if (p->ticks > greatest_ticks) {
// 				greatest_ticks = p->ticks;
// 				p_proc_ready = p;
// 			}
// 		}

// 		if (!greatest_ticks) {
// 			for (p = proc_table; p < proc_table+NR_TASKS; p++) {
// 				p->ticks = p->priority;
// 			}
// 		}
// 	}
// }
PUBLIC void schedule()
{
	
	PROCESS* p;
	for(p = proc_table; p < proc_table+NR_TASKS; p++){
		if (p->ticks > 0) {
			p->ticks--;
		}		
	}	

	next();
	
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}
PUBLIC void sys_dispstr(char* str){
	disp_str(str);
	if(disp_pos > 80*24*2){
		milli_delay(2000000);
		clear();
	}
}
PUBLIC void sys_color_dispstr(char* str,int color){
	disp_color_str(str,color);
	if(disp_pos > 80*24*2){
		milli_delay(2000000);
		clear();
	}
}
PUBLIC  void   sys_sleep(int milli_sec)
{
	
	p_proc_ready->ticks = milli_sec * HZ / 1000;
	next();
	//schedule();
}
PUBLIC void	sys_P(SEMAPHORE* sem,int index){
	sem->value--;
	if(sem->value<0){
		
		sem->queue[sem->in] = index;
		block();
		sem->in = (sem->in+1)%QUEUE_SIZE;	
	}
}
PUBLIC void sys_V(SEMAPHORE* sem){
	sem->value++;
	//disp_str("V operation\n");
	if(sem->value<=0){
		int index = sem->queue[sem->out];
		wakeup(index);
		sem->out = (sem->out+1)%QUEUE_SIZE;	
	}
}

void next()
{
	
	while (1)
	{
		p_proc_ready++;

		if (p_proc_ready >= proc_table + NR_TASKS)
			p_proc_ready = proc_table;

		if (p_proc_ready->ticks <= 0 && p_proc_ready->block != 1){
				//disp_str(p_proc_ready->p_name);
			
			break;
		}
	}
}

void block(){	
	//block操作
	p_proc_ready->block = 1;
	next();
}

void wakeup(int index){	
	//wakeup操作
	proc_table[index].block = 0;
	p_proc_ready = &proc_table[index];
}
void clear()
{
	disp_pos = 0;
        
        for(int i = 0; i < 80 * 25; i++)
                disp_str(" ");
        
        disp_pos = 0;
}
