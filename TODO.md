PROBLEMI:
- Celle libere e ostacoli rappresentati diversamente tra printOnlyMap() (X e .) e printPath() (" " e -). Usare unica notazione per non creare confusione (e.g., (" " e -) entrambe le volte)?

CONSIDERAZIONI UTILI PER IL REPORT:
- Seme costante per rand() in modo da poter fare considerazioni sulle performance usando sempre la stessa mappa
- Stampa di mappa/celle costa molto in termini di tempo

PARALLELIZZAZIONE (ATTENZIONE ALLE VARIABILI CONDIVISE):
- [riga 22-38] -> ogni thread esegue un if
- [riga 120-136] -> ogni thread esegue un if
- [riga 146-147] -> un thread per ogni operazione
- [riga 161-172] -> parallelizzazione ciclo for
- [riga 176-180] -> un thread per ogni operazione
- [riga 193-211] -> parallelizzazione ciclo for
- [riga 222-241] -> parallelizzazione ciclo for. Se non si riesce, provare a parallelizzare le operazioni dentro gli ultimi due if (riga 226-237)
- [riga 246-251] -> un thread per ogni operazione
- [riga 278-303] -> prime due operazioni + un thread per ogni vettore allocato dinamicamente
- [riga 313-321] -> parallelizzazione ciclo for
- [riga 330-331] -> un thread per ogni operazione
- [riga 340-344] -> un thread per ogni operazione
- [riga 361-367] -> un thread per ogni operazione
- [riga 418-420] -> un thread per ogni operazione
- [riga 424-437] -> parallelizzazione ciclo for
- [riga 447-???] -> vedere se si riesce a parallelizzare l'intero ciclo for