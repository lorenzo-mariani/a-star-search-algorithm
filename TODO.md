- Trovare possibili sezioni di codice da poter eseguire in parallelo (stando attenti alle variabili condivise)

Problemi trovati:
- initCells(): si potrebbero inserire dei NULL nei valori di f, g, h, parentRow e parentCol? Da valutare se convenga o no!
- La rappresentazione di celle libere e ostacoli cambia tra la mappa stampata da printOnlyMap() (X e .) e la mappa stampata da printPath() (" " e -); si può usare solamente una notazione per evitare di fare confusione? Per esempio usando (" " e -) in entrambe le situazioni

CONSIDERAZIONI:
- seme costante per rand() in modo da poter fare considerazioni sulle performance usando sempre la stessa mappa
- la stampa di mappa/celle costa un sacco di tempo!