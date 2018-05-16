# NW Analysis of Korean Data
ROOT code and libraries for analysis of digitized waveforms

## Prerequisites
  - [ROOT](https://root.cern.ch/) v6+
  - gcc 4.7.x
  
## Building and running
  1. Checkout the git repo and build source
```    
    $ git clone https://github.com/nscl-hira/NW-FADC.git
    $ cd NW-FADC
    $ make
``` 
  2. Start root from this directory to load the library automatically.
  3. Directory can be cleaned with `$ make clean`
 
## Updating
The repo can be brought to the most recent version with 
```
  $ git fetch origin master
  $ git reset --hard FETCH_HEAD
```
The source will have to be rebuilt.
