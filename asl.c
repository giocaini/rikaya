/************************************/
/* Progetto  SO 2019 - Rikaya       */
/* Gruppo lso19az18                 */
/* Ultimo agg: Gio 07/02 			*/
/************************************/

#include "asl.h"
#include "pcb.h"

/**************************/
/* ASL handling functions */
/**************************/

HIDDEN LIST_HEAD(semd_h); 		//inizializzazione della sentinella della lista ASL a semd_h
								//semd_h è la sentinella della lista ASL (lista dei semd attivi)

//HIDDDEN INIT_LIST_HEAD(semd_h);	//non so se sia necessaria --> E' "inclusa in LIST_HEAD, allora forse no

HIDDEN LIST_HEAD(semdFree_h);	//semdFree_h è la sentinella della lista semdFree (lista dei semd liberi)

/* 14 - Restituisce il puntatore al SEMD nella ASL la cui chiave è pari a key.
		Se non esiste un elemento nella ASL con chiave eguale a key, viene restituito NULL.*/
HIDDEN semd_t* getSemd(int *key){

	if (list_empty(&semd_h)) return NULL; 		//se ASL è vuota -> NULL
	semd_t* i; //per ciclare sulla lista ASL
	list_for_each_entry(i, &semd_h, s_next){		//ciclo sulla lista ASL partendo da sentinella semd_h
		if( key == i->s_key) return i;
			//container_of(semd_h.next , semd_t, s_next);
			//semd_h.next non va bene, ritorna sempre l'elemento successivo alla sentinella
			//### Non si può usare i.s_key perchè i è un puntatore: quindi i->s_key
		if(&(i->s_next) == &semd_h) return NULL;
		//Se arrivo a un elemento che ha come elemento successivo la sentinella allora sei arrivato all'ultimo e pui smettere di cercare
		else return NULL;
	}
}

/* 15 -	Viene inserito il PCB puntato da p nella coda dei processi bloccati associata
        al SEMD con chiave key. Se il semaforo corrispondente non è presente nella ASL,
        alloca un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce
        nella ASL, settando i campi in maniera opportuna (i.e. key e s_procQ). Se non 
        è possibile allocare un nuovo SEMD perché la lista di quelli liberi è vuota,
        restituisce TRUE. In tutti gli altri casi, restituisce FALSE. */
int insertBlocked(int *key, pcb_t* p){
	
	semd_t* semd =  getSemd(key); //### La getSemd vuole un puntatore ad intero, quindi si può mettere key, non &key

	//caso 1: c'è il SEMD corrispondente alla key
	if (semd != NULL){
		insertProcQ( &(semd->s_procQ) , p);	//aggiunge il PCB nella lista in base alla sua priorità
							//### p al posto di &p
						//e se è un albero? cosa ne sa SEMD se punta a una lista o ad un albero???
						//### SEMD è sempre un semaforo (che fa parte di una lista)
						//### p è un processo: non ci interessa sapere se p fa parte di un albero
		return FALSE;
	}

	else{	//semd == NULL quindi abbiamo 2 possibili casi
		//caso 3: SEMD non presente nella ASL && lista semdFree è vuota
		if(list_empty(&semdFree_h)) return TRUE; //se 1-> lista vuota, necessario specificare la condizione if? perchè secondo me no. si invece
							 //### cancellata condizione == 1
		//caso 2: SEMD non presente nella ASL -> prendo SEMD dalla lista semdFree
		//semd_t *new_semd = list_next( &semdFree_h );	//restituisce il primo elemento della lista semdFree
 		//### list_next dà un elemento di tipo head_list non semd_t: bisogna usare la container_of
		semd_t *new_semd = container_of(semdFree_h.next , semd_t, s_next);
		//### da qui in poi si riparla di liste, quindi al posto di new_semd ho messo new_semd->s_next
		list_del(&(new_semd->s_next));					//elimino new_semd dalla lista semdFree
		list_add_tail(&(new_semd->s_next), &semd_h);	//aggiunge semd alla coda ASL
		new_semd->s_key = key;							//assegno key
			//### ho tolto la & da &key
		mkEmptyProcQ( &(new_semd->s_procQ) );			//creo lista di pcb puntata da s_procQ
		insertProcQ( &(semd->s_procQ) , p);	
		return FALSE;
	}
	 
}

/* 16 -	Ritorna il primo PCB dalla coda dei processi bloccati (s_ProcQ) associata al SEMD 
		della ASL con chiave key. Se tale descrittore non esiste nella ASL, restituisce NULL.
		Altrimenti, restituisce l’elemento rimosso. Se la coda dei processi bloccati per
		il semaforo diventa vuota, rimuove il descrittore corrispondente dalla ASL e lo 
		inserisce nella coda dei descrittori liberi (semdFree). */
