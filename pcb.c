/************************************/
/* Progetto  SO 2019 - Rikaya       */
/* Gruppo lso19az18                 */
/* Ultimo agg: Lun 11/02 			*/
/************************************/

#define	HIDDEN static
#include "const.h"
#include "listx.h"
#include "types_rikaya.h"


// Scelte progettuali:
// - La lista pcbFree ha gli elementi inseriti in coda, non in testa

HIDDEN LIST_HEAD(pcbFree_h); //Macro di listx.h: inizializza la sentinella alla pcbFree. pcbFree_h è la sentinella della coda di pcb

/* DICHIARAZIONI
HIDDEN void initPcbs(void);
*/

/* PCB handling functions */

/************************************/
/* PCB free list handling functions */
/************************************/

/* 1 -	Inizializza la pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
		Questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati. */
HIDDEN void initPcbs(void){
	HIDDEN pcb_t pcbFree_table[MAXPROC]; //Array statico contenente MAX_PROC pcb_t
	for (int i=0; i<MAXPROC; i++){
		pcb_t* newPcb = &pcbFree_table[i]; //Mette in newPcp l'iesimo elemento della tabella dei pcb
		list_add_tail(&(newPcb->p_next), &(pcbFree_h)); 	//Aggiunge l'elemento in coda alla lista pcbFree
	}
}

/* 2 -	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree) */
HIDDEN void freePcb(pcb_t *p){
	list_add_tail(&(p->p_next), &(pcbFree_h));
}

/* 3 - 	Restituisce NULL se la pcbFree è vuota. Altrimenti rimuove un elemento dalla pcbFree,
		inizializza tutti i campi (NULL/0) e restituisce l’elemento rimosso. */
HIDDEN pcb_t *allocPcb(void){
	if (list_empty(&(pcbFree_h))) return NULL; //Macro di listx.h: se pcbFree è vuoto, restituisce NULL

	pcb_t* tempPcb = container_of(pcbFree_h.prev, pcb_t, p_next); //Restituisce puntatore all'ultimo elemento della pcbFree
	//pcb_t* elimPcb = tempPcb->p_next;
	list_del(&(tempPcb->p_next)); //Rimuove tempPcb dalla lista dei pcbFree: NON viene deallocato!

	//Inizializzazione di tutti i campi di tempPcb
	//Campi di tipo puntatore: imposto a NULL
	tempPcb->p_parent = NULL;
	tempPcb->p_semkey = NULL;
	//Campi di tipo p_state: A cosa lo imposto???????????? [AIUTO]
	//tempPcb->p_state = ??
	//Campi di tipo intero: imposto a 0
	tempPcb->priority = 0;
	//Campi di tipo list_head: uso INIT_LIST_HEAD
	INIT_LIST_HEAD(&(tempPcb->p_next));
	INIT_LIST_HEAD(&(tempPcb->p_child));
	INIT_LIST_HEAD(&(tempPcb->p_sib));

	return tempPcb;
}

/********************************/
/* PCB queue handling functions */
/********************************/

/* 4 - 	Inizializza la lista dei PCB, inizializzando l’elemento sentinella. */
HIDDEN void mkEmptyProcQ(struct list_head *head){
	//Abbiamo già il puntatore a list_head, quindi usiamo INIT_LIST_HEAD
	INIT_LIST_HEAD(head);
}


/* 5 - 	Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti. */
HIDDEN int emptyProcQ(struct list_head *head){
	return list_empty(head); //list_empty restituisce proprio questi valori.
}

/* 6 - 	Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
		L’inserimento deve avvenire tenendo conto della priorita’ di ciascun pcb (campo p->priority).
		La coda dei processi deve essere ordinata in base alla priorita’ dei PCB, in ordine decrescente
		(i.e. l’elemento di testa è l’elemento con la priorita’ più alta). */
HIDDEN void insertProcQ(struct list_head *head, pcb_t *p){
	pcb_t* i; //lo uso nel ciclo
	list_for_each_entry(i, head, p_next){
		if (p->priority > i->priority) {
			list_add(&(p->p_next), list_prev(&(i->p_next))); //Elemento precedente al Pcb dove voglio inserire p (In genere viene richiesta la sentinella e viene aggiunto in testa)
			//Se la priorità di p è maggiore di quella di i, p va inserito tra il precedente di i e i.
			/*__list_add(&(p->p_next), &(list_prev(i->p_next)), &(i->p_next)); //inserisco p nella lista head tra prev di i e i
			return; //Se arrivo qui, l'elemento l'ho già inserito;
			*/
		}
	}
	//La lista è vuota o p->priority è più basso di qualsiasi pcb presente nella lista.
	//Inserisco p in coda alla lista head
	list_add_tail(&(p->p_next), head);
}

