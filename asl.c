/************************************/
/* Progetto  SO 2019 - Rikaya       */
/* Gruppo lso19az18                 */
/* Ultimo agg: Gio 07/02 			*/
/************************************/

/**************************/
/* ASL handling functions */
/**************************/

HIDDEN LIST_HEAD(semd_h); 	//inizializzazione della sentinella della lista ASL a semd_h
							//semd_h è la sentinella della lista ASL (lista dei semd attivi)

/* 14 - Restituisce il puntatore al SEMD nella ASL la cui chiave è pari a key.
		Se non esiste un elemento nella ASL con chiave eguale a key, viene restituito NULL.*/
HIDDEN semd_t* getSemd(int *key){

	if list_empty(semd_h)
		return NULL;

	semd_t* i; //per ciclare sulla lista ASL
	list_for_each_entry(i, semd_h, s_next){		//ciclo sulla lista ASL partendo da sentinella semd_h
		if( key == i.s_key) return container_of(semd_h.next , semd_t, s_next);
	}
	/*da finire - evitare che cicli all'infinito*/
}

/* 15 -	Viene inserito il PCB puntato da p nella coda dei processi bloccati associata
        al SEMD con chiave key. Se il semaforo corrispondente non è presente nella ASL,
        alloca un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce
        nella ASL, settando I campi in maniera opportuna (i.e. key e s_procQ). Se non 
        è possibile allocare un nuovo SEMD perché la lista di quelli liberi è vuota,
        restituisce TRUE. In tutti gli altri casi, restituisce FALSE. */
void initASL(){

}

/* 16 -	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree) */
int insertBlocked(int *key,pcb_t* p){

}

/* 17 -	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree) */
pcb_t* removeBlocked(int *key){

}

/* 18 -	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree) */
pcb_t* outBlocked(pcb_t *p){

}

/* 19 -	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree) */
pcb_t* headBlocked(int *key){

}

/* 20 -	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree) */
void outChildBlocked(pcb_t *p){

}

#endif