pcb_t* removeBlocked(int *key){

	semd_t* semd = getSemd(key);

	//caso 1: SEMD non esiste nella ASL
	if(semd == NULL) return NULL;

	//caso 2: SEMD esiste -> ora abbiamo 3 possibili casi

	//caso 2.A : c'è SEMD ma non ha PCB
	//### Caso impossibile: se SEMD è nella ASL deve avere almeno un PCB! Quindi commento l'else if
	//->leggendo le specifiche della funzione 18, questo è un caso possibile. Anche se dopo qualche giro diventa inutile 
	//else if(list_empty( &(semd->s_procQ))) return NULL; //il semd non ha processi, se lista pcb è vuota
	/* elimina il SEMD dato che non ha processi?? */
	
	//caso 2.B: ritorna il primo PCB dalla coda dei processi bloccati 
	pcb_t* pcb_tmp = removeProcQ( &(semd->s_procQ) );	//rimuovo il primo pcb dalla coda

	//caso 2.C: controlli per vedere se eliminare semd da ASL
	if( list_empty( &(semd->s_procQ) )) { 		//controllo se la coda è vuota, 1->vuota
		list_del(&(semd->s_next));	//è vuota => elimino SEMD da ASL
		list_add(&(semd->s_next), &(semdFree_h));		//aggiungo semd alla lista semdFree (indifferente se in testa o in coda)
		//### Sostituito semd con semd->s_next
	}
	return pcb_tmp;		//ritorna il puntatore al primo processo bloccato del semd
}

/* 17 -	Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (indicato da 
		p->p_semKey). Se il PCB non compare in tale coda, allora restituisce NULL (condizione di errore).
		Altrimenti, restituisce p.*/
pcb_t* outBlocked(pcb_t *p){
	
	int *key = p->p_semkey;	//assegno la chiave del pcb ad un valore int* (passaggio anche inutile)
	semd_t* semd = getSemd(key);	//recupero il SEMD corrispondente alla key
	pcb_t* pcb = outProcQ( &(semd->s_procQ), p);	//elimina il PCB puntato da p, nella coda puntata da semd->s_procQ

	return pcb;
}

/* 18 -	Restituisce (senza rimuovere) il puntatore al PCB che si trova in testa alla coda dei
		processi associata al SEMD con chiave key. Ritorna NULL se il SEMD non compare nella ASL oppure
		se compare ma la sua coda dei processi è vuota. */
pcb_t* headBlocked(int *key){

	semd_t* semd =  getSemd(key);		//prendo il SEMD associato alla key

	if(semd == NULL) return NULL;			//SEMD non compare nella ASL (NULL)
	
	pcb_t* pcb = headProcQ( &(semd->s_procQ) );
	//if(pcb == NULL ) return NULL;	//coda dei processi è vuota (ritorna NULL) -> caso impossibile
	return pcb;				//primo processo nella lista s_procQ (ritnorna puntatore)
											
}


/* 19 - Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (indicato da
		p->p_semKey). Inoltre, elimina tutti i processi dell’albero radicato in p (ossia tutti
		i processi che hanno come avo p) dalle eventuali code dei semafori su cui sono bloccati.*/
void outChildBlocked(pcb_t *p){
	pcb_t * pcb = outBlocked(p); //Tolgo il pcb dalla coda del semd su cui è bloccato
	if (pcb == NULL) return; //ERRORE!! 
	//Ho sistemato la 142: c'era empty_child al posto di emptyCHild
	if (emptyChild(p)) return; //Caso base: pcb non ha figli --> finisce
	else {	//Pcb ha dei figli: per ogni figlio richiamo la outChildBlocked ricorsivamente
		pcb_t * i; //Elemento del ciclo
		list_for_each_entry(i, &(pcb->p_child), p_sib) {
			outChildBlocked(i);
		}
	}
}
/*
	pcb_t * pcb = outBlocked(p);
	if (pcb != NULL) {
		//## Dovevo scappare: si fa abbastanza facilmente, ma outCHildBlocked deve diventare ricorsiva!
		//## Ci guardo dopo!
	}
}
*/

/* 20 -	Inizializza la lista dei semdFree in modo da contenere tutti gli elementi della semdTable.
		Questo metodo viene invocato una volta sola durante l’inizializzazione della struttura dati. */
void initASL(void){

	HIDDEN semd_t semdFree_table[MAXPROC]; //Array statico contenente MAX_PROC semd_t


	for(int i = 0; i < MAXPROC; i++){
		semd_t* newSemd = &(semdFree_table[i]); //Mette in newPcp l'iesimo elemento della tabella dei pcb
		list_add_tail( &(newSemd->s_next), &semdFree_h);	//aggiungo l'i-esimo elemento della semdTable in coda
	}
}