/************************************/
/* Progetto  SO 2019 - Rikaya       */
/* Gruppo lso19az18                 */
/* Ultimo agg: Gio 07/02 			*/
/************************************/


LIST_HEAD(pcbFree_h); //Macro di listx.h: inizializza la sentinella alla pcbFree. pcbFree_h è la sentinella della coda di pcb

/* PCB handling functions */

/************************************/
/* PCB free list handling functions */
/************************************/

/* 1 -	Inizializza la pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
		Questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati. */
void initPcbs(void){
	HIDDEN pcb_t pcbFree_table[MAXPROC]; //Array statico contenente MAX_PROC pcb_t
	for (int i=0; i<MAXPROC; i++){
		pcb_t* newPcb = &pcbFree_table[i]; //Mette in newPcp l'iesimo elemento della tabella dei pcb
		list_add_tail(&(newPcb->p_next), &(pcbFree_h)) 	//Aggiunge l'elemento in coda alla lista pcbFree
														//NB p_next è il campo di pcb_t che permette di passare da un elemento al successivo nella lista
	}
}

/* 2 -	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree) */
void freePcb(pcb_t *p){
	list_add_tail(&(p->p_next), &(pcbFree_h));
}

/* 3 - 	Restituisce NULL se la pcbFree è vuota. Altrimenti rimuove un elemento dalla pcbFree,
		inizializza tutti i campi (NULL/0) e restituisce l’elemento rimosso. */
pcb_t *allocPcb(void){
	if (list_empty(&(pcbFree_h))) return NULL; //Macro di listx.h: se pcbFree è vuoto, restituisce NULL

	pcb_t* tempPcb = container_of(pcbFree_h.prev, pcb_t, p_next); //Restituisce puntatore all'ultimo elemento della pcbFree
	list_del(tempPcb); //Rimuove tempPcb dalla lista dei pcbFree: NON viene deallocato!

	//Inizializzazione di tutti i campi di tempPcb
	//Campi di tipo puntatore: imposto a NULL
	tempPcb->p_parent = NULL;
	tempPcb->p_semkey = NULL;
	//Campi di tipo p_state: A cosa lo imposto????????????
	tempPcb->p_state = ???
	//Campi di tipo intero: imposto a 0
	tempPcb->priority = 0;
	//Campi di tipo list_head: uso INIT_LIST_HEAD
	INIT_LIST_HEAD(tempPcb->p_next);
	INIT_LIST_HEAD(tempPcb->p_child);
	INIT_LIST_HEAD(tempPcb->p_sib);

	return tempPcb;
}

/********************************/
/* PCB queue handling functions */
/********************************/

/* 4 - 	Inizializza la lista dei PCB, inizializzando l’elemento sentinella. */
void mkEmptyProcQ(struct list_head *head){
	//Abbiamo già il puntatore a list_head, quindi usiamo INIT_LIST_HEAD
	INIT_LIST_HEAD(head);
}


/* 5 - 	Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti. */
int emptyProcQ(struct list_head *head){
	return list_empty(head); //list_empty restituisce proprio questi valori.
}

/* 6 - 	Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
		L’inserimento deve avvenire tenendo conto della priorita’ di ciascun pcb (campo p->priority).
		La coda dei processi deve essere ordinata in base alla priorita’ dei PCB, in ordine decrescente
		(i.e. l’elemento di testa è l’elemento con la priorita’ più alta). */
void insertProcQ(struct list_head *head, pcb_t *p){
	pcb_t* i; //lo uso nel ciclo
	list_for_each_entry(i, head, p_next){
		if (p->priority > i->priority) { //Se la priorità di p è maggiore di quella di i, p va inserito tra il precedente di i e i.
			__list_add(&(p->p_next), &(i->p_next->prev), &(i->p_next)); //inserisco p nella lista head tra prev di i e i
			return; //Se arrivo qui, l'elemento l'ho già inserito;
		}
	}
	//La lista è vuota o p->priority è più basso di qualsiasi pcb presente nella lista.
	//Inserisco p in coda alla lista head
	list_add_tail(&(p->p_next), head);
}

