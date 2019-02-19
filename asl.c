/************************************/
/* Progetto  SO 2019 - Rikaya       */
/* Gruppo lso19az18                 */
/* Ultimo agg: Gio 07/02 			*/
/************************************/

/**************************/
/* ASL handling functions */
/**************************/

HIDDEN LIST_HEAD(semd_h); 		//inizializzazione della sentinella della lista ASL a semd_h
								//semd_h è la sentinella della lista ASL (lista dei semd attivi)

HIDDDEN INIT_LIST_HEAD(semd_h);	//non so se sia necessaria

HIDDEN LIST_HEAD(semdFree_h);	//semdFree_h è la sentinella della lista semdFree (lista dei semd liberi)

/* 14 - Restituisce il puntatore al SEMD nella ASL la cui chiave è pari a key.
		Se non esiste un elemento nella ASL con chiave eguale a key, viene restituito NULL.*/
HIDDEN semd_t* getSemd(int *key){

	if list_empty(semd_h) return NULL; 		//se ASL è vuota -> NULL

	semd_t* i; //per ciclare sulla lista ASL
	list_for_each_entry(i, semd_h, s_next){		//ciclo sulla lista ASL partendo da sentinella semd_h
		if( key == i.s_key) return container_of(semd_h.next , semd_t, s_next);
	}
	/*da finire - evitare che cicli all'infinito, nel caso in cui key non sia presente*/
}

/* 15 -	Viene inserito il PCB puntato da p nella coda dei processi bloccati associata
        al SEMD con chiave key. Se il semaforo corrispondente non è presente nella ASL,
        alloca un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce
        nella ASL, settando i campi in maniera opportuna (i.e. key e s_procQ). Se non 
        è possibile allocare un nuovo SEMD perché la lista di quelli liberi è vuota,
        restituisce TRUE. In tutti gli altri casi, restituisce FALSE. */
int insertBlocked(int *key, pcb_t* p){
	
	semd_t* semd =  getSemd(&key);

	//caso 1: c'è il SEMD corrispondente alla key
	if (semd != NULL){
		insertProcQ( &(semd->s_procQ) , &p);	//aggiunge il PCB nella lista in base alla sua priorità
						//e se è un albero? cosa ne sa SEMD se punta a una lista o ad un albero???
		return FALSE;
	}
	//caso 2: SEMD non presente nella ASL -> prendo SEMD dalla lista semdFree
	else if( /*condizione*/ ) {
		//da fare AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
		return FALSE;
	}
	//caso 3: lista semdFree è vuota
	else if( list_empty(&semdFree_h) == 1 ) return TRUE; //se 1-> lista vuota, necessario specificare la condizione if? perchè secondo me no

	 
}

/* 16 -	Ritorna il primo PCB dalla coda dei processi bloccati (s_ProcQ) associata al SEMD 
		della ASL con chiave key. Se tale descrittore non esiste nella ASL, restituisce NULL.
		Altrimenti, restituisce l’elemento rimosso. Se la coda dei processi bloccati per
		il semaforo diventa vuota, rimuove il descrittore corrispondente dalla ASL e lo 
		inserisce nella coda dei descrittori liberi (semdFree). */
pcb_t* removeBlocked(int *key){

	semd_t* semd = getSemd(&key);
	//caso 1: descrittore non esiste nella ASL
	if(semd == NULL) return NULL;
	
	//caso 1.5: c'è SEMD ma non ha PCB
	else if( list_empty( &(semd->s_procQ) ) == 1 ) return NULL; //il semd non ha processi, se lista pcb è vuota
	// -> ma in teoria non dovrebbe succedere, credo

	//caso 2: ritorna il primo PCB dalla coda dei processi bloccati 
	pcb_t* pcb_tmp = removeProcQ( &(semd->s_procQ) );	//rimuovo il primo pcb dalla coda
	if( list_empty( &(semd->s_procQ) ) == 1 ){ 		//controllo se la coda sia vuota, 1->vuota
		list_del( &semd );	//è vuota => elimino SEMD da ASL
	}

	return pcb_tmp;		//ritorna il puntatore al primo processo bloccato del semd
}

/* 17 -	Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (indicato da 
		p->p_semKey). Se il PCB non compare in tale coda, allora restituisce NULL (condizione di errore).
		Altrimenti, restituisce p.*/
pcb_t* outBlocked(pcb_t *p){
	
	int *key = p->p_semkey;	//assegno la chiave del pcb ad un valore int* (passaggio anche inutile)
	semd_t* semd = getSemd(&key);	//recupero il SEMD corrispondente alla key
	pcb_t* pcb = outProcQ( &(semd->s_procQ), &p);	//elimina il PCB puntato da p, nella coda puntata da semd->s_procQ

	return pcb;
}

/* 18 -	Restituisce (senza rimuovere) il puntatore al PCB che si trova in testa alla coda dei
		processi associata al SEMD con chiave key. Ritorna NULL se il SEMD non compare nella ASL oppure
		se compare ma la sua coda dei processi è vuota. */
pcb_t* headBlocked(int *key){

	semd_t* semd =  getSemd(&key);		//prendo il SEMD associato alla key

	if(semd == NULL) return NULL;			//SEMD non compare nella ASL (NULL)
	
	pcb_t* pcb = headProcQ( &(semd->s_procQ) );
	if(pcb == NULL ) return NULL;	//coda dei processi è vuota (ritorna NULL)
	else return pcb;				//primo processo nella lista s_procQ (ritnorna puntatore)
											
}

/* 19 - Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (indicato da
		p->p_semKey). Inoltre, elimina tutti i processi dell’albero radicato in p (ossia tutti
		i processi che hanno come avo p) dalle eventuali code dei semafori su cui sono bloccati. */
void outChildBlocked(pcb_t *p){

}

/* 20 -	Inizializza la lista dei semdFree in modo da contenere tutti gli elementi della semdTable.
		Questo metodo viene invocato una volta sola durante l’inizializzazione della struttura dati. */
void initASL(void){

	//ma l'elemento semdTable[MAXPROC] esiste già o la dobbiamo creare noi?

	INIT_LIST_HEAD(semdFree_h);		//inizializza i campi, con entrambi i campi che puntano a se stessa)
	for(int i = 0; i < MAXPROC; i++){
		list_add_tail( &(semd_table[i]), &semdFree_h);	//aggiungo l'i-esimo elemento della semdTable in coda
	}

}

