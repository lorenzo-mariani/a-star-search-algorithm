A star Algorithm (in C), serial and parallel versions

Andrea Fecchio and Lorenzo Mariani, 2021
Course of Advanced Computer Architecture, University of Pavia - Italy

...

Link to the report: https://docs.google.com/document/d/1CGENxiXbEKRIlrjj887XKV81zFnDt3WyJH0RihnapEo/edit?usp=sharing

...

Mode of use:
- create /maps and /paths directories
- compile the files this way: gcc a-star-serial.c -o a-star-serial -lm -fopenmp
- build the map with the desired parameters, by compiling and launching create-map
- launch the a-star algoithm, with the same parameters (DIM, OBSTACLES)
- the found path is in the /paths folder.
