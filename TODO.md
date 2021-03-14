- Trovare possibili sezioni di codice da poter eseguire in parallelo (stando attenti alle variabili condivise)

Problemi trovati:
- (RISOLTO) Inizialmente start e goal sono rispettivamente in posizione (0,0) e (ROW-1, COL-1). Se questi valori vengono cambiati può capitare che con la generazione randomica un ostacolo si trovi sopra start/goal. O lo lasciamo cosi per far vedere che prima di eseguire l'algoritmo facciamo un controllo per verificare che start/goal siano liberi, oppure possiamo fare in modo che con la generazione randomica gli ostacoli non si possano trovare nella posizione di start/goal (tenendo comunque in considerazione il primo punto di questa lista, ovvero se ho il 100% di ostacoli allora anche su start/goal ci devono essere ostacoli).
- (RISOLTO) Con OBSTACLES = 0 e CONN = 4, il percorso non è ottimale!
- (RISOLTO) initCells() : usare numeri molto alti oppure una variabile per indicare il primo aggiornamento di f,g,h? Stessa cosa per i parents della cella start --> si potrebbe usare NULL ? CERCARE DI OTTIMIZZARE LA FUNZIONE!
- (RISOLTO) RIGA 292: se f < f, oppure se ( f == f && h < h )

CONSIDERAZIONI:
- seme costante per rand() in modo da poter fare considerazioni sulle performance usando sempre la stessa mappa
- la stampa di mappa/celle costa un sacco di tempo!