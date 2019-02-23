#ifndef PCB_H
#define PCB_H
#include <umps/types.h>
#include "types_rikaya.h"

/* PCB allocation deallocation functions 1 - 3 */
void initPcbs(void);
void freePcb(pcb_t *p);
pcb_t *allocPcb(void);

/* PCB queue handling functions 4 - 9*/
void mkEmptyProcQ(struct list_head *head);
int emptyProcQ(struct list_head *head);
void insertProcQ(struct list_head *head, pcb_t *p);
pcb_t *headProcQ(struct list_head *head);

pcb_t *removeProcQ(struct list_head *head);
pcb_t *outProcQ(struct list_head *head, pcb_t *p);


/* PCB tree handling functions 10 - 13 */
int emptyChild(pcb_t *this);
void insertChild(pcb_t *prnt, pcb_t *p);
pcb_t *removeChild(pcb_t *p);
pcb_t *outChild(pcb_t *p);

#endif
