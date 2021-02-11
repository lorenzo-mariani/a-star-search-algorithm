- Cercare IL MIGLIOR percorso: bisogna salvare temporaneamente quello trovato e tenere in considerazione il suo costo totale (f) e rimuovere dall'openset tutte le celle con costo maggiore o uguale di f.
- Trovare possibili sezioni di codice da poter eseguire in parallelo (stando attenti alle variabili condivise)


--> ATTENZIONE: a volte non bisogna fare il check "fin quando opensetsize>0", ma piuttosto "fin quando nessuna cella dell'openset ha F minore di quella del goal raggiunto". ved. line 233.


--> ATTENZIONE: ho chiuso il goal dietro un muro (ved. funzione fillMap())


--> CASO DI ERRORE 2: 

. . . . . . .
. . . . X . .
. . . . . . .
. X . X . X .
. X X . . . .
. . X X . . .
. . . X . . X

Cella open set: 6 2 with f=0.000000, g=0.000000, h=0.000000, con genitore: 393216 0...

>>> Cella corrente: 6 2, con genitore: 6 2
   Neighbor: 5 1 , now opensetSize=1
   Neighbor: 6 1 , now opensetSize=2

Cella open set: 5 1 with f=7.817338, g=1.414214, h=6.403124, con genitore: 393216 859819507...
Cella open set: 6 1 with f=8.071068, g=1.000000, h=7.071068, con genitore: 393216 1017184...

>>> Cella corrente: 5 1, con genitore: 6 2
   Neighbor: 4 0 , now opensetSize=2. closedsetSize=2
   Neighbor: 5 0 , now opensetSize=3. closedsetSize=2
   Neighbor: 6 0 , now opensetSize=4. closedsetSize=2
   Neighbor: 6 1    Neighbor: 6 2
Cella open set: 6 1 with f=8.071068, g=1.000000, h=7.071068, con genitore: 393216 1017184...
Cella open set: 4 0 with f=9.536631, g=2.828427, h=6.708204, con genitore: 393216 1313982065...
Cella open set: 5 0 with f=9.625316, g=2.414214, h=7.211103, con genitore: 393216 1862308979...
Cella open set: 6 0 with f=10.638677, g=2.828427, h=7.810250, con genitore: 393216 -1522071257...

>>> Cella corrente: 6 1, con genitore: 5 1
   Neighbor: 5 0    Neighbor: 5 1    Neighbor: 6 0    Neighbor: 6 2
Cella open set: 4 0 with f=9.536631, g=2.828427, h=6.708204, con genitore: 393216 1313982065...
Cella open set: 5 0 with f=9.625316, g=2.414214, h=7.211103, con genitore: 393216 1862308979...
Cella open set: 6 0 with f=9.810250, g=2.000000, h=7.810250, con genitore: 393216 195508788...

>>> Cella corrente: 4 0, con genitore: 5 1
   Neighbor: 3 0 , now opensetSize=3. closedsetSize=4
   Neighbor: 5 0    Neighbor: 5 1
Cella open set: 5 0 with f=9.625316, g=2.414214, h=7.211103, con genitore: 393216 1862308979...
Cella open set: 6 0 with f=9.810250, g=2.000000, h=7.810250, con genitore: 393216 195508788...
Cella open set: 3 0 with f=10.152982, g=3.828427, h=6.324555, con genitore: 393216 -1223885667...

>>> Cella corrente: 5 0, con genitore: 4 0
   Neighbor: 4 0    Neighbor: 5 1    Neighbor: 6 0    Neighbor: 6 1
Cella open set: 6 0 with f=9.810250, g=2.000000, h=7.810250, con genitore: 393216 195508788...
Cella open set: 3 0 with f=10.152982, g=3.828427, h=6.324555, con genitore: 393216 -1223885667...

>>> Cella corrente: 6 0, con genitore: 5 0
   Neighbor: 5 0    Neighbor: 5 1    Neighbor: 6 1
Cella open set: 3 0 with f=10.152982, g=3.828427, h=6.324555, con genitore: 393216 -1223885667...

>>> Cella corrente: 3 0, con genitore: 4 0
   Neighbor: 2 0    Neighbor: 2 1    Neighbor: 4 0
--------------------------------
Process exited after 5.152 seconds with return value 3221225477