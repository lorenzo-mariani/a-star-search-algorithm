- Cercare IL MIGLIOR percorso: bisogna salvare temporaneamente quello trovato e tenere in considerazione il suo costo totale (f) e rimuovere dall'openset tutte le celle con costo maggiore o uguale di f.
- Trovare possibili sezioni di codice da poter eseguire in parallelo (stando attenti alle variabili condivise)

Problemi trovati:
- Inizialmente start e goal sono rispettivamente in posizione (0,0) e (ROW-1, COL-1). Se questi valori vengono cambiati può capitare che con la generazione randomica un ostacolo si trovi sopra start/goal. O lo lasciamo cosi per far vedere che prima di eseguire l'algoritmo facciamo un controllo per verificare che start/goal siano liberi, oppure possiamo fare in modo che con la generazione randomica gli ostacoli non si possano trovare nella posizione di start/goal (tenendo comunque in considerazione il primo punto di questa lista, ovvero se ho il 100% di ostacoli allora anche su start/goal ci devono essere ostacoli).