/* 7 - 	Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO.
		Ritorna NULL se la coda non ha elementi. */
pcb_t *headProcQ(struct list_head *head){
	if list_empty(head)
		return NULL;
	//Nella lista c'è almeno un elemento: restituisco il primo (priorità massima)
	pcb_t* tempPcb = container_of(pcbFree_h.next, pcb_t, p_next); //Restituisce puntatore al primo elemento della pcbFree (non lo rimuove!)
	return tempPcb;
}

/* 8 - 	Rimuove il primo elemento dalla coda dei processi puntata da head. Ritorna NULL se la coda è vuota.
		Altrimenti ritorna il puntatore all’elemento rimosso dalla lista. */
pcb_t *removeProcQ(struct list_head *head){
	if list_empty(head)
		return NULL;
	//Nella lista c'è almeno un elemento: rimuovo e restituisco il primo (priorità massima)
	pcb_t* tempPcb = container_of(pcbFree_h.next, pcb_t, p_next); //Restituisce puntatore al primo elemento della pcbFree (non lo rimuove!)
	list_del(&(tempPcb->p_next)); //Rimuove tempPcb dalla lista head (NON lo dealloca!)
	return tempPcb;

}

/* 9 - 	Rimuove il PCB puntato da p dalla coda dei processi puntata da head. Se p non è presente nella coda,
		restituisce NULL. (NOTA: p può trovarsi in una posizione arbitraria della coda). */
