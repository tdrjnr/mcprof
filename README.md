﻿# MCProf #

`MCProf` is a memory and communication profiler. It traces memory reads/writes and reports memory accesses by various functions in the application as well as the data-communication between functions. 
`MCProf` relies on [Intel Pin](www.pintool.org) dynamic binary instrumentation framework.

# A picture is worth a thousand words #
Consider the following `C` code snippet in which some operations are performed on some arrays.

```c
#define SIZE 100
int *srcArr1, *srcArr2, *sumArr, *diffArr;

void initVecs() {
    for(i = 0; i < SIZE; i++) {
        *(srcArr1+i)=i*5 + 7;
        *(srcArr2+i)=2*i - 3;
    }
}
void sumVecs(){
    for(i = 0; i < SIZE; i++)
        *(sumArr+i) = *(srcArr1+i) + (*(srcArr2+i));
}
void diffVecs(){
    for(i = 0; i < SIZE; i++)
        *(diffArr+i) = *(srcArr1+i) + (*(srcArr2+i));
}
int main() {
    srcArr1 = malloc( SIZE*sizeof(int) );
    //simmilarly, other allocations

    initVecs();
    for(j=0;j<3;j++)    sumVecs();
    for(j=0;j<5;j++)    diffVecs();
    printf("output : %d\n", sumArr[1]+diffArr[1]);

    free(srcArr1);
    // simmilarly, other memory frees
    return 0;
}
```

An example application and the `MCProf` generated communication graph is shown in the figure below.

![graph](https://bitbucket.org/imranashraf/mcprof/raw/master/doc/example/exampleGraph.png)

This README file briefly explains the process of setting up `MCProf` and start generating such graphs for C/C++ applications.

## Licensing

Copyright (c) 2014-2015 TU Delft, The Netherlands.
All rights reserved.

MCProf is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

MCProf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with MCProf.  If not, see <http://www.gnu.org/licenses/>.


## Reference

**To cite MCProf Tool:**
```
"MCProf: Open-source Memory and Communication Profiler"
Imran Ashraf
URL: https://bitbucket.org/imranashraf/mcprof
```

**To cite MCProf Design OR MCProf Shadow Memory Technique:**
```
"Intra-Application Data-Communication Characterization"
Imran Ashraf, Vlad-Mihai Sima and Koen Bertels
Technical report, Delft University of Technology, 2015
```


## Download

`MCProf` can be downloaded [from](https://bitbucket.org/imranashraf/mcprof/downloads).

## Required Packages

In order to setup and use `MCProf` the following two packages are required:

* [Intel Pin DBI framework](www.pintool.org) Revision 62732 or higher
* g++ compiler
* libelf library to read static symbols from the binary
* graphviz Dot utility for converting the generated communication graphs from dot to pdf, png etc

## Setup

`MCProf` uses Makefile to compile the sources. In order to compile `MCProf` from sources on 32-bit / 64-bit Linux, the following steps can be performed.

* Download Pin and copy and extract it to the directory where you want to keep Pin.

* Define a variable `PIN_ROOT` by running the following commands:

        export PIN_ROOT=/<path to pin>

* Add Pin to your path by the following command:

        export PATH=$PIN_ROOT:$PATH

* You can also add these lines, for instance, to your `.bashrc` in case you are using `bash` to export these variable automatically on opening a terminal.

* Download `MCProf` and copy and extract it to the directory where you want to compile it.

* Go the `MCProf` directory and run the following command to compile it:

        make

If every thing goes fine, you will see `mcprof.so` in `obj-intel64` direcoty (or `obj-ia32` directory depending upon your architecture).

## Usage
`MCProf` usage is explained here for the given tests available in `test` directory as well as a standalone application.

#### Running Given Test Examples
Various test examples are available in `tests` directory inside `MCProf`. These tests are also compiled to binaries with the compilation process and are available as executables in obj-intel64/obj-ia32 directory. In order to run `MCProf` for one of these tests (e.g. vecOps.cpp) you can run (from the `MCProf` directory) the following command:

    make vecOps.test

This will create `communication.dot` file which can be converted to pdf file by the following command:

    dot -Tpdf communication.dot -o communication.pdf
    
A script `dot2pdf.sh` is also provided in the `scripts` directory which can also be used to convert the graphs in `dot` format to `pdf` format. This script also performs some extra tasks, for instance, remove unconnected nodes in the graph. This script can be used as:

    <path to mcprof dir>/scripts/dot2pdf.sh

#### Profiling your own application
In order to use `MCProf` to analyze your application, you should compile your application with  `-g -O1 -fno-inline` compiler flags. For instance if the name of your application binary is`yourAppBinary` then you can use `MCProf` as:

    /<path to pin dir>/pin -t <path to mcprof dir>/obj-intel64/mcprof.so -- <path to your binary>/<yourAppBinary>

This generates `communication.dot` file which can be converted to pdf (or some other formats of your interest) for visualization.

To simplify this process and give a complete example, a test application containing the `makefile` showing the complete process is available in `tests/yourApp` example. Feel free to copy it to any place you like and modify the paths in the first two lines to point to Pin and path to `MCProf` directory. After this, you can compile this application and profile it by `MCProf` by the following command:

    make mcprof


## Detailed Documentation

Detailed documentation of `MCProf` with input options and outputs generated can be found in the [manual](https://bitbucket.org/imranashraf/mcprof/raw/master/doc/manual.pdf) available in the  [doc](https://bitbucket.org/imranashraf/mcprof/raw/master/doc) directory.


## Whom do I talk to?

In case you are interested in contributing to `MCProf`, or you have questions orsuggestions for improvements, or you want to report a bug, contact:

* Imran Ashraf [I.Ashraf@TUDelft.nl](I.Ashraf@TUDelft.nl)