#ifndef PCB_H
#define PCB_H
#include <umps/types.h>
#include "types_rikaya.h"
#include "const.h"

/* PCB allocation deallocation functions 1 - 3 */
HIDDEN void initPcbs(void);
HIDDEN void freePcb(pcb_t *p);
HIDDEN pcb_t *allocPcb(void);

/* PCB queue handling functions 4 - 9*/
HIDDEN void mkEmptyProcQ(struct list_head *head);
HIDDEN int emptyProcQ(struct list_head *head);
HIDDEN void insertProcQ(struct list_head *head, pcb_t *p);
HIDDEN pcb_t *headProcQ(struct list_head *head);

HIDDEN pcb_t *removeProcQ(struct list_head *head);
HIDDEN pcb_t *outProcQ(struct list_head *head, pcb_t *p);


/* PCB tree handling functions 10 - 13 */
HIDDEN int emptyChild(pcb_t *this);
HIDDEN void insertChild(pcb_t *prnt, pcb_t *p);
HIDDEN pcb_t *removeChild(pcb_t *p);
HIDDEN pcb_t *outChild(pcb_t *p);

#endif