pcb_t *outProcQ(struct list_head *head, pcb_t *p){
	pcb_t* i; //lo uso nel ciclo
	list_for_each_entry(i, head, p_next){
		if (p == i) {//Ho trovato p in head: lo rimuovo e lo restituisco
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
int emptyChild(pcb_t *this){
	if(this->p_child == NULL) //non so se il figlio viene dichiarato null se vuoto
		return FALSE;	//In questo caso restituisco false
	else
		return TRUE; // Altrimenti restituisco true
}
/* 10 by Gio:
p_child è una sentinella di una lista, quindi non è NULL se è vuota.
La funzione 4 inizializza un PCB e anche i suoi campi, tra cui p_child:
chiama infatti la funzione INIT_LIST_HEAD(tempPcb->p_child) che è in listx.h e che ti ho copiato qui.
		static inline void INIT_LIST_HEAD(struct list_head *list) {
			list->next = list;
			list->prev = list;
		}
(Le liste di listx sono cicliche, quindi se la lista è vuota la sentinella e i suoi campi next e prev puntano tutti alla stessa cosa.)
In definitiva, p_child è una sentinella di una lista:
per verificare se this non ha figli, ovvero se p_child è una lista vuota, conviene usare la funzione
list_empty (che è sempre di listx.h).
list_empty di una variabile "list_head" ti dice se la lista "puntata" da "list_head" è vuota,

*/
int emptyChild(pcb_t *this){
	if (list_empty(this->p_child)) //La lista dei figli di this è vuota --> non ha figli
		return TRUE;	//Non ha figli
	else
		return FALSE; // Ha figli
}

/* 11 -	Inserisce il PCB puntato da p come figlio del PCB puntato da prnt. */
void insertChild(pcb_t *prnt, pcb_t *p){ //Qua non ho proprio capito cosa vuole che faccia ma credo così
	pcb_t *figlio = prnt->p_child; //Ottengo la lista dei figli di prnt
	list_add_tail(&(p),&(figlio)); //Aggiungo p nella lista dei figli di prnt, sincremante non so a cosa serva il & in questo caso ma l'ho messo comunque

}

/* 11 -	by Gio
	ti passa due pcb (prnt e p) e devi aggiungere p alla lista dei figli di prnt.
	p_child è una sentinella di una lista (tipo head_list), non pcb_t. Da qui il caos... xD
	
	Da qui il commento si è trasformato in una dissertazione su ste benedette liste di listx.
	Provo a spiegarmi, ma non è semplice!
	list_head è una struttura con due campi (prev e next) che ha il solo scopo di creare liste di altre strutture.
	pcb_t è una struttura con tanti dati che riguarderanno quel determinato processo.
	Come fare per creare una lista di pcb_t?
	1) 	Modo classico: aggiungo i classici campi next e prev alla struttura di pcb_t. next e prev sono puntatori ad altri oggetti pcb_t: NULL se è l'ultimo della lista. 
		Mi devo salvare da qualche parte però il punt. al pcb_t iniziale, chiamiamolo p: con quello, ciclando p=p->next finchè p!=NULL, mi scorro tutta la lista.
		Non è funzionale, per vari motivi: primo fra tutti il fatto che è necessario avere sempre almeno un pcb_t, quello iniziale.
		Se ho i pcb p1, p2 e p3, li collego direttamente con next e prev. 
		Si avrà:
			p1->next = p2
			p2->prev = p1
			p2->next = p3
			p3->prev = p2
	2)	Modo list_head: uso una struttura in più, la list_head, definita così:
		struct list_head {
			struct list_head *next, *prev;
		};
		Ovvero, una struttura che ha solo due campi, entrambi puntatori ad altre strutture list_head.
		Nota: la list_head è proprio una lista definita nel modo classico!
		Come collego però i pcb_t tra loro?
			Aggiungo alla struttura dei pcb_t un puntatore ad una struttura list_head e lo chiamo p_next.
			Per collegare i pcb p1, p2 e p3, collego i loro p_next: 
				p1->p_next->next = p2->p_next
				p2->p_next->prev = p1->p_next
				p2->p_next->next = p3->p_next
				p3->p_next->prev = p2->p_next
		Come accedo alla lista?
			Creo un oggetto di tipo list_head (chiamiamolo list_h) che avrà la sola funzione di sentinella per accedere alla lista.
			Quando aggiungo il primo elemento pcb_t alla lista (chiamiamolo p), metto: list_h->next = p->p_next.
		Come scorro la lista?
			Scorro la lista dei p_next (che sono di tipo list_head), similmente al modo "classico", partendo dalla sentinella list_h). 
			Ma c'è un problema: se scansiono i p_next come faccio dal p_next a risalire al pcb_t di cui fa parte?
			Uso un'altra bellissima macro di listx, complessissima da capire:
				container_of(ptr, type, member), che restituisce il puntatore all'oggetto che contiene un determinato list_head.
			Supponiamo di chiamare p_next_temp il p_next che sto considerando attualmente mentre scansiono la lista dei p_next.
			Se voglio avere il puntatore al pcb_t p che contiene p_next_temp dovrò usare:
				pcb_t *p = container_of(p_next_temp, pcb_t, p_next) dove:
					p_next_temp è il puntatore di cui voglio trovare il "contenitore"
					pbc_t è il tipo della struttura "contenitore"
					p_next è il nome del campo all'interno della struttura "contenitore" a cui corrisponde p_next_temp	
		
	uahdihdasihadshflkdajhfjklahfkjhdsjkasd,kjfhliadhlkjhljkhadkjhaKJDHLQEHFhjkahsdfkjlhafljkhajldfhjkahfjahfjlhadlfjhjla
	Scrivo cose a caso, perchè non penso di essere stato troppo chiaro prima e quindi non arriverete a leggere fino a qui! :D
	
	#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!
	#	DA NOTARE MOLTO BENE!! 
	#	p->next collega le code di processi.
	#	I figli di uno stesso padre sono collegati tra loro invece da p->sib (che funziona nello stesso medo di p->next).
	#	Mentre la sentinella di p->next me la sono salvata all'inizio,
	#	la sentinella della lista dei figli di uno stesso padre (che sono fratelli, da cui p->sib) è salvata dentro ogni pcb_t in p_child!
	#	Quindi:
	#	- p_next serve solo per la coda di processi!
	#		la sua sentinella è salvata fuori da tutto in pcbFree_h.
	#	- p_sib serve per collegare i fratelli tra loro
	#		la sua sentinella è salvata in ogni pcb padre in p_child 
	#
	#	PARTENDO DA QUESTA COSA, tutti i p_next da ora in poi li ho sostituiti con p_sib!
	
*/
void insertChild(pcb_t *prnt, pcb_t *p){
	list_head *figlio = prnt->p_child; //Ottengo la lista dei figli di prnt: è di tipo list_head
	list_add_tail(&(p->p->sib),&(figlio)); 	//list_add_tail prende in input due list_head e inserisce il primo alla lista identifica dal secondo.
											//Abbiamo detto che ciè che collega i pcb nelle liste p_child di pcb_t è il campo p->sib.
											//Quindi uso p->p_sib al posto di p. (p sarebbe di tipo pcb_t non list_head).
}

/* 12 - Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL. */
pcb_t *removeChild(pcb_t *p){
	if(emptyChild(p)) //Controllo se PCB ha figli
		return NULL; //Se non ne ha restituisce NULL
	else
		removeProcQ(p->p_child); //Se ha figli allora rimuovo il figlio in testa alla lista di figli
}


/* 12 - by Gio 
	La prima parte OK. Nella seconda c'è un problema: removeProcQ rimuove i pcb dalla coda dei processi, non dalla lista dei figli.
	Usa quindi p_next non p_sib, quindi non si può usare qui!

*/
pcb_t *removeChild(pcb_t *p){
	if(emptyChild(p)) //Controllo se PCB ha figli
		return NULL; //Se non ne ha restituisce NULL
	else {		//p ha figli: restituisco il primo
		list_head *figli = p->p_child; //Sentinella della lista dei figli
		list_head *list_primofiglio = figli->next; //Primo figlio: è il next della sentinella (che punta a p_sib del primo figlio)
		//Devo ricavarmi il pcb_t che contiene list_primofiglio:
		pcb_t *primofiglio = container_of(list_primofiglio, pcb_t, p_sib); //Ho il pcb del primo figlio
		list_del(&(p->p_sib)); //Elimino p dalla lista puntata da p_child: ovvero elimino p_sib dalla lista in cui è.
		return primofiglio; //Restituisco il puntatore al primo figlio di p (che ho appena tolto dalla lista dei figli di p).
	}
}
/* 13 - Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da p non ha un padre,
		restituisce NULL. Altrimenti restituisce l’elemento rimosso (cioè p). A differenza della removeChild,
		p può trovarsi in una posizione arbitraria (ossia non è necessariamente il primo figlio del padre). */
pcb_t *outChild(pcb_t *p){
	if(p->p_parent== NULL){ //Se p non ha un padre allora restituisco NULL
		return NULL;
	}
	else{
		pcb_t *padre= p->p_parent; //Altrimenti assegno a padre il padre di p
		list_for_each_entry(i, padre->p_child, p_sib){ //Scorro tutti i figli del padre fino a trovare quello puntato da p
			if (p == i) {//Ho trovato p in head: lo rimuovo e lo restituisco
				list_del(&(i->p_sib));
				return p;
			}
		}
	}
}

/* 13 by Gio 
	La tua va bene: manca solo la definizione di i (pcb_t* i) e in realtà non puoi usare p_next
	In questo caso però io so già che se p ha come parent padre, sicuramente p sarà nella lista dei figli del padre (padre->p_child).
	Quindi in realtà, se p->parent è diverso da NULL, basta semplicemente eliminare p dalla p->p_parent->p_child
*/
pcb_t *outChild(pcb_t *p){
	if(p->p_parent == NULL){ //Se p non ha un padre allora restituisco NULL
		return NULL;
	}
	else { 
		//p è sicuramente nella lista dei figli di p->p_parent, ovvero p->p_parent->p_child. 
		//In particolare p->p_sib farà parte della lista puntata da p->p_parent->p_child.
		//Basta togliere p->p_sib dalla lista in cui si trova!
		list_del(&(p->p_sib);
		return p;
	}
}