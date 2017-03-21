# Interpreter

Project for the PB173 course @ FI MU

Implementation of interpreter of my custom programming language. You can build it by following shell commands. C++ compiler with support of c++14 is required.

```shell
mkdir _build
cd _build/
cmake ..
make
```
Automated tests are part of a build. You can find test binary in *test/* folder. Path to interpreter binary is *src/Interpreter*. You should specify name of the input file in the first command line argument as follows:
```shell
./Interpreter <path/to/input/file>
```
