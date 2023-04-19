# Database System Technology course project
This is a repo for the project of Database System Technology course.

# Steps of project

## Step 1

- [x] Implement memtable (includes Red Black Tree as the self-balancing binary search tree).
- [x] Implement database Open, Get, Put, Scan, and Close APIs.
- [x] Implement writing memtable data to SST files.

## Step 2

- [x] Implement buffer pool using an extendible hash table.
- [x] Implement eviction policies (LRU and Clock).
- [x] Implement SST files to be static B-trees.
- [x] Implement Get/Scan from static B-tree SST files.

## Step 3

- [x] Implement LSM tree and its compaction process.
- [x] Implement bloom filter for level's SST files.
- [x] Implement Update and Delete APIs, which are handled as part of compaction process.
- [x] Implement LSM tree's Scan API.
- [x] Implement using bloom filters in Get API workflow.
- [x] Implement persisting bloom filters in db's buffer pool.

## Build Source

The project uses CMake. To build the source, run the following:
```shell
$ mkdir build 
$ cd build
$ cmake ..
$ make
```
This creates executable under the `build/` folder.

## Run Experiment

To run the entire experiment pipeline, simply run the shell script:
```shell
$ ./run-experiment.sh
```
If you want to run the experiment from a specific step, you can call the executable generated in the `build` folder:
```shell
$ cd build
$ ./experiments/exp <step number>
```

## Run Tests

Tests executables are also generated within the `build` folder. To run all the tests, go to this folder and run:
```shell
$ ./tests/test
```

You can view the project's report [here](report.pdf).
