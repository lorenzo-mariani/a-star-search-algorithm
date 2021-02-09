- Cercare IL MIGLIOR percorso: bisogna salvare temporaneamente quello trovato e tenere in considerazione il suo costo totale (f) e rimuovere dall'openset tutte le celle con costo maggiore o uguale di f.
- Trovare possibili sezioni di codice da poter eseguire in parallelo (stando attenti alle variabili condivise)


--> ATTENZIONE: a volte non bisogna fare il check "fin quando opensetsize>0", ma piuttosto "fin quando nessuna cella dell'openset ha F minore di quella del goal raggiunto". ved. line 233.