- Cercare IL MIGLIOR percorso: bisogna salvare temporaneamente quello trovato e tenere in considerazione il suo costo totale (f) e rimuovere dall'openset tutte le celle con costo maggiore o uguale di f.
- Trovare possibili sezioni di codice da poter eseguire in parallelo (stando attenti alle variabili condivise)

Problemi trovati:
- Quando OBSTACLES = 100 le posizioni (0,0) e (ROW-1, COL-1) (sarebbero le posizioni iniziali di start/goal) della mappa vengono comunque lasciate libere (in teoria se la percentuale di ostacoli è 100%, anche su start/goal dovrebbero esserci degli ostacoli). Esempio:

 .  X   X   X   X   X    
 X  X   X   X   X   X    
 X  X   X   X   X   X    
 X  X   X   X   X   X    
 X  X   X   X   X   . 

NOTA! In realtà, anche cambiando la percentuale di OBSTACLES le posizioni (0,0) e (ROW-1, COL-1) vegono SEMPRE lasciate libere.
- Se viene impostato un valore negativo di OBSTACLES, è come se il meno non venisse preso in considerazione (es. OBSTACLES -50 viene considerato come OBSTACLES 50). In teoria ha senso, perchè avere una percentuale negativa di ostacoli è impossibile, il punto è che non so se sia giusto per pura casualità o perchè c'è un controllo nel codice che considera solo il modulo.
- Inizialmente start e goal sono rispettivamente in posizione (0,0) e (ROW-1, COL-1). Se questi valori vengono cambiati può capitare che con la generazione randomica un ostacolo si trovi sopra start/goal. O lo lasciamo cosi per far vedere che prima di eseguire l'algoritmo facciamo un controllo per verificare che start/goal siano liberi, oppure possiamo fare in modo che con la generazione randomica gli ostacoli non si possano trovare nella posizione di start/goal (tenendo comunque in considerazione il primo punto di questa lista, ovvero se ho il 100% di ostacoli allora anche su start/goal ci devono essere ostacoli).
- Se OBSTACLES 0 (percentuale di ostacoli pari a 0%) non viene stampato nulla e viene mostrato un warning alla riga 24 perchè viene eseguita una divisione per 0.
- Provando ad inserire un valore sbagliato di CONNECTIVITY (es. CONNECTIVITY 30), viene preso come valore di default CONNECTIVITY 8. Bisognerebbe inserire nella funzione "heurisitc", al posto dell'else, un else if CONNECTIVITY 8 e aggiungere un nuovo else (per tutti i valori sbagliati), che ti avvisi dell'errore.
- Se OBSTACLES assume valori da 101 (compreso) in poi, non viene stampato nulla (penso sia dovuto alla riga 24 in cui viene fatta una divisione e il risultato viene 0). Bisognerebbe inserire anche qui un controllo per verificare che OBSTACLES assuma valori da 0% a 100% e basta, altrimenti viene stampato un messaggio di errore.
