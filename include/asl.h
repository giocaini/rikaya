#ifndef ASL_H
#define ASL_H
#include <umps/types.h>
#include "types_rikaya.h"
#include "pcb.h"

/* ASL handling functions 14 - 20 */
semd_t* getSemd(int *key);
int insertBlocked(int *key,pcb_t*p);
pcb_t* removeBlocked(int *key);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int *key);
void outChildBlocked(pcb_t *p);
void initASL();

#endif
