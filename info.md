A star Algorithm (in C), serial and parallel versions

Andrea Fecchio and Lorenzo Mariani, 2021
Course of Advanced Computer Architecture, University of Pavia - Italy

...

Link to the report: https://docs.google.com/document/d/1CGENxiXbEKRIlrjj887XKV81zFnDt3WyJH0RihnapEo/edit?usp=sharing

...

Mode of use:
- Create **/maps** and **/paths** directories
- Build the map with the desired parameters, by compiling and launching *create-map.c*
- Launch the a-star algorithm (*a-star-serial.c* or *a-star-parallel.c*), with the same parameters (DIM, OBSTACLES) used in *create-map.c*. **NOTE!** Compile the files in this way: *gcc a-star-serial.c -o a-star-serial -lm -fopenmp*
- The found path is in the **/paths** folder.
