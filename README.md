# A* search algorithm
Authors: Lorenzo Mariani, Andrea Fecchio

## Table of contents

- [Purpose](#Purpose)
- [Installation](#Installation)
- [Usage](#Usage)

## Purpose

Serial and parallel implementation of the A* algorithm, one of the best search algorithms to find the shortest path between nodes or graphs. The project was realized for the course of Advanced Computer Architecture, University of Pavia - Italy.

Link to the report: https://docs.google.com/document/d/1CGENxiXbEKRIlrjj887XKV81zFnDt3WyJH0RihnapEo/edit?usp=sharing

## Installation

To install adgen follow the steps below:

    git clone https://github.com/lorenzo-mariani/a-star-search-algorithm
    cd a-star-search-algorithm
    
## Usage

Once installed, you have to follow the following steps:
- Create two directiores, called **_/maps_** and **_/paths_** respectively
- Build the map with the desired parameters, by compiling and launching _create-map.c_
- Launch the a-star algorithm (_a-star-serial.c_ or _a-star-parallel.c_), with the same parameters (DIM, OBSTACLES) used in _create-map.c_
- The found path is in the **_/paths_** folder

**_Note!_** When you run the program in the parallel version, compile the file in this way:

    gcc a-star-parallel.c -o a-star-parallel -lm -fopenmp*
    