/* 7 - 	Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO.
		Ritorna NULL se la coda non ha elementi. */
HIDDEN pcb_t *headProcQ(struct list_head *head){
	if (list_empty(head))
		return NULL;
	//Nella lista c'è almeno un elemento: restituisco il primo (priorità massima)
	pcb_t* tempPcb = container_of(pcbFree_h.next, pcb_t, p_next); //Restituisce puntatore al primo elemento della pcbFree (non lo rimuove!)
	return tempPcb;
}

/* 8 - 	Rimuove il primo elemento dalla coda dei processi puntata da head. Ritorna NULL se la coda è vuota.
		Altrimenti ritorna il puntatore all’elemento rimosso dalla lista. */
HIDDEN pcb_t *removeProcQ(struct list_head *head){
	if (list_empty(head))
		return NULL;
	//Nella lista c'è almeno un elemento: rimuovo e restituisco il primo (priorità massima)
	pcb_t* tempPcb = container_of(pcbFree_h.next, pcb_t, p_next); //Restituisce puntatore al primo elemento della pcbFree (non lo rimuove!)
	list_del(&(tempPcb->p_next)); //Rimuove tempPcb dalla lista head (NON lo dealloca!)
	return tempPcb;

}

/* 9 - 	Rimuove il PCB puntato da p dalla coda dei processi puntata da head. Se p non è presente nella coda,
		restituisce NULL. (NOTA: p può trovarsi in una posizione arbitraria della coda). */
HIDDEN pcb_t *outProcQ(struct list_head *head, pcb_t *p){
	pcb_t* i; //lo uso nel ciclo
	list_for_each_entry(i, head, p_next){
		if (p == i) {	//Ho trovato p in head: lo rimuovo e lo restituisco
			list_del(&(i->p_next));
			return p;
		}
	}
	return NULL; //Non ho trovato p in head: restituisco NULL
}

/***********************/
/* Tree view functions */
/***********************/

/* 10 -	restituisce TRUE se il PCB puntato da p non ha figli, restituisce FALSE altrimenti. */
HIDDEN int emptyChild(pcb_t *this){
	return list_empty(&(this->p_child)); //TRUE se p_child è vuota, FALSE altrimenti
}

/* 11 -	Inserisce il PCB puntato da p come figlio del PCB puntato da prnt. */
HIDDEN void insertChild(pcb_t *prnt, pcb_t *p){
	list_add_tail(&(p->p_sib),&(prnt->p_child)); 	//Inserisco p nella lista dei figli di prnt, 
													//ovvero inserisco p->p_sib alla lista che ha come sentinella prnt->p_child
	p->p_parent = prnt; //Aggiorno il campo p_parent di p a prnt.
}

/* 12 - Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL. */
HIDDEN pcb_t *removeChild(pcb_t *p){
	if(emptyChild(p)) //Controllo se PCB ha figli
		return NULL; //Se non ne ha restituisce NULL
	else {		//p ha figli: restituisco il primo
		//list_next(&(p->p_child)); //Primo figlio: è il next della sentinella (che punta a p_sib del primo figlio)

		//Devo ricavarmi il pcb_t che contiene list_primofiglio:
		pcb_t *primofiglio = container_of( list_next(&(p->p_child)) , pcb_t, p_sib); //Ho il pcb del primo figlio
		list_del(&(primofiglio->p_sib)); //Elimino primofiglio dalla lista puntata da p_child: ovvero elimino p_sib dalla lista in cui è.
		primofiglio->p_parent = NULL; //Aggiorno a NULL il campo p_parent di primofiglio
		return primofiglio; //Restituisco il puntatore al primo figlio di p (che ho appena tolto dalla lista dei figli di p).
	}
}
/* 13 - Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da p non ha un padre,
		restituisce NULL. Altrimenti restituisce l’elemento rimosso (cioè p). A differenza della removeChild,
		p può trovarsi in una posizione arbitraria (ossia non è necessariamente il primo figlio del padre). */
HIDDEN pcb_t *outChild(pcb_t *p){
	if(p->p_parent == NULL){ //Se p non ha un padre allora restituisco NULL
		return NULL;
	}
	else { 
		//p è sicuramente nella lista dei figli di p->p_parent, ovvero p->p_parent->p_child. 
		//In particolare p->p_sib farà parte della lista puntata da p->p_parent->p_child.
		//Basta togliere p->p_sib dalla lista in cui si trova!
		list_del(&(p->p_sib));
		p->p_parent = NULL; //p è diventato orfano: imposto a NULL il campo p_parent
		return p;
	}
}