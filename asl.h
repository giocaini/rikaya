#ifndef ASL_H
#define ASL_H
#include <umps/types.h>
#include "types_rikaya.h"
#include "pcb.h"

/* ASL handling functions 14 - 20 */
HIDDEN semd_t* getSemd(int *key);
HIDDEN void initASL();

HIDDEN int insertBlocked(int *key,pcb_t*p);
HIDDEN pcb_t* removeBlocked(int *key);
HIDDEN pcb_t* outBlocked(pcb_t *p);
HIDDEN pcb_t* headBlocked(int *key);
HIDDEN void outChildBlocked(pcb_t *p);

#endif
